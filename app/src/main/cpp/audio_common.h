//
// Created by Shi on 2017/10/19.
//

#ifndef AUDIOENGINE_AUDIO_COMMON_H
#define AUDIOENGINE_AUDIO_COMMON_H
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stddef.h>
#include "movieconfig.h"

/* Helper for GCC version checks */
#ifdef __GNUC__
# define CC_GCC_VERSION(maj,min) \
    ((__GNUC__ > (maj)) || (__GNUC__ == (maj) && __GNUC_MINOR__ >= (min)))
#else
# define VLC_GCC_VERSION(maj,min) (0)
#endif

#ifndef __cplusplus
# include <stdbool.h>
#endif



typedef uint32_t cc_fourcc_t;

#ifdef WORDS_BIGENDIAN
#   define CC_FOURCC( a, b, c, d ) \
        ( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
           | ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )
#   define CC_TWOCC( a, b ) \
        ( (uint16_t)(b) | ( (uint16_t)(a) << 8 ) )

#else
#   define CC_FOURCC( a, b, c, d ) \
        ( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
           | ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )
#   define CC_TWOCC( a, b ) \
        ( (uint16_t)(a) | ( (uint16_t)(b) << 8 ))
#endif


/* CEIL: division with round to nearest greater integer */
#define CEIL(n, d)  ( ((n) / (d)) + ( ((n) % (d)) ? 1 : 0) )

/* PAD: PAD(n, d) = CEIL(n ,d) * d */
#define PAD(n, d)   ( ((n) % (d)) ? ((((n) / (d)) + 1) * (d)) : (n) )

/* __MAX and __MIN: self explanatory */
#ifndef __MAX
#   define __MAX(a, b)   ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef __MIN
#   define __MIN(a, b)   ( ((a) < (b)) ? (a) : (b) )
#endif
/* clip v in [min, max] */
#define CC_CLIP(v, amin, amax)    __MIN(__MAX((v), (amin)), (amax))

/* Values available for audio channels */
#define AOUT_CHAN_CENTER            0x1
#define AOUT_CHAN_LEFT              0x2
#define AOUT_CHAN_RIGHT             0x4
#define AOUT_CHAN_REARCENTER        0x10
#define AOUT_CHAN_REARLEFT          0x20
#define AOUT_CHAN_REARRIGHT         0x40
#define AOUT_CHAN_MIDDLELEFT        0x100
#define AOUT_CHAN_MIDDLERIGHT       0x200
#define AOUT_CHAN_LFE               0x1000

#define AOUT_CHANS_FRONT  (AOUT_CHAN_LEFT       | AOUT_CHAN_RIGHT)
#define AOUT_CHANS_MIDDLE (AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT)
#define AOUT_CHANS_REAR   (AOUT_CHAN_REARLEFT   | AOUT_CHAN_REARRIGHT)
#define AOUT_CHANS_CENTER (AOUT_CHAN_CENTER     | AOUT_CHAN_REARCENTER)

#define AOUT_CHANS_STEREO AOUT_CHANS_2_0
#define AOUT_CHANS_2_0    (AOUT_CHANS_FRONT)
#define AOUT_CHANS_2_1    (AOUT_CHANS_FRONT | AOUT_CHAN_LFE)
#define AOUT_CHANS_3_0    (AOUT_CHANS_FRONT | AOUT_CHAN_CENTER)
#define AOUT_CHANS_3_1    (AOUT_CHANS_3_0   | AOUT_CHAN_LFE)
#define AOUT_CHANS_4_0    (AOUT_CHANS_FRONT | AOUT_CHANS_REAR)
#define AOUT_CHANS_4_1    (AOUT_CHANS_4_0   | AOUT_CHAN_LFE)
#define AOUT_CHANS_5_0    (AOUT_CHANS_4_0   | AOUT_CHAN_CENTER)
#define AOUT_CHANS_5_1    (AOUT_CHANS_5_0   | AOUT_CHAN_LFE)
#define AOUT_CHANS_6_0    (AOUT_CHANS_4_0   | AOUT_CHANS_MIDDLE)
#define AOUT_CHANS_7_0    (AOUT_CHANS_6_0   | AOUT_CHAN_CENTER)
#define AOUT_CHANS_7_1    (AOUT_CHANS_5_1   | AOUT_CHANS_MIDDLE)
#define AOUT_CHANS_8_1    (AOUT_CHANS_7_1   | AOUT_CHAN_REARCENTER)

