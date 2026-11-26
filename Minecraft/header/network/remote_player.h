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

inline void RemotePlayer::Push(
    uint32_t tick, const PlayerState& state)
{
    if (!snapshots.empty())
    {
        // 같은 틱이면 중복 저장하지 않고 상태만 교체한다.
        if (tick == snapshots.back().tick)
        {
            snapshots.back().state = state;
            return;
        }

        // 마지막으로 저장한 틱보다 오래된 상태는 무시한다.
        if (tick < snapshots.back().tick)
            return;
    }

    snapshots.push_back(Snapshot{ tick, state });

    // 최근 10개만 보관한다.
    while (snapshots.size() > MAX_SNAPSHOTS)
    {
        snapshots.pop_front();
    }
}