// ============================================================================
// protocol.h : 서버와 클라이언트가 함께 쓰는 패킷 약속
// ============================================================================
// 서버(server.cpp)와 클라이언트(net_client.cpp)가 똑같이 include한다.
// 양쪽이 같은 파일을 보므로 "몇 번째 바이트에 무엇이 들어 있는지"가 항상 일치한다.
//
// 모든 패킷은 [헤더 3바이트][내용] 모양이다. 헤더에는 패킷 전체 크기와 종류가 들어 있다.
// TCP는 보낸 바이트를 순서대로 이어서 전달할 뿐, 패킷이 어디서 끝나는지는 알려 주지 않는다.
// 두 패킷이 recv() 한 번에 붙어서 오거나, 한 패킷이 두 번에 나뉘어 올 수 있다.
// 그래서 받는 쪽은 헤더의 size를 보고 패킷 하나만큼 잘라 낸다 (ExtractPacket).
//
// 패킷 7가지 (접두사가 방향)
//   CS_ : 클라이언트 -> 서버
//     CS_MOVE           매 틱 내 위치, 시선, 든 블록
//     CS_BLOCK_CHANGE   블록을 부수거나 놓았다
//   SC_ : 서버 -> 클라이언트
//     SC_JOIN_OK        입장 허락. 내 id와 월드 시드
//     SC_ADD_PLAYER     다른 플레이어가 있다 / 들어왔다
//     SC_REMOVE_PLAYER  다른 플레이어가 나갔다
//     SC_MOVE_PLAYER    다른 플레이어가 움직였다 (CS_MOVE 중계)
//     SC_BLOCK_CHANGE   블록 한 칸이 바뀌었다 (CS_BLOCK_CHANGE 중계)
//
// 한 판의 흐름
//   입장 : 클라 connect() -> 서버 accept()
//          서버 -> 새 클라   SC_JOIN_OK 1개, SC_BLOCK_CHANGE x 바뀐 칸 수, SC_ADD_PLAYER x 기존 인원
//          서버 -> 다른 클라 SC_ADD_PLAYER (새 플레이어)
//   게임 : 클라 CS_MOVE         -> 서버 -> SC_MOVE_PLAYER  -> 보낸 사람을 뺀 클라 (매 틱)
//          클라 CS_BLOCK_CHANGE -> 서버 -> SC_BLOCK_CHANGE -> 보낸 사람을 포함한 전원
//   퇴장 : 서버 -> 남은 클라 SC_REMOVE_PLAYER
//
// 바이트 순서 : 서버와 클라이언트가 모두 Windows x86/x64(리틀 엔디언)라서 숫자를 변환 없이 그대로 보낸다.
//               sockaddr_in에 넣는 포트 번호만 htons()로 바꾼다.
// ============================================================================

// include guard
#ifndef PROTOCOL_H // PROTOCOL_H가 정의되어 있지 않을 때만
#define PROTOCOL_H // PROTOCOL_H를 정의


#include <winsock2.h>  
#include <cstdint> 
#include <cstring>

#pragma comment(lib, "ws2_32.lib")  
// 윈속 라이브러리를 자동 링크


// ---------------------------------------------------------------------------
// 공통 상수
// ---------------------------------------------------------------------------
constexpr uint16_t SERVER_PORT = 9000;  // 서버가 listen하는 포트
constexpr int MAX_PLAYERS = 3;          // 정원. 서버의 g_players[MAX_PLAYERS], 플레이어 id는 0 ~ 2
constexpr int MAX_PACKET_SIZE = 64;     // 패킷 하나의 최대 바이트 수. 가장 큰 SC_ADD_PLAYER(26바이트)보다 넉넉하게 잡았다.
                                        // ExtractPacket이 꺼낸 패킷을 담는 배열의 크기이고,
                                        // 헤더의 size가 이보다 크면 깨진 데이터로 본다.


