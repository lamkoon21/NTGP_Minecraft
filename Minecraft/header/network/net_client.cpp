#include "net_client.h"
#include "../protocol/protocol.h"
#include <atomic>
#include <queue>

// 수신 큐에 복사해서 보관할 패킷 데이터
typedef struct ReceivedPacket
{
	char data[MAX_PACKET_SIZE];
} ReceivedPacket;

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

} NetClient;

// 서버 연결 소켓과 수신 스레드 핸들을 저장하는 전역 변수
static NetClient net = { INVALID_SOCKET, NULL, {} };

DWORD WINAPI RecvThread(LPVOID arg)
{
	NetClient* client = (NetClient*)arg;

	if (client == NULL || client->sock == INVALID_SOCKET)
		return 0;

	const SOCKET sock = client->sock;
	const int BUF_SIZE = 4096;
	char buf[BUF_SIZE];
	int len = 0;

	while (true)
	{
		// 버퍼의 남은 공간에 서버 데이터 수신
		const int received = recv(sock, buf + len, BUF_SIZE - len, 0);
		if (received <= 0)
			break;
		len += received;

		while (len >= (int)sizeof(PacketHeader))
		{
			// 잘못된 패킷 크기 수신 시 끊김 표시 후 수신 종료
			PacketHeader header;
			memcpy(&header, buf, sizeof(header));
			if (header.size < sizeof(PacketHeader) || header.size > MAX_PACKET_SIZE)
			{
				client->disconnected = true;
				return 0;
			}

			// 완성된 패킷 추출, 아직 덜 받은 데이터는 다음 수신까지 보관
			ReceivedPacket packet{};
			if (!ExtractPacket(buf, len, packet.data))
				break;

			// 임계 영역 안에서 수신 패킷을 큐에 복사
			EnterCriticalSection(&client->queueCS);
			client->packets.push(packet);
			LeaveCriticalSection(&client->queueCS);
		}
	}

	// 서버 종료 또는 수신 오류를 게임 갱신에 전달
	client->disconnected = true;

	return 0;
}

const std::vector<Character*>& Remotes()
{
	return net.remotes;
}

bool ConnectServer(const char* ipv4, uint8_t& outPlayerId, int& outSeed)
{
	// 서버 주소가 없거나 이미 연결된 경우 접속 요청 거절
	if (ipv4 == nullptr || net.sock != INVALID_SOCKET || net.recvThread != NULL)
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

	// 서버에 TCP 연결 요청
	if (connect(sock, (const sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
		closesocket(sock);
		return false;
	}

	// 작은 패킷의 전송 지연을 줄이기 위한 TCP_NODELAY 설정
	BOOL nodelay = TRUE;
	if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay)) == SOCKET_ERROR) {
		closesocket(sock);
		return false;
	}

	// 서버 입장 제한 시간 10초 적용
	DWORD time_limit = 10000;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time_limit, sizeof(time_limit)) == SOCKET_ERROR) {
		closesocket(sock);
		return false;
	}

	// 입장 허가 패킷(SC_JOIN_OK) 수신 및 종류, 크기와 플레이어 ID 확인
	SC_JOIN_OK_PACKET packet{};
	const int packetSize = sizeof(packet);
	const int received = recv(sock, (char*)&packet, packetSize, MSG_WAITALL);
	if (received != packetSize || packet.header.type != SC_JOIN_OK || 
		packet.header.size != packetSize || packet.id >= MAX_PLAYERS) {
		closesocket(sock);
		return false;
	}

	// 입장 시 10초 제한 해제
	time_limit = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time_limit, sizeof(time_limit)) == SOCKET_ERROR) {
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
	net.playerId = packet.id;
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

	// 게임 시작에 사용할 플레이어 ID와 월드 시드 전달
	outPlayerId = packet.id;
	outSeed = packet.seed;

	return true;
}
