#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <cmath>

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

inline bool RemotePlayer::Sample(
    double renderTick, PlayerState& outState) const
{
    if (snapshots.empty() || !std::isfinite(renderTick))
        return false;

    // 가장 오래된 상태보다 앞이면 첫 상태를 유지한다.
    if (renderTick <= static_cast<double>(snapshots.front().tick))
    {
        outState = snapshots.front().state;
        return true;
    }

    // 최신 상태보다 뒤이면 마지막 상태를 유지한다.
    // 다음 위치를 추측해서 이동시키지는 않는다.
    if (renderTick >= static_cast<double>(snapshots.back().tick))
    {
        outState = snapshots.back().state;
    }

    for (std::size_t i = 1; i < snapshots.size(); ++i)
    {
        const Snapshot& previous = snapshots[i - 1];
        const Snapshot& next = snapshots[i];

        if (renderTick > static_cast<double>(next.tick))
            continue;

        // 정확히 해당 틱이면 슬롯과 이동 모드까지 그대로 적용한다.
        if (renderTick == static_cast<double>(next.tick))
        {
            outState = next.state;
            return true;
        }

        const double interval =
            static_cast<double>(next.tick) -
            static_cast<double>(previous.tick);

        // Push()에서 틱을 오름차순으로 보관하므로 interval은 양수다.
        const float alpha = static_cast<float>(
            (renderTick - static_cast<double>(previous.tick))
            / interval
            );

        const PlayerState& a = previous.state;
        const PlayerState& b = next.state;

        // 슬롯과 이동 모드는 다음 틱에 도달하기 전까지 이전 값 유지.
        outState = a;

        outState.x = a.x + (b.x - a.x) * alpha;
        outState.y = a.y + (b.y - a.y) * alpha;
        outState.z = a.z + (b.z - a.z) * alpha;
        outState.pitch = a.pitch + (b.pitch - a.pitch) * alpha;

        // yaw는 가장 짧은 회전 방향으로 보간한다.
        // 예: 359도 → 1도는 -358도가 아니라 +2도 회전.
        const float yawDelta =
            std::remainder(b.yaw - a.yaw, 360.0f);

        outState.yaw = a.yaw + yawDelta * alpha;

        return true;
    }

    return false;
}