// ---------------------------------------------------------------------------
// 패킷 종류 번호 : 헤더의 type에 들어간다
// ---------------------------------------------------------------------------
// ": uint8_t"는 이 enum을 1바이트로 만든다는 뜻이다.
// 번호는 서버와 클라이언트의 약속이라 한 번 정하면 바꾸지 않는다. 줄 순서를 바꿔도 번호가 그대로이도록 직접 적었다.
// 0은 비워 두었다. 0으로 채워진 메모리를 패킷으로 착각하지 않게 하기 위해서다.
// Windows 헤더의 매크로와 이름이 겹치면 안 된다. 그래서 SC_MOVE가 아니라 SC_MOVE_PLAYER다.
// (<winuser.h>에 "#define SC_MOVE 0xF010"이 있어서, SC_MOVE라고 쓰면 0xF010으로 바뀌어 컴파일 오류가 난다)
enum PacketType : uint8_t {
    CS_MOVE          = 1,
    CS_BLOCK_CHANGE  = 2,

    SC_JOIN_OK       = 3,
    SC_ADD_PLAYER    = 4,
    SC_REMOVE_PLAYER = 5,
    SC_MOVE_PLAYER   = 6,
    SC_BLOCK_CHANGE  = 7,
};


// ---------------------------------------------------------------------------
// 패킷 구조체
// ---------------------------------------------------------------------------
// #pragma pack(push, 1) : 이 구간의 구조체에는 멤버 사이에 빈 바이트(패딩)를 넣지 말라는 뜻이다.
//   컴파일러는 보통 float를 4의 배수 위치에 두려고 빈 바이트를 끼워 넣는다.
//   1로 묶으면 sizeof가 멤버 크기의 합과 같아지고, 메모리 모양이 곧 네트워크로 나가는 바이트 모양이 된다.
// #pragma pack(pop) (아래) : 원래 설정으로 되돌린다.
//   되돌리지 않으면 이 파일 뒤에 include되는 다른 헤더의 구조체까지 빈칸 없이 묶여서 엉뚱하게 동작한다.
//
// 각 패킷의 header는 선언만 해도 { 그 구조체의 크기, 그 패킷 번호 }로 채워진다.
// 그래서 보내는 쪽은 내용만 채워서 SendPacket()에 넘기면 된다.
#pragma pack(push, 1)

// 모든 패킷의 맨 앞 3바이트
struct PacketHeader {
    uint16_t size;  // 헤더를 포함한 패킷 전체 바이트 수 (= sizeof(그 패킷 구조체))
    uint8_t  type;  // PacketType 값
};

// 플레이어 한 명의 움직임 상태 (21바이트)
// CS_MOVE, SC_MOVE_PLAYER, SC_ADD_PLAYER에 똑같이 들어가므로, 서버는 받은 state를 통째로 저장하고 그대로 중계하면 된다.
struct PlayerState {
    float x = 0.0f, y = 0.0f, z = 0.0f;  // Character::Position (발 위치, 월드 좌표)
    float yaw = 0.0f;                    // camera.Yaw   (도 단위)
    float pitch = 0.0f;                  // camera.Pitch (도 단위, -89 ~ 89)
    uint8_t block_kind = 0;              // Character::block_kind : 손에 든 블록의 칸 번호 0 ~ 8 (설계서의 block_num)
                                         // BlockType 값이 아니라 Character::blocks[] 배열의 인덱스다.
};


// ----- 클라이언트 -> 서버 -----

// CS_MOVE (24바이트) : 매 틱 SendMove()가 보낸다.
// 보낸 사람 id는 넣지 않는다. 서버는 어느 소켓(ClientThread)으로 왔는지 보고 이미 안다.
// 서버 : g_players[id]에 저장하고, 보낸 사람을 뺀 접속자에게 SC_MOVE_PLAYER로 중계한다.
struct CS_MOVE_PACKET {
    PacketHeader header = { sizeof(CS_MOVE_PACKET), CS_MOVE };
    PlayerState state;
};

