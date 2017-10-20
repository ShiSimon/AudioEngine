//
// Created by Shi on 2017/10/19.
//
#include <jni.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include <string>
//#include "avtimer.h"
#include <android/log.h>
//#include "movieconfig.h"
#include "platform/android/jni/Context.h"
#include "platform/android/jni/AudioFormat.h"
#include "platform/android/jni/AudioTrack.h"
#include "AudioEngine/Utils/AEChannelData.h"
#include "AudioEngine/AESinkFactory.h"
#include "AudioEngine/Utils/AEDeviceInfo.h"
#include "AudioEngine/Utils/AEBitstreamPacker.h"
#include "audio_common.h"
#include "audioengine_priv.h"
#include "audioengine_utils.h"
#include "audioengine.h"
#include "AudioEngine/Engines/ActiveAE/ActiveAESink.h"
#include "AudioEngine/Interfaces/AE.h"
#include "AudioEngine/Utils/AEUtil.h"

using namespace std;
using namespace jni;
using namespace ActiveAE;

#define TAG    "AudioEngine" // 这个是自定义的LOG的标识
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__) // 定义LOGD类型

typedef struct audio_output_callback_t_
{
    //audio
    void *audiowrite;
    void *audiostop;
    void *audioflush;
    void *audiopause;
    void *audioopen;
    void *audioclose;
    void *audiostart;
    void *audiodelay;
    void *audiocrack;
} audio_output_callback_t;

static void audio_close(void *audio);

static AEChannel AUDIOTRACKChannelToAEChannel(int atChannel)
{
    AEChannel aeChannel;

    /* cannot use switch since CJNIAudioFormat is populated at runtime */

    if (atChannel == CJNIAudioFormat::CHANNEL_OUT_FRONT_LEFT)            aeChannel = AE_CH_FL;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_FRONT_RIGHT)           aeChannel = AE_CH_FR;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_FRONT_CENTER)          aeChannel = AE_CH_FC;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_LOW_FREQUENCY)         aeChannel = AE_CH_LFE;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_BACK_LEFT)             aeChannel = AE_CH_BL;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_BACK_RIGHT)            aeChannel = AE_CH_BR;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_SIDE_LEFT)             aeChannel = AE_CH_SL;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_SIDE_RIGHT)            aeChannel = AE_CH_SR;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_FRONT_LEFT_OF_CENTER)  aeChannel = AE_CH_FLOC;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_FRONT_RIGHT_OF_CENTER) aeChannel = AE_CH_FROC;
    else if (atChannel == CJNIAudioFormat::CHANNEL_OUT_BACK_CENTER)           aeChannel = AE_CH_BC;
    else                                                                      aeChannel = AE_CH_UNKNOWN1;

    return aeChannel;
}

