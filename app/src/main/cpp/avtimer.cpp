//
// Created by Shi on 2017/10/20.
//

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include "movieconfig.h"
#include "avtimer.h"


struct timespec mtime_to_ts (mtime_t date)
{
    lldiv_t d = lldiv (date, CLOCK_FREQ);
    struct timespec ts = { d.quot, d.rem * (1000000000 / CLOCK_FREQ) };

    return ts;
}

/* time */
mtime_t mdate (void)
{
    struct timespec ts;

    if(clock_gettime (CLOCK_MONOTONIC, &ts) != 0)
        abort ();

    return (INT64_C(1000000) * ts.tv_sec) + (ts.tv_nsec / 1000);
}


int64_t MyCurrentHostCounter(void) //return ns
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return( ((int64_t)now.tv_sec * 1000000000L) + now.tv_nsec );
}

int64_t MyCurrentHostFrequency(void)
{
    return( (int64_t)1000000000L );
}

void mtime_delay_status::dSetDelay(double d)
{
    ddelay = d;
    dmaxcorrection = d;
    dtick = MyCurrentHostCounter();
}

double mtime_delay_status::dGetDelay() //unit = second
{
    double d = 0;
    if (dtick)
        d = (double)(MyCurrentHostCounter() - dtick) / MyCurrentHostFrequency();
    if (d > dmaxcorrection)
        d = dmaxcorrection;

    return ddelay - d;
}