// CS_BLOCK_CHANGE (16바이트) : 블록을 부수거나 놓은 직후 SendBlockChange()가 보낸다.
// 서버 : 좌표와 종류를 검사하고 g_worldDiff에 기록한 뒤, 보낸 사람을 포함한 전원에게 SC_BLOCK_CHANGE를 보낸다.
struct CS_BLOCK_CHANGE_PACKET {
    PacketHeader header = { sizeof(CS_BLOCK_CHANGE_PACKET), CS_BLOCK_CHANGE };
    int32_t x = 0;                // 블록 한 칸의 월드 좌표 = 청크 번호 * 16 + 청크 안 칸 번호 (z도 같다)
    int32_t y = 0;                // 청크 -19 ~ 19이므로 x, z는 -304 ~ 319, 높이 y는 0 ~ 127
    int32_t z = 0;
    uint8_t block_type = 0;       // 바뀐 뒤의 BlockType 값 (blocktype.h). 부순 것은 AIR(0)
};


// ----- 서버 -> 클라이언트 -----

// SC_JOIN_OK (8바이트) : 접속하자마자 서버가 맨 먼저 보낸다 (SendJoinData의 첫 패킷).
// 클라이언트 ConnectServer()는 이 크기만큼만 MSG_WAITALL로 받고(최대 10초) header의 type과 size를 확인한다.
// 뒤따라오는 SC_BLOCK_CHANGE, SC_ADD_PLAYER는 소켓에 남아 있다가 RecvThread가 받는다.
struct SC_JOIN_OK_PACKET {
    PacketHeader header = { sizeof(SC_JOIN_OK_PACKET), SC_JOIN_OK };
    uint8_t id = 0;               // 서버가 정해 준 내 id (0 ~ MAX_PLAYERS - 1)
    int32_t seed = 0;             // 월드 시드. CreateWorld(seed) -> new World(seed)
};

// SC_ADD_PLAYER (26바이트) : 원격 플레이어를 만들라는 패킷. 두 경우에 보낸다.
//   1) 새 접속자에게 : 기존 플레이어마다 1개 (SendJoinData). 서버가 마지막으로 받은 상태를 담는다.
//   2) 기존 접속자에게 : 방금 들어온 플레이어 1개 (main). 아직 움직인 적이 없으므로 has_moved = 0
// 클라이언트 : 원격 Character를 만들고, has_moved가 1이면 ApplyRemoteState()로 state를 적용한다.
//              0이면 생성자가 정한 시작 지점에 그대로 둔다.
struct SC_ADD_PLAYER_PACKET {
    PacketHeader header = { sizeof(SC_ADD_PLAYER_PACKET), SC_ADD_PLAYER };
    uint8_t id = 0;
    uint8_t has_moved = 0;        // 1 : CS_MOVE를 한 번이라도 보냈다 -> state가 유효하다
                                  // 0 : 아직 보낸 적이 없다 -> state를 무시한다
    PlayerState state;
};

// SC_REMOVE_PLAYER (4바이트) : 누가 나갔다. 남은 접속자에게 보낸다 (ClientThread가 끝날 때).
// 클라이언트 : 그 id의 원격 Character를 지운다.
struct SC_REMOVE_PLAYER_PACKET {
    PacketHeader header = { sizeof(SC_REMOVE_PLAYER_PACKET), SC_REMOVE_PLAYER };
    uint8_t id = 0;
};

// SC_MOVE_PLAYER (25바이트) : 서버가 받은 CS_MOVE를 보낸 사람 id만 붙여서 다른 접속자에게 그대로 중계한다.
// 클라이언트 : 그 id의 원격 Character에 ApplyRemoteState()로 state를 적용한다.
struct SC_MOVE_PLAYER_PACKET {
    PacketHeader header = { sizeof(SC_MOVE_PLAYER_PACKET), SC_MOVE_PLAYER };
    uint8_t id = 0;               // 움직인 플레이어
    PlayerState state;
};