#define AOUT_CHANS_4_0_MIDDLE (AOUT_CHANS_FRONT | AOUT_CHANS_MIDDLE)
#define AOUT_CHANS_4_CENTER_REAR (AOUT_CHANS_FRONT | AOUT_CHANS_CENTER)
#define AOUT_CHANS_5_0_MIDDLE (AOUT_CHANS_4_0_MIDDLE | AOUT_CHAN_CENTER)
#define AOUT_CHANS_6_1_MIDDLE (AOUT_CHANS_5_0_MIDDLE | AOUT_CHAN_REARCENTER | AOUT_CHAN_LFE)

/* Values available for original channels only */
#define AOUT_CHAN_DOLBYSTEREO       0x10000
#define AOUT_CHAN_DUALMONO          0x20000
#define AOUT_CHAN_REVERSESTEREO     0x40000

#define AOUT_CHAN_PHYSMASK          0xFFFF
#define AOUT_CHAN_MAX               9

/* Size of a frame for S/PDIF output. */
#define AOUT_SPDIF_SIZE 6144

/* Number of samples in an A/52 frame. */
#define A52_FRAME_NB 1536

/* Buffers which arrive in advance of more than AOUT_MAX_ADVANCE_TIME
 * will be considered as bogus and be trashed */
#define AOUT_MAX_ADVANCE_TIME           (AOUT_MAX_PREPARE_TIME + CLOCK_FREQ)

/* Buffers which arrive in advance of more than AOUT_MAX_PREPARE_TIME
 * will cause the calling thread to sleep */
#define AOUT_MAX_PREPARE_TIME           (2 * CLOCK_FREQ)

/* Buffers which arrive after pts - AOUT_MIN_PREPARE_TIME will be trashed
 * to avoid too heavy resampling */
#define AOUT_MIN_PREPARE_TIME           AOUT_MAX_PTS_ADVANCE

/* Tolerance values from EBU Recommendation 37 */
/** Maximum advance of actual audio playback time to coded PTS,
 * above which downsampling will be performed */
#define AOUT_MAX_PTS_ADVANCE            (CLOCK_FREQ / 25)

/** Maximum delay of actual audio playback time from coded PTS,
 * above which upsampling will be performed */
#define AOUT_MAX_PTS_DELAY              (3 * CLOCK_FREQ / 50)

/* Max acceptable resampling (in %) */
#define AOUT_MAX_RESAMPLING             10

/* Buffers which arrive in advance of more than AOUT_MAX_PREPARE_TIME
 * will cause the calling thread to sleep */
#define AOUT_MAX_PREPARE_TIME           (2 * CLOCK_FREQ)


#define CC_CODEC_S8                         CC_FOURCC('s','8',' ',' ')
#define CC_CODEC_U8                         CC_FOURCC('u','8',' ',' ')
#define CC_CODEC_S16L                       CC_FOURCC('s','1','6','l')
#define CC_CODEC_S16B                       CC_FOURCC('s','1','6','b')
#define CC_CODEC_U16L                       CC_FOURCC('u','1','6','l')
#define CC_CODEC_U16B                       CC_FOURCC('u','1','6','b')
#define CC_CODEC_S20B                       CC_FOURCC('s','2','0','b')
#define CC_CODEC_S24L                       CC_FOURCC('s','2','4','l')
#define CC_CODEC_S24B                       CC_FOURCC('s','2','4','b')
#define CC_CODEC_U24L                       CC_FOURCC('u','2','4','l')
#define CC_CODEC_U24B                       CC_FOURCC('u','2','4','b')
#define CC_CODEC_S24L32                     CC_FOURCC('s','2','4','4')
#define CC_CODEC_S24B32                     CC_FOURCC('S','2','4','4')
#define CC_CODEC_S32L                       CC_FOURCC('s','3','2','l')
#define CC_CODEC_S32B                       CC_FOURCC('s','3','2','b')
#define CC_CODEC_U32L                       CC_FOURCC('u','3','2','l')
#define CC_CODEC_U32B                       CC_FOURCC('u','3','2','b')
#define CC_CODEC_F32L                       CC_FOURCC('f','3','2','l')
#define CC_CODEC_F32B                       CC_FOURCC('f','3','2','b')
#define CC_CODEC_F64L                       CC_FOURCC('f','6','4','l')
#define CC_CODEC_F64B                       CC_FOURCC('f','6','4','b')

