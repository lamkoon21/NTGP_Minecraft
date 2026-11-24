#include "net_client.h"
#include "../protocol/protocol.h"
#include <atomic>
#include <queue>
#include "../character/character.h"

static constexpr int SELECT_TIMEOUT_MS = 200;
static constexpr DWORD PING_INTERVAL_MS = 1000;
static constexpr DWORD CONNECT_TIMEOUT_MS = 5000;
static constexpr DWORD JOIN_TIMEOUT_MS = 10000;
static constexpr DWORD SEND_TIMEOUT_MS = 5 * 60 * 1000;

// 수신 큐에 복사해서 보관할 패킷 데이터
typedef struct ReceivedPacket
{
	char data[MAX_PACKET_SIZE];
} ReceivedPacket;

struct JoinResult
{
	bool success = false;
	uint8_t playerId = MAX_PLAYERS;
	int32_t seed = 0;
	uint8_t tickRate = 0;
	uint32_t initialPacketCount = 0;
	uint8_t rejectReason = 0;
	const char* errorMessage = "";
};

typedef struct NetClient
{
	SOCKET sock;
	HANDLE recvThread;
	std::vector<Character*> remotes;	// 내 캐릭터를 제외한 원격 플레이어 목록
	bool winsockStarted = false;		// Winsock 초기화 성공 여부 기록, 재접속 시 중복 초기화 방지

	std::queue<ReceivedPacket> packets;	// 게임 갱신에서 처리할 수신 패킷 목록
	CRITICAL_SECTION queueCS;			// 수신 스레드와 게임 갱신의 큐 동시 접근 방지
	bool queueCSInitialized = false;	// 연결 해제 시 임계 영역 정리 여부 확인
	std::atomic<bool> disconnected{};	// 게임 갱신에서 서버 연결 끊김 확인
	uint8_t playerId = MAX_PLAYERS;		// 원격 플레이어 처리에서 내 ID 구분
	HANDLE quitEvent = NULL;				// 종료 요청 (수동 리셋)
	HANDLE sendEvent = NULL;				// 송신 알림 (자동 리셋)
	CRITICAL_SECTION sendCS;
	std::vector<char> sendBuf;

	// ConnectThread에서 사용하는 접속 정보와 결과, 송신 스레드 핸들
	char connectIp[INET_ADDRSTRLEN]{};
	HANDLE connectDoneEvent = NULL;
	HANDLE sendThread = NULL;
	JoinResult joinResult;

} NetClient;

// 서버 연결 소켓과 수신 스레드 핸들을 저장하는 전역 변수
static NetClient net = { INVALID_SOCKET, NULL, {} };
static SOCKET pendingJoinSocket = INVALID_SOCKET;
static uint8_t pendingJoinPlayerId = MAX_PLAYERS;

