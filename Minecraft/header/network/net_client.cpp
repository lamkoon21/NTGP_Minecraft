#include "net_client.h"
#include "../protocol/protocol.h"

typedef struct NetClient
{
	SOCKET sock;
	HANDLE recvThread;
	std::vector<Character*> remotes;	// 내 캐릭터를 제외한 원격 플레이어 목록
	bool winsockStarted = false;		// Winsock 초기화 성공 여부 기록, 재접속 시 중복 초기화 방지

	// 나중에 패킷 큐, 큐 잠금, 끊김 표시, 내 id, 월드 시드 등 상태 추가

} NetClient;

// 서버 연결 소켓과 수신 스레드 핸들을 저장하는 전역 변수
static NetClient net = { INVALID_SOCKET, NULL, {} };

DWORD WINAPI RecvThread(LPVOID arg)
{
	NetClient* client = (NetClient*)arg;

	if (client == NULL || client->sock == INVALID_SOCKET)
		return 0;

	// client->sock으로 수신
	// 완성된 패킷을 큐에 저장

	return 0;
}

const std::vector<Character*>& Remotes()
{
	return net.remotes;
}

bool ConnectServer(const char* ipv4, uint8_t& outPlayerId, int& outSeed)
{
	// 서버 주소가 없거나 이미 연결된 경우 접속 요청 거절
	if (ipv4 == nullptr || net.sock != INVALID_SOCKET)
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

	// 서버에 TCP 연결 요청, 실패 시 소켓 닫기
	if (connect(sock, (const sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
		closesocket(sock);
		return false;
	}

	// 서버의 입장 응답을 기다리는 시간에 10초 제한 적용
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

	// 소켓 보관 및 서버가 보낸 플레이어 ID와 월드 시드 전달
	net.sock = sock;
	outPlayerId = packet.id;
	outSeed = packet.seed;

	return true;
}
