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