DWORD WINAPI ConnectThread(LPVOID arg)
{
	NetClient* client = (NetClient*)arg;
	if (client == NULL)
		return 0;

	// 접속에 필요한 이벤트와 이전 연결 자원의 정리 여부 확인
	if (client->quitEvent == NULL || client->connectDoneEvent == NULL ||
		client->sock != INVALID_SOCKET || client->recvThread != NULL ||
		client->sendThread != NULL || client->sendEvent != NULL ||
		client->queueCSInitialized ||
		(client == &net && pendingJoinSocket != INVALID_SOCKET)) {
		client->joinResult = {};
		client->joinResult.errorMessage = "Connection is not ready";
		if (client->connectDoneEvent != NULL)
			SetEvent(client->connectDoneEvent);
		return 0;
	}

	client->disconnected = false;
	JoinResult result;
	bool sendCSInitialized = false;
	SOCKET sock = INVALID_SOCKET;
	WSAEVENT socketEvent = WSA_INVALID_EVENT;
	const char* error = "Connection failed";

	// 각 단계에서 실패하면 아래의 공통 자원 정리로 이동
	do {
		DWORD quitStatus = WaitForSingleObject(client->quitEvent, 0);
		if (quitStatus != WAIT_TIMEOUT) {
			error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
			break;
		}

		// Winsock 초기화 및 재접속 시 중복 초기화 방지
		if (!client->winsockStarted) {
			WSADATA data{};
			if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
				error = "Winsock initialization failed";
				break;
			}
			client->winsockStarted = true;
		}

		// IPv4 형식 검사 및 서버 접속 주소 구성
		sockaddr_in address{};
		address.sin_family = AF_INET;
		address.sin_port = htons(SERVER_PORT);
		if (inet_pton(AF_INET, client->connectIp, &address.sin_addr) != 1) {
			error = "Invalid IPv4 address";
			break;
		}
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET)
			break;

		// 연결 이벤트 등록 및 소켓의 논블로킹 모드 전환
		socketEvent = WSACreateEvent();
		if (socketEvent == WSA_INVALID_EVENT ||
			WSAEventSelect(sock, socketEvent, FD_CONNECT) == SOCKET_ERROR)
			break;

		// 취소 이벤트를 먼저 두어 두 신호가 겹치면 취소 우선
		const HANDLE events[2] = { client->quitEvent, socketEvent };

		// TCP 연결 요청 및 최대 5초 동안 연결 결과 대기
		const ULONGLONG connectDeadline = GetTickCount64() + CONNECT_TIMEOUT_MS;
		bool connected = connect(sock, (const sockaddr*)&address, sizeof(address)) == 0;
		if (!connected && WSAGetLastError() != WSAEWOULDBLOCK)
			break;

		while (!connected) {
			quitStatus = WaitForSingleObject(client->quitEvent, 0);
			if (quitStatus != WAIT_TIMEOUT) {
				error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
				break;
			}
			const ULONGLONG now = GetTickCount64();
			if (now >= connectDeadline) {
				error = "Connection timed out";
				break;
			}

			const DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, (DWORD)(connectDeadline - now));
			quitStatus = WaitForSingleObject(client->quitEvent, 0);
			if (quitStatus != WAIT_TIMEOUT) {
				error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
				break;
			}
			if (waitResult != WAIT_OBJECT_0 + 1) {
				error = waitResult == WAIT_TIMEOUT ? "Connection timed out" : "Socket event wait failed";
				break;
			}

			WSANETWORKEVENTS socketEvents{};
			if (WSAEnumNetworkEvents(sock, socketEvent, &socketEvents) == SOCKET_ERROR) {
				error = "Socket event read failed";
				break;
			}
			if (socketEvents.lNetworkEvents & FD_CONNECT) {
				connected = socketEvents.iErrorCode[FD_CONNECT_BIT] == 0;
				break;
			}
		}
		if (!connected)
			break;

		quitStatus = WaitForSingleObject(client->quitEvent, 0);
		if (quitStatus != WAIT_TIMEOUT) {
			error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
			break;
		}

		// TCP_NODELAY와 블로킹 송신 제한 시간 설정, 송수신 및 연결 종료 이벤트 등록
		BOOL nodelay = TRUE;
		DWORD sendTimeout = SEND_TIMEOUT_MS;
		if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay)) == SOCKET_ERROR ||
			setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&sendTimeout, sizeof(sendTimeout)) == SOCKET_ERROR ||
			WSAEventSelect(sock, socketEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) {
			error = "Socket setup failed";
			break;
		}

		// CS_HELLO 전송부터 입장 응답 수신까지 10초 제한
		const ULONGLONG joinDeadline = GetTickCount64() + JOIN_TIMEOUT_MS;
		CS_HELLO_PACKET hello{};
		PacketHeader header{};
		char buffer[MAX_PACKET_SIZE];
		int sent = 0;
		int received = 0;
		int packetSize = sizeof(PacketHeader);
		bool responseComplete = false;

		while (!responseComplete) {
			quitStatus = WaitForSingleObject(client->quitEvent, 0);
			if (quitStatus != WAIT_TIMEOUT) {
				error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
				break;
			}
			if (GetTickCount64() >= joinDeadline) {
				error = "Join response timed out";
				break;
			}

			// 인사 패킷을 모두 보낸 뒤 응답을 수신, 일부만 처리되면 남은 바이트를 이어서 처리
			const bool sending = sent < (int)sizeof(hello);
			int bytes;
			if (sending)
				bytes = send(sock, (const char*)&hello + sent, (int)sizeof(hello) - sent, 0);
			else
				bytes = recv(sock, buffer + received, packetSize - received, 0);

			if (bytes > 0) {
				if (sending) {
					sent += bytes;
				}
				else {
					received += bytes;
					// 헤더로 승인과 거절을 구분하고, 해당 패킷 크기만큼만 수신
					if (received == (int)sizeof(PacketHeader)) {
						memcpy(&header, buffer, sizeof(header));
						if ((header.type != SC_JOIN_OK || header.size != sizeof(SC_JOIN_OK_PACKET)) &&
							(header.type != SC_JOIN_REJECT || header.size != sizeof(SC_JOIN_REJECT_PACKET))) {
							error = "Invalid join response";
							break;
						}
						packetSize = header.size;
					}
					responseComplete = received == packetSize;
				}
				continue;
			}
			if (bytes == 0 || WSAGetLastError() != WSAEWOULDBLOCK) {
				error = "Server connection closed during join";
				break;
			}

			// 송수신할 수 있을 때까지 소켓 이벤트와 취소 이벤트 대기
			const ULONGLONG now = GetTickCount64();
			if (now >= joinDeadline) {
				error = "Join response timed out";
				break;
			}
			const DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, (DWORD)(joinDeadline - now));
			quitStatus = WaitForSingleObject(client->quitEvent, 0);
			if (quitStatus != WAIT_TIMEOUT) {
				error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
				break;
			}
			if (waitResult != WAIT_OBJECT_0 + 1) {
				error = waitResult == WAIT_TIMEOUT ? "Join response timed out" : "Socket event wait failed";
				break;
			}

			WSANETWORKEVENTS socketEvents{};
			if (WSAEnumNetworkEvents(sock, socketEvent, &socketEvents) == SOCKET_ERROR) {
				error = "Socket event read failed";
				break;
			}
		}
		if (!responseComplete)
			break;

		// 입장 거절 사유와 오류 문구 저장
		if (header.type == SC_JOIN_REJECT) {
			SC_JOIN_REJECT_PACKET reject{};
			memcpy(&reject, buffer, sizeof(reject));
			result.rejectReason = reject.reason;
			error = reject.reason == JOIN_REJECT_FULL ? "Server is full" :
				reject.reason == JOIN_REJECT_VERSION ? "Protocol version mismatch" : "Join rejected";
			break;
		}

		// 입장 승인 정보와 플레이어 ID, 틱 속도 검사
		SC_JOIN_OK_PACKET packet{};
		memcpy(&packet, buffer, sizeof(packet));
		if (packet.id >= MAX_PLAYERS || packet.tick_rate == 0) {
			error = "Invalid join response";
			break;
		}
		quitStatus = WaitForSingleObject(client->quitEvent, 0);
		if (quitStatus != WAIT_TIMEOUT) {
			error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
			break;
		}

		// 소켓 이벤트 등록 해제 및 블로킹 모드 복구
		u_long nonblocking = 0;
		if (WSAEventSelect(sock, NULL, 0) == SOCKET_ERROR ||
			ioctlsocket(sock, FIONBIO, &nonblocking) == SOCKET_ERROR) {
			error = "Socket setup failed";
			break;
		}
		WSACloseEvent(socketEvent);
		socketEvent = WSA_INVALID_EVENT;

		// 송수신 임계 영역과 송신 알림 이벤트 초기화
		error = "Network worker setup failed";
		if (!InitializeCriticalSectionAndSpinCount(&client->queueCS, 0))
			break;
		client->queueCSInitialized = true;
		if (!InitializeCriticalSectionAndSpinCount(&client->sendCS, 0))
			break;
		sendCSInitialized = true;
		client->sendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (client->sendEvent == NULL)
			break;
		client->sock = sock;
		client->playerId = packet.id;

		quitStatus = WaitForSingleObject(client->quitEvent, 0);
		if (quitStatus != WAIT_TIMEOUT) {
			error = quitStatus == WAIT_OBJECT_0 ? "Connection cancelled" : "Cancel event failed";
			break;
		}

		// 후속 패킷 수신과 버퍼 전송을 담당할 스레드 생성
		client->recvThread = CreateThread(NULL, 0, RecvThread, client, 0, NULL);
		if (client->recvThread == NULL)
			break;
		client->sendThread = CreateThread(NULL, 0, SendThread, client, 0, NULL);
		if (client->sendThread == NULL)
			break;

		// 송수신 시작 후에는 취소와 연결 끊김이 같은 종료 이벤트를 사용
		quitStatus = WaitForSingleObject(client->quitEvent, 0);
		if (quitStatus != WAIT_TIMEOUT) {
			error = quitStatus == WAIT_OBJECT_0 ? "Connection interrupted" : "Cancel event failed";
			break;
		}
		if (client->disconnected.load()) {
			error = "Connection interrupted";
			break;
		}

		// 메인 스레드에 전달할 입장 결과 저장
		result.success = true;
		result.playerId = packet.id;
		result.seed = packet.seed;
		result.tickRate = packet.tick_rate;
		result.initialPacketCount = packet.initial_packet_count;
	} while (false);

	if (!result.success) {
		client->disconnected = true;
		SetEvent(client->quitEvent);

		// 생성된 스레드의 종료 확인 후 소켓과 공유 자원 정리
		HANDLE* threads[2] = { &client->recvThread, &client->sendThread };
		for (int i = 0; i < 2; ++i) {
			if (*threads[i] != NULL) {
				WaitForSingleObject(*threads[i], INFINITE);
				CloseHandle(*threads[i]);
				*threads[i] = NULL;
			}
		}
		if (sock != INVALID_SOCKET)
			closesocket(sock);
		client->sock = INVALID_SOCKET;
		client->playerId = MAX_PLAYERS;
		if (client->sendEvent != NULL) {
			CloseHandle(client->sendEvent);
			client->sendEvent = NULL;
		}
		if (sendCSInitialized) {
			DeleteCriticalSection(&client->sendCS);
		}
		if (client->queueCSInitialized) {
			DeleteCriticalSection(&client->queueCS);
			client->queueCSInitialized = false;
		}
		std::queue<ReceivedPacket>().swap(client->packets);
		client->sendBuf.clear();
		result.errorMessage = error;
	}
	if (socketEvent != WSA_INVALID_EVENT)
		WSACloseEvent(socketEvent);

	// 결과 저장 후 완료 이벤트로 메인 스레드에 알림
	client->joinResult = result;
	SetEvent(client->connectDoneEvent);
	return 0;
}

