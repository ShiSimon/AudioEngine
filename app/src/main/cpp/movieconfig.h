//
// Created by Shi on 2017/10/19.
//

#ifndef AUDIOENGINE_MOVIECONFIG_H
#define AUDIOENGINE_MOVIECONFIG_H


#include <stdint.h>

/* All timestamp below or equal to this define are invalid/unset
 * XXX the numerical value is 0 because of historical reason and will change.*/
#define VLC_TS_INVALID INT64_C(0)
#define VLC_TS_0 INT64_C(1)

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#define CLOCK_FREQ INT64_C(1000000)

/*****************************************************************************
 * Interface configuration
 *****************************************************************************/

/* Base delay in micro second for interface sleeps */
#define INTF_IDLE_SLEEP                 (CLOCK_FREQ/20)

/*****************************************************************************
 * Input thread configuration
 *****************************************************************************/

/* Used in ErrorThread */
#define INPUT_IDLE_SLEEP                (CLOCK_FREQ/10)

/*
 * General limitations
 */

/* Duration between the time we receive the data packet, and the time we will
 * mark it to be presented */
#define DEFAULT_PTS_DELAY               (3*CLOCK_FREQ/10)

/*****************************************************************************
 * SPU configuration
 *****************************************************************************/

/* Buffer must avoid arriving more than SPU_MAX_PREPARE_TIME in advanced to
 * the SPU */
#define SPU_MAX_PREPARE_TIME            (CLOCK_FREQ/2)

/*****************************************************************************
 * Video configuration
 *****************************************************************************/

/*
 * Default settings for video output threads
 */

/* Multiplier value for aspect ratio calculation (2^7 * 3^3 * 5^3) */
#define VOUT_ASPECT_FACTOR              432000

/* Maximum width of a scaled source picture - this should be relatively high,
 * since higher stream values will result in no display at all. */
#define VOUT_MAX_WIDTH                  4096

/* Number of planes in a picture */
#define VOUT_MAX_PLANES                 5

/*
 * Time settings
 */

/* Time to sleep when waiting for a buffer (from vout or the video fifo).
 * It should be approximately the time needed to perform a complete picture
 * loop. Since it only happens when the video heap is full, it does not need
 * to be too low, even if it blocks the decoder. */
#define VOUT_OUTMEM_SLEEP               (CLOCK_FREQ/50)

/* The default video output window title */
#define VOUT_TITLE                      "MOVIE"

/*****************************************************************************
 * Messages and console interfaces configuration
 *****************************************************************************/

/* Maximal depth of the object tree output by vlc_dumpstructure */
#define MAX_DUMPSTRUCTURE_DEPTH         100


#define __MIN(x,y)						(x > y) ? y:x


#define AUDIOTRACK_HW_LATENCY
#define AUDIOTRACK_USE_FLOAT
#define AUDIOTRACK_NATIVE_SAMPLERATE

#define HAS_LIBAMCODEC
#define SAFE_DELETE(p)       do { delete (p);     (p)=NULL; } while (0)
#define SAFE_DELETE_ARRAY(p) do { delete[] (p);   (p)=NULL; } while (0)
#define SAFE_RELEASE(p)      do { if(p) { (p)->Release(); (p)=NULL; } } while (0)


#endif //AUDIOENGINE_MOVIECONFIG_H
