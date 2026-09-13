#ifndef GAME_LOOP_TIMING_H
#define GAME_LOOP_TIMING_H

#include <algorithm>
#include <cassert>
#include <chrono>

// Absolute deadlines retain partial ticks and include time spent in all loop
// work. A long stall drops excess debt rather than replaying an unbounded backlog.
class GameTickSchedule
{
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    struct DueTicks
    {
        int count;
        long long dropped;
    };

    GameTickSchedule(TimePoint start, std::chrono::microseconds interval, int maxCatchUp)
        : nextTick(start + interval), interval(interval), maxCatchUp(maxCatchUp)
    {
        assert(interval.count() > 0 && maxCatchUp > 0);
    }

    TimePoint nextDeadline() const { return nextTick; }

    DueTicks consume(TimePoint now)
    {
        if (now < nextTick)
            return {0, 0};

        const auto due = 1 + (now - nextTick) / interval;
        nextTick += interval * due;
        const int count = static_cast<int>(std::min<long long>(due, maxCatchUp));
        return {count, due - count};
    }

private:
    TimePoint nextTick;
    std::chrono::microseconds interval;
    int maxCatchUp;
};

// Rate-limit the ping itself, not just the reconnect log. Check even when no
// queries run, and retry failed reconnects without doing so on every game tick.
class DatabaseHealthSchedule
{
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    explicit DatabaseHealthSchedule(TimePoint start)
        : nextCheck(start), nextReconnect(start) {}

    template<typename Check, typename Reconnect>
    void poll(TimePoint now, Check check, Reconnect reconnect)
    {
        if (now < nextCheck)
            return;
        nextCheck = now + std::chrono::seconds(1);

        if (check())
        {
            // A new outage after recovery should get an immediate first retry.
            nextReconnect = now;
        }
        else if (now >= nextReconnect)
        {
            nextReconnect = now + std::chrono::seconds(15);
            reconnect();
        }
    }

private:
    TimePoint nextCheck;
    TimePoint nextReconnect;
};

#endif