DWORD WINAPI RecvThread(LPVOID arg)
{
	NetClient* client = (NetClient*)arg;

	if (client == NULL || client->sock == INVALID_SOCKET)
		return 0;

	const SOCKET sock = client->sock;
	const int BUF_SIZE = 4096;
	char buf[BUF_SIZE];
	int len = 0;

	bool receiving = true;
	while (receiving && (client->quitEvent == NULL ||
		WaitForSingleObject(client->quitEvent, 0) == WAIT_TIMEOUT))
	{
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(sock, &readset);
		timeval timeout = { 0, SELECT_TIMEOUT_MS * 1000 };
		const int ready = select(0, &readset, NULL, NULL, &timeout);
		if ((client->quitEvent != NULL && WaitForSingleObject(client->quitEvent, 0) != WAIT_TIMEOUT) ||
			ready == SOCKET_ERROR)
			break;
		if (ready == 0)
			continue;

		// 버퍼의 남은 공간에 서버 데이터 수신
		const int received = recv(sock, buf + len, BUF_SIZE - len, 0);
		if (received <= 0)
			break;
		len += received;

		while (client->quitEvent == NULL || WaitForSingleObject(client->quitEvent, 0) == WAIT_TIMEOUT)
		{
			// 완성된 패킷 추출, 아직 덜 받은 데이터는 다음 수신까지 보관
			ReceivedPacket packet{};
			const ExtractResult result = ExtractPacket(buf, len, packet.data);
			if (result == EXTRACT_INCOMPLETE)
				break;

			// 잘못된 패킷 크기 수신 시 끊김 표시 후 수신 종료
			if (result == EXTRACT_INVALID)
			{
				receiving = false;
				break;
			}

			// 임계 영역 안에서 수신 패킷을 큐에 복사
			EnterCriticalSection(&client->queueCS);
			client->packets.push(packet);
			LeaveCriticalSection(&client->queueCS);
		}
	}

	// 서버 종료 또는 수신 오류를 게임 갱신에 전달
	client->disconnected = true;
	if (client->quitEvent != NULL)
		SetEvent(client->quitEvent);

	return 0;
}

