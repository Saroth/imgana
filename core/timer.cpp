
#include "timer.h"

Timer::Timer()
{
    reset();
}

void Timer::start()
{
    if (stoped) {
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        stoped = false;
    }
}

struct timespec Timer::time()
{
    if (!stoped) {
        clock_gettime(CLOCK_MONOTONIC, &time_cur);
    }

    struct timespec tim = {
        time_count.tv_sec  + time_cur.tv_sec  - time_start.tv_sec,
        time_count.tv_nsec + time_cur.tv_nsec - time_start.tv_nsec,
    };
    return tim;
}

struct timespec Timer::stop()
{
    time();
    stoped = true;
    time_count.tv_sec += time_cur.tv_sec - time_start.tv_sec;
    time_count.tv_nsec += time_cur.tv_nsec - time_start.tv_nsec;
    time_start = { 0, 0, };
    time_cur = { 0, 0, };

    return time_count;
}

void Timer::reset()
{
    time_count = { 0, 0, };
    time_start = { 0, 0, };
    time_cur = { 0, 0, };
    stoped = true;
}

bool Timer::is_running()
{
    return !stoped;
}