// SC_BLOCK_CHANGE (16바이트) : 블록 한 칸이 바뀌었다. 두 경우에 보낸다.
//   1) CS_BLOCK_CHANGE 중계 : 보낸 사람까지 전원에게. 그래서 모두가 서버가 받은 순서대로 맞춰진다.
//   2) 새 접속자에게 : g_worldDiff의 바뀐 칸마다 1개 (SendJoinData)
// 클라이언트 : world->SetBlock(x, y, z, block_type)
struct SC_BLOCK_CHANGE_PACKET {
    PacketHeader header = { sizeof(SC_BLOCK_CHANGE_PACKET), SC_BLOCK_CHANGE };
    int32_t x = 0;                // 뜻은 CS_BLOCK_CHANGE와 같다
    int32_t y = 0;
    int32_t z = 0;
    uint8_t block_type = 0;
};

#pragma pack(pop)


// 패킷 크기를 못 박아 둔다. 누가 필드를 바꾸거나 pack을 지우면 여기서 컴파일 오류가 나서 바로 알 수 있다.
// 새 패킷을 추가하면 여기에도 한 줄 추가한다.
static_assert(sizeof(PacketHeader) == 3, "PacketHeader must be 3 bytes");
static_assert(sizeof(PlayerState) == 21, "PlayerState must be 21 bytes");
static_assert(sizeof(CS_MOVE_PACKET) == 24, "CS_MOVE_PACKET must be 24 bytes");
static_assert(sizeof(CS_BLOCK_CHANGE_PACKET) == 16, "CS_BLOCK_CHANGE_PACKET must be 16 bytes");
static_assert(sizeof(SC_JOIN_OK_PACKET) == 8, "SC_JOIN_OK_PACKET must be 8 bytes");
static_assert(sizeof(SC_ADD_PLAYER_PACKET) == 26, "SC_ADD_PLAYER_PACKET must be 26 bytes");
static_assert(sizeof(SC_REMOVE_PLAYER_PACKET) == 4, "SC_REMOVE_PLAYER_PACKET must be 4 bytes");
static_assert(sizeof(SC_MOVE_PLAYER_PACKET) == 25, "SC_MOVE_PLAYER_PACKET must be 25 bytes");
static_assert(sizeof(SC_BLOCK_CHANGE_PACKET) == 16, "SC_BLOCK_CHANGE_PACKET must be 16 bytes");
static_assert(sizeof(SC_ADD_PLAYER_PACKET) <= MAX_PACKET_SIZE, "every packet must fit in MAX_PACKET_SIZE");