#define CC_CODEC_AC3                        CC_FOURCC('a','c','3',' ')
#define CC_CODEC_DTS                        CC_FOURCC('d','t','s',' ')
#define CC_CODEC_ALAW                       CC_FOURCC('a','l','a','w')
#define CC_CODEC_MULAW                      CC_FOURCC('m','l','a','w')
#define CC_CODEC_DAT12                      CC_FOURCC('L','P','1','2')
#define CC_CODEC_S24DAUD                    CC_FOURCC('d','a','u','d')
#define CC_CODEC_TWINVQ                     CC_FOURCC('T','W','I','N')
#define CC_CODEC_BMVAUDIO                   CC_FOURCC('B','M','V','A')
#define CC_CODEC_ULEAD_DV_AUDIO_NTSC        CC_FOURCC('m','s',0x02,0x15)
#define CC_CODEC_ULEAD_DV_AUDIO_PAL         CC_FOURCC('m','s',0x02,0x16)
#define CC_CODEC_INDEO_AUDIO                CC_FOURCC('m','s',0x04,0x02)
#define CC_CODEC_METASOUND                  CC_FOURCC('m','s',0x00,0x75)
#define CC_CODEC_ON2AVC                     CC_FOURCC('m','s',0x05,0x00)
#define CC_CODEC_TAK                        CC_FOURCC('t','a','k',' ')
#define CC_CODEC_SMACKAUDIO                 CC_FOURCC('S','M','K','A')

enum audio_channels {
    CHANNEL_OUT_FRONT_LEFT            = 0x4,
    CHANNEL_OUT_FRONT_RIGHT           = 0x8,
    CHANNEL_OUT_FRONT_CENTER          = 0x10,
    CHANNEL_OUT_LOW_FREQUENCY         = 0x20,
    CHANNEL_OUT_BACK_LEFT             = 0x40,
    CHANNEL_OUT_BACK_RIGHT            = 0x80,
    CHANNEL_OUT_FRONT_LEFT_OF_CENTER  = 0x100,
    CHANNEL_OUT_FRONT_RIGHT_OF_CENTER = 0x200,
    CHANNEL_OUT_BACK_CENTER           = 0x400,
    CHANNEL_OUT_MONO = CHANNEL_OUT_FRONT_LEFT,
    CHANNEL_OUT_STEREO = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT),
    CHANNEL_OUT_5POINT1 			  = 0x00fc,
    CHANNEL_OUT_7POINT1 			  = 0x03fc
};


/* Special endian dependant values
 * The suffic N means Native
 * The suffix I means Inverted (ie non native) */
#ifdef WORDS_BIGENDIAN
#   define CC_CODEC_S16N CC_CODEC_S16B
#   define CC_CODEC_U16N CC_CODEC_U16B
#   define CC_CODEC_S24N CC_CODEC_S24B
#   define CC_CODEC_U24N CC_CODEC_U24B
#   define CC_CODEC_S32N CC_CODEC_S32B
#   define CC_CODEC_U32N CC_CODEC_U32B
#   define CC_CODEC_FL32 CC_CODEC_F32B
#   define CC_CODEC_FL64 CC_CODEC_F64B

#   define CC_CODEC_S16I CC_CODEC_S16L
#   define CC_CODEC_U16I CC_CODEC_U16L
#   define CC_CODEC_S24I CC_CODEC_S24L
#   define CC_CODEC_U24I CC_CODEC_U24L
#   define CC_CODEC_S32I CC_CODEC_S32L
#   define CC_CODEC_U32I CC_CODEC_U32L