// 송신 버퍼 전송
DWORD WINAPI SendThread(LPVOID arg)
{
	NetClient* client = (NetClient*)arg;
	if (client == NULL || client->sock == INVALID_SOCKET ||
		client->quitEvent == NULL || client->sendEvent == NULL)
		return 0;

	const HANDLE events[] = { client->quitEvent, client->sendEvent };
	std::vector<char> buffer;
	ULONGLONG nextPing = GetTickCount64() + PING_INTERVAL_MS;
	while (true)
	{
		const ULONGLONG now = GetTickCount64();
		const DWORD timeout = now < nextPing ? static_cast<DWORD>(nextPing - now) : 0;
		const DWORD result = WaitForMultipleObjects(2, events, FALSE, timeout);
		if ((result != WAIT_OBJECT_0 + 1 && result != WAIT_TIMEOUT) ||
			WaitForSingleObject(client->quitEvent, 0) != WAIT_TIMEOUT)
			break;

		EnterCriticalSection(&client->sendCS);
		buffer.swap(client->sendBuf);
		LeaveCriticalSection(&client->sendCS);

		// 1초마다 핑 추가
		if (GetTickCount64() >= nextPing)
		{
			CS_PING_PACKET ping{};
			const char* data = reinterpret_cast<const char*>(&ping);
			buffer.insert(buffer.end(), data, data + sizeof(ping));
			nextPing = GetTickCount64() + PING_INTERVAL_MS;
		}

		if (!buffer.empty() && !SendBuffer(client->sock, buffer.data(), static_cast<int>(buffer.size())))
			break;
		buffer.clear();
	}

	client->disconnected = true;
	SetEvent(client->quitEvent);
	return 0;
}

