//
// Created by Shi on 2017/10/19.
//

#ifndef AUDIOENGINE_AUDIOENGINE_PRIV_H
#define AUDIOENGINE_AUDIOENGINE_PRIV_H

#include <stdint.h>
#include "platform/android/jni/AudioFormat.h"
#include "platform/android/jni/AudioManager.h"
#include "AudioEngine/Engines/ActiveAE/ActiveAESink.h"
#include "AudioEngine/Utils/AEAudioFormat.h"
#include "platform/android/jni/Context.h"

typedef struct audio_engine_t_
{
    CJNIContext			*m_Context;					//context for system service
    CJNIAudioManager	*m_AudioManager;			//audio manager instance
    ActiveAE::CActiveAESink		*m_sink;					//audio sink
    AEAudioFormat 		Format;
    char				device[64];					//string of device name
    char 				deviceFriendlyName[64];

    //some flags
    int					passthrough;
    uint8_t       		m_packedBuffer[MAX_IEC61937_PACKET];

} audio_engine_t;

#endif //AUDIOENGINE_AUDIOENGINE_PRIV_H
