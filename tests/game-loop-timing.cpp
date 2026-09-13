#include "utils/GameLoopTiming.h"

#include <cassert>
#include <iostream>

using namespace std::chrono_literals;
using TimePoint = GameTickSchedule::TimePoint;

static void steadyTicksDespiteWork()
{
    // Model work before and after heartbeat at the configured seven-tick rate.
    // Neither it nor small sleep overruns should accumulate into clock drift.
    constexpr auto interval = 142857us;
    GameTickSchedule schedule(TimePoint{}, interval, 14);
    auto now = TimePoint{};
    int ticks = 0;
    for (int i = 0; i < 700; ++i)
    {
        now = std::max(now, schedule.nextDeadline());
        now += (i % 3) * 1ms; // OS scheduling jitter
        now += 20ms;         // database/input/socket work
        auto due = schedule.consume(now);
        assert(due.count == 1 && due.dropped == 0);
        ticks += due.count;
        now += 30ms;         // heartbeat work
    }
    assert(ticks == 700);
    assert(now - TimePoint{} < 101s);
    assert(schedule.nextDeadline() == TimePoint{} + 701 * interval);
}

static void stallsAndFractionalTicks()
{
    GameTickSchedule schedule(TimePoint{}, 100ms, 14);
    assert(schedule.consume(TimePoint{} + 99ms).count == 0);
    auto due = schedule.consume(TimePoint{} + 350ms);
    assert(due.count == 3 && due.dropped == 0);
    assert(schedule.nextDeadline() == TimePoint{} + 400ms);
    assert(schedule.consume(TimePoint{} + 399ms).count == 0);
    assert(schedule.consume(TimePoint{} + 400ms).count == 1);

    // A ten-second stall gets bounded catch-up, then returns to the old phase.
    due = schedule.consume(TimePoint{} + 10450ms);
    assert(due.count == 14 && due.dropped == 86);
    assert(schedule.nextDeadline() == TimePoint{} + 10500ms);
    assert(schedule.consume(TimePoint{} + 10450ms).count == 0);
    assert(schedule.consume(TimePoint{} + 10500ms).count == 1);
}

static void databaseChecksAndRecovery()
{
    DatabaseHealthSchedule health(TimePoint{});
    bool connected = true;
    int checks = 0, reconnects = 0;
    auto check = [&] { ++checks; return connected; };
    auto reconnect = [&] { ++reconnects; }; // initially simulate failed reconnects

    for (int ms = 0; ms < 10000; ms += 100)
        health.poll(TimePoint{} + std::chrono::milliseconds(ms), check, reconnect);
    assert(checks == 10 && reconnects == 0);

    connected = false;
    health.poll(TimePoint{} + 10s, check, reconnect);
    assert(checks == 11 && reconnects == 1);
    for (int ms = 10001; ms < 25000; ms += 1)
        health.poll(TimePoint{} + std::chrono::milliseconds(ms), check, reconnect);
    assert(checks == 25 && reconnects == 1);
    health.poll(TimePoint{} + 25s, check, reconnect);
    assert(checks == 26 && reconnects == 2);

    connected = true;
    health.poll(TimePoint{} + 26s, check, reconnect);
    connected = false; // a separate outage should not inherit the old backoff
    health.poll(TimePoint{} + 27s, check, reconnect);
    assert(reconnects == 3);

    // Reconnection succeeds later; no extra reconnects once health is restored.
    health.poll(TimePoint{} + 42s, check, [&] { ++reconnects; connected = true; });
    health.poll(TimePoint{} + 43s, check, reconnect);
    assert(reconnects == 4);

    // A long game stall produces only one check, not a backlog of pings.
    const int before = checks;
    health.poll(TimePoint{} + 1h, check, reconnect);
    assert(checks == before + 1);
    health.poll(TimePoint{} + 1h, check, reconnect);
    assert(checks == before + 1);
}

int main()
{
    steadyTicksDespiteWork();
    stallsAndFractionalTicks();
    databaseChecksAndRecovery();
    std::cout << "PASS: tick cadence under load, fractional ticks, bounded catch-up, database check rate and reconnect recovery\n";
}