/* audio_open opens audio decoder and configures it.
 * paras:
 * 	audio_format		inout, audio format.
 * 	audio_sink			in, use which sink to output audio.
 * return:
 * 	if successful, return pointer to decoder. otherwise, return 0.
*/
static void *audio_open(jobject context, audio_format_t *audio_format, audio_sink_t* audio_sink)
{
    if(context == 0 || audio_format == 0)
        return 0;

    jobject audioManager = 0;
    AEAudioFormat requestedFormat;
    AEAudioFormat format;
    CActiveAESink *m_sink = 0;
    AEDeviceList deviceslist;
    std::string outdevice = audio_sink->device;

    audio_engine_t *audio_engine = (audio_engine_t*)malloc(sizeof(audio_engine_t));
    if(audio_engine == 0)
        return 0;
    memset(audio_engine, 0, sizeof(audio_engine_t));

    /////////////////////////////////////////////
    //transform user's format to sink format
    format.m_dataFormat = (enum AEDataFormat)audio_format->m_dataFormat;
    format.m_sampleRate = audio_format->m_sampleRate;
    format.m_frameSize = audio_format->m_frameSize;
    format.m_frames = audio_format->m_frames;
    format.m_streamInfo.m_type = (enum CAEStreamInfo::DataType)audio_format->m_streamInfo;
    format.m_streamInfo.m_channels = audio_format->m_nchanel;
    format.m_streamInfo.m_sampleRate = audio_format->m_sampleRate;

    if(format.m_dataFormat == AE_FMT_RAW)
    {
        switch(format.m_streamInfo.m_type)
        {
            case CAEStreamInfo::STREAM_TYPE_AC3:
                format.m_streamInfo.m_ac3FrameSize = audio_format->m_frames; //number of frames each quration
                break;
        }
    }
    switch(audio_format->m_channelLayout)
    {
        case 2:
            format.m_channelLayout = AE_CH_LAYOUT_2_0;
            break;
        case 6:
            format.m_channelLayout = AE_CH_LAYOUT_5_1;
            break;
        case 8:
            format.m_channelLayout = AE_CH_LAYOUT_7_1;
            break;
        default:
            format.m_channelLayout = AE_CH_LAYOUT_2_0;
            break;
    }

    ///////////////////////////////////////////////////////
    //create active sink
    //EnumerateSinkList
    m_sink = new CActiveAESink();
    if(m_sink == 0)
    {
        LOGD("#audio_open: create sink. Failed!!! \n");
        goto err_context;
    }

    audio_engine->m_sink = m_sink;
    m_sink->EnumerateSinkList(true);
    m_sink->EnumerateOutputDevices(deviceslist,true);
    //open sink
    m_sink->OpenSink(format,outdevice);
    if(m_sink->check_err())
    {
        LOGD("#audio_open: open sink. Failed!!! \n");
        goto err_context;
    }

    //print output capabilities
    LOGD("#audio_open: AudioFormat \n");
    LOGD("#CJNIAudioFormat::ENCODING_AC3=%d \n", CJNIAudioFormat::ENCODING_AC3);
    LOGD("#CJNIAudioFormat::ENCODING_E_AC3=%d \n", CJNIAudioFormat::ENCODING_E_AC3);
    LOGD("#CJNIAudioFormat::ENCODING_DTS=%d \n", CJNIAudioFormat::ENCODING_DTS);
    LOGD("#CJNIAudioFormat::ENCODING_DTS_HD=%d \n", CJNIAudioFormat::ENCODING_DTS_HD);
    LOGD("#CJNIAudioFormat::ENCODING_DOLBY_TRUEHD=%d \n", CJNIAudioFormat::ENCODING_DOLBY_TRUEHD);

    LOGD("#audio_open: open sink. OK!!! \n");

    //set volume
    //create context handle
    audio_engine->passthrough = audio_format->m_passthrough;
    if(audio_format->m_passthrough)
    {
        CJNIAudioFormat JNIAudioFormat;
        audio_engine->m_Context = new CJNIContext(context);
        if(audio_engine->m_Context == 0)
            goto err_context;

        //create audio manager handle
        audioManager = audio_engine->m_Context->getSystemService("audio");
        audio_engine->m_AudioManager = new CJNIAudioManager(audioManager);
        if(audio_engine->m_AudioManager == 0)
            goto err_context;

        //get focus of audio
        if(audio_engine->m_AudioManager->requestAudioFocus(CJNIAudioManager::STREAM_MUSIC, CJNIAudioManager::AUDIOFOCUS_GAIN) == 0)
        {
            LOGD("#requestAudioFocus: Failed\n");
            goto err_context;
        }

        //set route
        //LOGD("##########mode = %d\n", audio_engine->m_AudioManager->getMode());
        //	audio_engine->m_AudioManager->setBluetoothScoOn(false);
        //	audio_engine->m_AudioManager->setSpeakerphoneOn(false);
        //	audio_engine->m_AudioManager->unloadSoundEffects();
        //	audio_engine->m_AudioManager->getFrameNum();
        //	audio_engine->m_AudioManager->getSamplerate();
        audio_engine->m_AudioManager->setStreamVolume(audio_engine->m_AudioManager->getStreamMaxVolume());

        if(audio_engine->m_AudioManager)
        {
            audio_engine->m_AudioManager->abandonAudioFocus();
            delete audio_engine->m_AudioManager;
            audio_engine->m_AudioManager = 0;
        }

        if(audio_engine->m_Context)
        {
            delete audio_engine->m_Context;
            audio_engine->m_Context = 0;
        }


    }

    return audio_engine;

    err_context:
    audio_close(audio_engine);
    return 0;
}