// ---------------------------------------------------------------------------
// ExtractPacket : recv()로 쌓아 둔 바이트에서 완성된 패킷 하나를 꺼낸다
// ---------------------------------------------------------------------------
// buf : recv()로 받은 바이트를 이어 붙여 둔 버퍼. 앞에서부터 len바이트가 들어 있다.
// len : buf에 들어 있는 바이트 수. 패킷을 꺼내면 그만큼 줄어든다 (그래서 참조로 받는다).
// out : 꺼낸 패킷을 복사해 줄 곳. MAX_PACKET_SIZE바이트 이상이어야 한다.
// 반환 : true  = 맨 앞 패킷이 다 모여서 out에 꺼냈다. buf의 나머지는 앞으로 당겨졌다.
//        false = 아직 덜 모였다. recv()를 더 해야 한다. buf와 len은 그대로다.
//
// 사용 예 (서버 ClientThread, 클라이언트 RecvThread)
//     const int BUF_SIZE = 4096;                       // MAX_PACKET_SIZE보다 커야 한다
//     char buf[BUF_SIZE];
//     int len = 0;
//     while (true) {
//         int received = recv(sock, buf + len, BUF_SIZE - len, 0);
//         if (received <= 0) break;                    // 0 이하 = 연결이 끊겼다
//         len += received;
//
//         char packet[MAX_PACKET_SIZE];
//         while (ExtractPacket(buf, len, packet)) {    // 한 번에 여러 개가 왔을 수 있으니 while
//             ProcessPacket(id, packet);               // packet은 헤더부터 시작하는 패킷 하나
//         }
//     }
//
// 헤더의 size가 말이 안 되면(헤더보다 작거나 MAX_PACKET_SIZE보다 크면) 데이터가 깨진 것이다.
// TCP에는 경계 표시가 없어서 한 번 어긋나면 다시 맞출 방법이 없으므로, 쌓인 바이트를 모두 버리고 false를 돌려준다.
// size가 0이면 같은 자리를 끝없이 꺼내고, 너무 크면 out 배열 밖을 덮어쓰게 되는데 그것을 막는다.
inline bool ExtractPacket(char* buf, int& len, char* out)
{
    // 1) 헤더 3바이트도 아직 안 왔으면 패킷 크기를 알 수 없다
    if (len < static_cast<int>(sizeof(PacketHeader)))
        return false;

    // 2) 헤더에서 이 패킷의 전체 크기를 읽는다
    //    char 배열을 구조체 포인터로 바꿔 읽는 대신 지역 변수로 복사해서 읽는다 (C++ 규칙상 가장 안전한 방법).
    PacketHeader header;
    memcpy(&header, buf, sizeof(header));
    const int size = header.size;

    // 3) 말이 안 되는 크기 = 깨진 데이터. 모두 버린다
    if (size < static_cast<int>(sizeof(PacketHeader)) || size > MAX_PACKET_SIZE) {
        len = 0;
        return false;
    }

    // 4) 패킷 끝까지 아직 안 왔으면 더 받아야 한다
    if (len < size)
        return false;

    // 5) 완성된 패킷을 out에 복사하고, 뒤에 남은 바이트를 buf 맨 앞으로 당긴다.
    //    당길 때는 원본과 복사할 곳이 겹치므로 memcpy가 아니라 memmove를 쓴다.
    memcpy(out, buf, size);
    len -= size;
    memmove(buf, buf + size, len);
    return true;
}


// ---------------------------------------------------------------------------
// SendPacket : 패킷 하나를 통째로 보낸다
// ---------------------------------------------------------------------------
// sock   : 보낼 소켓
// packet : 보낼 패킷 구조체의 주소 (&p). 맨 앞 헤더의 size만큼 보낸다.
// 반환   : true = 다 보냈다, false = 실패했다 (연결이 끊겼거나 헤더가 잘못됐다)
//
// 사용 예 (클라이언트 SendMove)
//     CS_MOVE_PACKET p;                         // header는 이미 { 24, CS_MOVE }로 채워져 있다
//     p.state.x = player->Position.x;
//     ...
//     SendPacket(sock, &p);
//
// send()는 요청한 길이보다 적게 보내고 돌아올 수도 있어서, 다 보낼 때까지 반복한다.
// 같은 소켓에 두 스레드가 동시에 보내면 바이트가 섞일 수 있다. 그래서 서버는 g_cs 안에서만 부른다.
inline bool SendPacket(SOCKET sock, const void* packet)
{
    PacketHeader header;
    memcpy(&header, packet, sizeof(header));
    const int size = header.size;

    // 헤더가 잘못된 패킷을 보내면 받는 쪽의 데이터가 깨지므로 보내지 않는다
    if (size < static_cast<int>(sizeof(PacketHeader)) || size > MAX_PACKET_SIZE)
        return false;

    const char* data = static_cast<const char*>(packet);
    int sent = 0;
    while (sent < size) {
        const int result = send(sock, data + sent, size - sent, 0);
        if (result <= 0)              // SOCKET_ERROR(-1) : 연결이 끊겼거나 오류
            return false;
        sent += result;
    }
    return true;
}


#endif