#else
#   define CC_CODEC_S16N CC_CODEC_S16L
#   define CC_CODEC_U16N CC_CODEC_U16L
#   define CC_CODEC_S24N CC_CODEC_S24L
#   define CC_CODEC_U24N CC_CODEC_U24L
#   define CC_CODEC_S32N CC_CODEC_S32L
#   define CC_CODEC_U32N CC_CODEC_U32L
#   define CC_CODEC_FL32 CC_CODEC_F32L
#   define CC_CODEC_FL64 CC_CODEC_F64L

#   define CC_CODEC_S16I CC_CODEC_S16B
#   define CC_CODEC_U16I CC_CODEC_U16B
#   define CC_CODEC_S24I CC_CODEC_S24B
#   define CC_CODEC_U24I CC_CODEC_U24B
#   define CC_CODEC_S32I CC_CODEC_S32B
#   define CC_CODEC_U32I CC_CODEC_U32B
#endif

#define CC_CODEC_A52   	CC_FOURCC('a','5','2',' ')
#define CC_CODEC_SPDIFL CC_FOURCC('s','p','d','i')
#define CC_CODEC_SPDIFB CC_FOURCC('s','p','d','b')


/* Branch prediction */
#ifdef __GNUC__
#   define likely(p)   __builtin_expect(!!(p), 1)
#   define unlikely(p) __builtin_expect(!!(p), 0)
#else
#   define likely(p)   (!!(p))
#   define unlikely(p) (!!(p))
#endif

/** Bit weight */
static inline unsigned popcount (unsigned x)
{
#if CC_GCC_VERSION(3,4)
    return __builtin_popcount (x);
#else
    unsigned count = 0;
    while (x)
    {
        count += x & 1;
        x = x >> 1;
    }
    return count;
#endif
}

#define AOUT_FMT_SPDIF( p_format ) \
    ( ((p_format)->i_format == CC_CODEC_SPDIFL)       \
       || ((p_format)->i_format == CC_CODEC_SPDIFB)   \
       || ((p_format)->i_format == CC_CODEC_A52)      \
       || ((p_format)->i_format == CC_CODEC_DTS) )

#define HAVE_FPU 		1

///**
// * audio format description
// */
//typedef struct audio_format_t_
//{
//    cc_fourcc_t i_format;                          /**< audio format fourcc */
//    unsigned int i_rate;                              /**< audio sample-rate */
//
//    /* Describes the channels configuration of the samples (ie. number of
//     * channels which are available in the buffer, and positions). */
//    uint16_t     i_physical_channels;
//
//    /* Describes from which original channels, before downmixing, the
//     * buffer is derived. */
//    uint32_t     i_original_channels;
//
//    /* Optional - for A/52, SPDIF and DTS types : */
//    /* Bytes used by one compressed frame, depends on bitrate. */
//    unsigned int i_bytes_per_frame;
//
//    /* Number of sampleframes contained in one compressed frame. */
//    unsigned int i_frame_length;
//    /* Please note that it may be completely arbitrary - buffers are not
//     * obliged to contain a integral number of so-called "frames". It's
//     * just here for the division :
//     * buffer_size = i_nb_samples * i_bytes_per_frame / i_frame_length
//     */
//
//    /* FIXME ? (used by the codecs) */
//    unsigned     i_bitspersample;
//    unsigned     i_blockalign;
//    uint8_t      i_channels; /* must be <=32 */
//} audio_format_t;

/**
 * The audio format structure that fully defines a stream's audio information
 */

typedef struct audio_format_t_
{
    int 		m_dataFormat; 		//The stream's data format (eg, AE_FMT_S16LE)
    int 		m_streamInfo;		//Stream info of raw passthrough
    int 		m_sampleRate;		//The stream's sample rate (eg, 48000)
    int 		m_nchanel; 			//number of channels
    uint32_t 	m_channelLayout; 	//The stream's channel layout
    uint32_t 	m_frames;			//he number of frames per period
    uint32_t 	m_frameSize;		//The size of one frame in bytes
    int			m_passthrough;

}audio_format_t;

typedef struct audio_sink_t_
{
    void *		sink;				//audio sink
    char		device[64];
    int			min_buf_size;		//minimum buffer size for audio track;
    int			buf_size;			//buffer size for audio track;
    int			samples_to_send_once;//
    bool		is_passthrough;
    bool 		IECPassthrough;
}audio_sink_t;

#endif //AUDIOENGINE_AUDIO_COMMON_H