static void audio_close(void *audio)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0)
        return;

    //release audio manager
    if(audio_engine->m_AudioManager)
    {
        audio_engine->m_AudioManager->abandonAudioFocus();
        delete audio_engine->m_AudioManager;
        audio_engine->m_AudioManager = 0;
    }

    //release audio context
    if(audio_engine->m_Context)
    {
        delete audio_engine->m_Context;
        audio_engine->m_Context = 0;
    }

    //close audio output device.
    if(audio_engine->m_sink)
    {
        audio_engine->m_sink->Dispose();
        delete audio_engine->m_sink;
    }

    free(audio_engine);
}

static int audio_write(void *audio, uint8_t **data, uint32_t data_len_in_byte, uint32_t SamplePakcetCount, uint32_t pause_burst_ms,
                       mtime_delay_status &mtime_delay)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0 || data == 0 || data[0] == 0)
        return -1;

    int framesOrPackets_need_to_remove = 0;
    AEDelayStatus outstatus;

    //LOGD("##audio_write-0: %d,%d\n", SamplePakcetCount, pause_burst_ms,data[0]);
    int delayms = audio_engine->m_sink->OutputSamples(data, SamplePakcetCount, pause_burst_ms, outstatus, framesOrPackets_need_to_remove);
    //LOGD("##audio_write-1:");

    mtime_delay.ddelay = outstatus.delay;
    mtime_delay.dtick = outstatus.tick;
    mtime_delay.dmaxcorrection = outstatus.maxcorrection;

    //check if error occurs
    if(audio_engine->m_sink->check_err() || delayms == 0)
    {
        LOGD("##audio_write-ERR:");
        return -2;
    }

    return delayms;
}

/* audio_flush clears audio data in audio buffer.
 * paras:
 * 	audio				in, pointer to audio decoder.
 * return:
 * 	return no value.
*/
static void audio_flush(void *audio)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0)
        return;

    if(audio_engine->m_sink)
        audio_engine->m_sink->Flush();

}


/* audio_stop stops audio device.
 * paras:
 * 	audio				in, pointer to audio decoder.
 * return:
 * 	return no value.
*/
static void audio_stop(void *audio)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0)
        return;

    if(audio_engine->m_sink)
        audio_engine->m_sink->Stop();
}

/* audio_play begins to play audio device.
 * paras:
 * 	audio				in, pointer to audio decoder.
 * return:
 * 	return no value.
*/
static void audio_play(void *audio)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0)
        return;

    if(audio_engine->m_sink)
    {
        //check which state audio device is in. If not playing state, set it.
        if(audio_engine->m_sink->getPlayState() != CJNIAudioTrack::PLAYSTATE_PLAYING)
            audio_engine->m_sink->Play();
    }
}


/* audio_pause pauses audio device.
 * paras:
 * 	audio				in, pointer to audio decoder.
 * return:
 * 	return no value.
*/
static void audio_pause(void *audio)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0)
        return;

    if(audio_engine->m_sink)
        audio_engine->m_sink->Pause();
}

/* audio_pack crack audio data.
 * paras:
 * 	audio				in, pointer to audio decoder.
 * 	data				in, pointer to audio data
 * 	size				in, audio data length
 * 	dest				out, cracked audio data
 * return:
 * 	return length of cracked audio data.
*/
static int audio_crack(void *audio, uint8_t *data, unsigned int size, uint8_t *dest)
{
    audio_engine_t *audio_engine = (audio_engine_t*)audio;
    if(audio == 0)
        return -1;

    return CrackAC3(data, size, dest);
}


int audio_decoder_config(void *audio_decoder_callback)
{
    audio_output_callback_t *audio_decoder_callbackS = (audio_output_callback_t *)audio_decoder_callback;
    if(audio_decoder_callback == 0)
        return -1;

    audio_decoder_callbackS->audioopen = (void*)audio_open;
    audio_decoder_callbackS->audioclose = (void*)audio_close;
    audio_decoder_callbackS->audioflush = (void*)audio_flush;
    audio_decoder_callbackS->audiopause = (void*)audio_pause;
    audio_decoder_callbackS->audiostop = (void*)audio_stop;
    audio_decoder_callbackS->audiowrite = (void*)audio_write;
    audio_decoder_callbackS->audiostart = (void*)audio_play;
    audio_decoder_callbackS->audiocrack = (void *)audio_crack;
    return 0;
}

