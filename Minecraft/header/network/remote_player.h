#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>

#include "../protocol/protocol.h"

class RemotePlayer
{
public:
    void Push(uint32_t tick, const PlayerState& state);

    // 저장된 상태가 없으면 false.
    // 상태를 구했으면 outState에 넣고 true.
    bool Sample(double renderTick, PlayerState& outState) const;

private:
    struct Snapshot
    {
        uint32_t tick;
        PlayerState state;
    };

    static constexpr std::size_t MAX_SNAPSHOTS = 10;

    std::deque<Snapshot> snapshots;
};