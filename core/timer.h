#ifndef __TIME_COUNTER_H__
#define __TIME_COUNTER_H__

#include <time.h>

class Timer
{
public:
    Timer();

    void start();
    struct timespec time();
    struct timespec stop();
    void reset();
    bool is_running();

private:
    struct timespec time_count;
    struct timespec time_start;
    struct timespec time_cur;
    bool stoped;

};

#endif /* __TIME_COUNTER_H__ */

