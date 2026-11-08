#include "net_client.h"

typedef struct NetClient
{
    SOCKET sock;
    HANDLE recvThread;

    // 나중에 패킷 큐, 큐 잠금, 끊김 표시, 내 id, 월드 시드 등 상태 추가
    
    // 내 캐릭터를 제외한 원격 플레이어 목록
    std::vector<Character*> remotes;

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