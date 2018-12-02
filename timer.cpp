#include <iostream>
#include <ctime>

class Timer
{
public:
    Timer() { clock_gettime(CLOCK_MONOTONIC_RAW, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_MONOTONIC_RAW, &end_);
        return ((end_.tv_sec - beg_.tv_sec) * 1000) +
            ((end_.tv_nsec - beg_.tv_nsec) / 1e+6);
    }

    void reset() { clock_gettime(CLOCK_MONOTONIC_RAW, &beg_); }

private:
    timespec beg_, end_;
};
