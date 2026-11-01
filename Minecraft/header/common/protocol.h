#pragma once

#include <winsock2.h>
#include <cstdint>
#include <type_traits>

// 패킷 종류
enum class PacketType : uint8_t
{
    CS_MOVE = 1,
    CS_BLOCK_CHANGE,

    SC_JOIN_OK,
    SC_ADD_PLAYER,
    SC_REMOVE_PLAYER,
    SC_MOVED,
    SC_BLOCK_CHANGE
};

// 불필요한 패딩 제거
#pragma pack(push, 1)

// 모든 패킷의 공통 헤더
struct PacketHeader
{
    uint16_t size;  // 패킷 전체 크기
    PacketType type; // 패킷 종류
};

// 이전 방식 복원
#pragma pack(pop)

// 패킷 전송 함수
template <typename T>
inline bool SendPacket(SOCKET sock, const T& packet)
{
    // 패킷 구조 검사 (컴파일 시점)
    static_assert(std::is_trivially_copyable<PacketT>::value,
        "패킷 구조 검사");

    // 패킷 크기 검사 (컴파일 시점)
    static_assert(sizeof(PacketT) <= UINT16_MAX, 
        "패킷 크기 검사");


}