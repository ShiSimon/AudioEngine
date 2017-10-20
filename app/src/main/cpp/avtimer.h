//
// Created by Shi on 2017/10/20.
//

#ifndef AUDIOENGINE_AVTIMER_H
#define AUDIOENGINE_AVTIMER_H

typedef int64_t mtime_t;

struct mtime_delay_status
{
    mtime_delay_status()
            :ddelay(0.0)
            ,dmaxcorrection(0.0)
            ,dtick(0)
    {}

    void   dSetDelay(double d);
    double dGetDelay();
    double ddelay;  // delay in sink currently
    double dmaxcorrection; // time correction must not be greater than sink delay
    int64_t dtick;  // timestamp when delay was calculated
};


struct timespec mtime_to_ts (mtime_t date);
mtime_t mdate (void);

#endif //AUDIOENGINE_AVTIMER_H