const std::vector<Character*>& Remotes()
{
	return net.remotes;
}

// 내 플레이어의 위치, 시선, 든 블록 정보를 서버로 전송
void SendMove(const Character* player)
{
	// 싱글플레이, 연결 끊김, 플레이어가 없는 경우에는 보내지 않는다.
	if (player == nullptr ||
		net.sock == INVALID_SOCKET ||
		net.disconnected.load())
	{
		return;
	}

	CS_MOVE_PLAYER_PACKET packet{};

	packet.state.x = player->Position.x;
	packet.state.y = player->Position.y;
	packet.state.z = player->Position.z;
	packet.state.yaw = player->camera.Yaw;
	packet.state.pitch = player->camera.Pitch;
	packet.state.block_kind =
		static_cast<uint8_t>(player->block_kind);

	// 부분 송신 처리는 공통 SendPacket()이 담당한다.
	if (!SendPacket(net.sock, &packet))
	{
		net.disconnected = true;
	}
}

bool ConnectServer(const char* ipv4, uint8_t& outPlayerId, int& outSeed, HANDLE cancelEvent)
{
	// 서버 주소가 없거나 이미 연결된 경우 접속 요청 거절
	if (ipv4 == nullptr || net.sock != INVALID_SOCKET || net.recvThread != NULL ||
		pendingJoinSocket != INVALID_SOCKET)
		return false;

	if (cancelEvent != NULL && WaitForSingleObject(cancelEvent, 0) == WAIT_OBJECT_0)
		return false;

	// IPv4 입력값 확인 및 서버 포트를 포함한 접속 주소 구성
	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, ipv4, &address.sin_addr) != 1)
		return false;

	// Winsock 초기화
	if (!net.winsockStarted) {
		WSADATA data{};
		if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
			return false;
		net.winsockStarted = true;
	}

	// TCP 소켓 생성
	const SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		return false;

	const WSAEVENT socketEvent = WSACreateEvent();
	if (socketEvent == WSA_INVALID_EVENT ||
		WSAEventSelect(sock, socketEvent, FD_CONNECT) == SOCKET_ERROR) {
		closesocket(sock);
		if (socketEvent != WSA_INVALID_EVENT)
			WSACloseEvent(socketEvent);
		return false;
	}

	const HANDLE events[] = { socketEvent, cancelEvent };
	const DWORD eventCount = cancelEvent == NULL ? 1 : 2;

	// 서버에 TCP 연결 요청
	bool connected = connect(sock, (const sockaddr*)&address, sizeof(address)) == 0;
	if (!connected && WSAGetLastError() == WSAEWOULDBLOCK &&
		WaitForMultipleObjects(eventCount, events, FALSE, INFINITE) == WAIT_OBJECT_0) {
		WSANETWORKEVENTS socketEvents{};
		connected = WSAEnumNetworkEvents(sock, socketEvent, &socketEvents) == 0 &&
			(socketEvents.lNetworkEvents & FD_CONNECT) &&
			socketEvents.iErrorCode[FD_CONNECT_BIT] == 0;
	}

	// 작은 패킷의 전송 지연을 줄이기 위한 TCP_NODELAY 설정
	BOOL nodelay = TRUE;
	SC_JOIN_OK_PACKET packet{};
	const int packetSize = sizeof(packet);
	int received = 0;

	if (connected &&
		WSAEventSelect(sock, socketEvent, FD_READ | FD_CLOSE) == 0 &&
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay)) == 0) {
		// 서버 입장 제한 시간 10초 적용
		const ULONGLONG deadline = GetTickCount64() + 10000;
		while (received < packetSize) {
			const ULONGLONG now = GetTickCount64();
			if (now >= deadline)
				break;

			const int bytes = recv(sock, (char*)&packet + received, packetSize - received, 0);
			if (bytes > 0) {
				received += bytes;
				continue;
			}
			if (bytes == 0 || WSAGetLastError() != WSAEWOULDBLOCK)
				break;

			WSANETWORKEVENTS socketEvents{};
			if (WaitForMultipleObjects(eventCount, events, FALSE, (DWORD)(deadline - now)) != WAIT_OBJECT_0 ||
				WSAEnumNetworkEvents(sock, socketEvent, &socketEvents) == SOCKET_ERROR)
				break;
		}
	}

	// 입장 허가 패킷을 확인한 뒤 수신 스레드에서 쓰는 블로킹 모드로 복구
	u_long nonblocking = 0;
	const bool joined = received == packetSize && packet.header.type == SC_JOIN_OK &&
		packet.header.size == packetSize && packet.id < MAX_PLAYERS &&
		(cancelEvent == NULL || WaitForSingleObject(cancelEvent, 0) == WAIT_TIMEOUT) &&
		WSAEventSelect(sock, NULL, 0) == 0 &&
		ioctlsocket(sock, FIONBIO, &nonblocking) == 0;

	if (!joined)
		closesocket(sock);
	WSACloseEvent(socketEvent);
	if (!joined)
		return false;

	// 서버 입장 결과 보관
	pendingJoinSocket = sock;
	pendingJoinPlayerId = packet.id;
	if (cancelEvent == NULL && !FinishServerJoin(true))
		return false;

	outPlayerId = packet.id;
	outSeed = packet.seed;
	return true;
}

bool FinishServerJoin(bool acceptJoin)
{
	if (pendingJoinSocket == INVALID_SOCKET)
		return false;

	const SOCKET sock = pendingJoinSocket;
	pendingJoinSocket = INVALID_SOCKET;
	if (!acceptJoin) {
		closesocket(sock);
		return false;
	}

	// 임계 영역 초기화
	if (!InitializeCriticalSectionAndSpinCount(&net.queueCS, 0)) {
		closesocket(sock);
		return false;
	}
	net.queueCSInitialized = true;

	// 수신 스레드에서 사용할 소켓과 연결 상태 준비
	net.sock = sock;
	net.playerId = pendingJoinPlayerId;
	net.disconnected = false;

	// 서버의 후속 패킷을 받을 수신 스레드 생성
	net.recvThread = CreateThread(NULL, 0, RecvThread, &net, 0, NULL);
	if (net.recvThread == NULL) {
		// 스레드 생성 실패 시 이번 접속의 소켓과 임계 영역 정리
		closesocket(sock);
		net.sock = INVALID_SOCKET;
		net.playerId = MAX_PLAYERS;
		DeleteCriticalSection(&net.queueCS);
		net.queueCSInitialized = false;
		return false;
	}

	return true;
}

