/*
 *      Copyright (C) 2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#include <algorithm>
#include "AudioManager.h"
#include <android/log.h>

#define TAG    "AudioEngine" // 这个是自定义的LOG的标识
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__) // 定义LOGD类型

//using namespace jni;

JNIEnv *movie_jnienv();
int	movie_GetSDKVersion();

int CJNIAudioManager::STREAM_MUSIC(3);

int CJNIAudioManager::AUDIOFOCUS_GAIN(0x00000001);
int CJNIAudioManager::AUDIOFOCUS_LOSS(0xffffffff);
int CJNIAudioManager::AUDIOFOCUS_GAIN_TRANSIENT(0x00000002);
int CJNIAudioManager::AUDIOFOCUS_LOSS_TRANSIENT(0xfffffffe);
int CJNIAudioManager::AUDIOFOCUS_REQUEST_GRANTED(0x00000001);
int CJNIAudioManager::AUDIOFOCUS_REQUEST_FAILED(0x00000000);

int CJNIAudioManager::GET_DEVICES_ALL(0x00000003);
int CJNIAudioManager::GET_DEVICES_INPUTS(0x00000001);
int CJNIAudioManager::GET_DEVICES_OUTPUTS(0x00000002);

#define GET_CLASS(clazzname)							env->FindClass(clazzname)
#define GET_STATIC_FIELD_ID(clazz, name, signature) 	env->GetStaticFieldID(clazz, name, signature)
#define GET_STATIC_FIELD_INT(clazz,fieldID)  			env->GetStaticIntField(clazz, fieldID)
#define GET_STATIC_INT(clazz,name)  					GET_STATIC_FIELD_INT(clazz, (GET_STATIC_FIELD_ID(clazz,name,"I")))
#define EXCEPTIONCLEAR()								env->ExceptionClear()
#define EXCEPTIONCHECK() 								env->ExceptionOccurred()
#define GET_METHOD_ID(get, clazz, str, args) 			env->get(clazz, (str), (args))

#define JNI_CALL( what, obj, method, ... ) env->what(  obj, method, ##__VA_ARGS__ )
#define JNI_CALL_INT( obj, method, ... ) JNI_CALL( CallIntMethod, obj, method, ##__VA_ARGS__ )
#define JNI_CALL_BOOL( obj, method, ... ) JNI_CALL( CallBooleanMethod, obj, method, ##__VA_ARGS__ )
#define JNI_CALL_VOID( obj, method, ... ) JNI_CALL( CallVoidMethod, obj, method, ##__VA_ARGS__ )
#define JNI_CALL_STATIC_INT( clazz, method, ... ) JNI_CALL( CallStaticIntMethod, clazz, method, ##__VA_ARGS__ )

CJNIAudioManager::CJNIAudioManager(const jobject &object)
{
  JNIEnv *env = movie_jnienv();
  m_object = (jobject) env->NewGlobalRef( object );

  jclass clazz = GET_CLASS("com/example/demoplayer2/CCOnAudioFocusChangeListener");

  if(clazz == 0)
    return;
  jmethodID listener_ctor = GET_METHOD_ID(GetMethodID, clazz, "<init>", "()V");

  EXCEPTIONCLEAR();
  m_AudioFocusChangeListener = env->NewObject(clazz,listener_ctor);
  if (EXCEPTIONCHECK()) {
    //LOGD("##Unable to create m_AudioFocusChangeListener");
    EXCEPTIONCLEAR();
    env->DeleteLocalRef(clazz);
    return;
  }
  m_AudioFocusChangeListener = (jobject) env->NewGlobalRef( m_AudioFocusChangeListener );
  env->DeleteLocalRef(clazz);
}

CJNIAudioManager::~CJNIAudioManager()
{
  JNIEnv *env = movie_jnienv();
  if(m_object)
    env->DeleteGlobalRef( m_object);
  if(m_AudioFocusChangeListener)
    env->DeleteGlobalRef( m_AudioFocusChangeListener);
}

void CJNIAudioManager::PopulateStaticFields()
{
  /*jclass clazz = find_class("android/media/AudioManager");
  STREAM_MUSIC = (get_static_field<int>(clazz, "STREAM_MUSIC"));
  AUDIOFOCUS_GAIN = (get_static_field<int>(clazz, "AUDIOFOCUS_GAIN"));
  AUDIOFOCUS_LOSS = (get_static_field<int>(clazz, "AUDIOFOCUS_LOSS"));
  AUDIOFOCUS_REQUEST_GRANTED = (get_static_field<int>(clazz, "AUDIOFOCUS_REQUEST_GRANTED"));
  AUDIOFOCUS_REQUEST_FAILED = (get_static_field<int>(clazz, "AUDIOFOCUS_REQUEST_FAILED"));

  int sdk = CJNIBase::GetSDKVersion();
  if (sdk >= 23)
  {
    GET_DEVICES_ALL = (get_static_field<int>(clazz, "GET_DEVICES_ALL"));
    GET_DEVICES_INPUTS = (get_static_field<int>(clazz, "GET_DEVICES_INPUTS"));
    GET_DEVICES_OUTPUTS = (get_static_field<int>(clazz, "GET_DEVICES_OUTPUTS"));
  }*/
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  STREAM_MUSIC  = (GET_STATIC_INT(clazz, "STREAM_MUSIC"));
  AUDIOFOCUS_GAIN  = (GET_STATIC_INT(clazz, "AUDIOFOCUS_GAIN"));
  AUDIOFOCUS_LOSS  = (GET_STATIC_INT(clazz, "AUDIOFOCUS_LOSS"));
  AUDIOFOCUS_REQUEST_GRANTED  = (GET_STATIC_INT(clazz, "AUDIOFOCUS_REQUEST_GRANTED"));
  AUDIOFOCUS_REQUEST_FAILED  = (GET_STATIC_INT(clazz, "AUDIOFOCUS_REQUEST_FAILED"));
  env->DeleteLocalRef(clazz);
}

int CJNIAudioManager::getStreamMaxVolume()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID getStreamMaxVolumeID = GET_METHOD_ID(GetMethodID,clazz,"getStreamMaxVolume","(I)I");
  env->DeleteLocalRef(clazz);
  return JNI_CALL_INT(m_object,getStreamMaxVolumeID,STREAM_MUSIC);
}

int CJNIAudioManager::getMode()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID getModeID = GET_METHOD_ID(GetMethodID,clazz,"getMode","()I");
  env->DeleteLocalRef(clazz);
  return JNI_CALL_INT(m_object,getModeID);
}

void CJNIAudioManager::setSpeakerphoneOn(bool onoff)
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID setSpeakerphoneOnID = GET_METHOD_ID(GetMethodID,clazz,"setSpeakerphoneOn","(Z)V");
  env->DeleteLocalRef(clazz);
  JNI_CALL_VOID(m_object,setSpeakerphoneOnID,onoff);
}

void CJNIAudioManager::unloadSoundEffects()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID unloadSoundEffectsID = GET_METHOD_ID(GetMethodID,clazz,"unloadSoundEffects","()V");
  env->DeleteLocalRef(clazz);
  JNI_CALL_VOID(m_object,unloadSoundEffectsID);
}

void CJNIAudioManager::setBluetoothScoOn(bool onoff)
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID setBluetoothScoOnID = GET_METHOD_ID(GetMethodID,clazz,"setBluetoothScoOn","(Z)V");
  env->DeleteLocalRef(clazz);
  JNI_CALL_VOID(m_object,setBluetoothScoOnID,onoff);
}

int CJNIAudioManager::getStreamVolume()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID getStreamVolumeID = GET_METHOD_ID(GetMethodID,clazz,"getStreamVolume","(I)I");
  env->DeleteLocalRef(clazz);
  return JNI_CALL_INT(m_object,getStreamVolumeID,STREAM_MUSIC);
}

void CJNIAudioManager::setStreamVolume(int index /* 0 */, int flags /* NONE */)
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID setStreamVolumeID = GET_METHOD_ID(GetMethodID,clazz,"setStreamVolume","(III)V");
  env->DeleteLocalRef(clazz);
  JNI_CALL_VOID(m_object,setStreamVolumeID,STREAM_MUSIC,index,flags);
}

void CJNIAudioManager::getSamplerate()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID getPropertyID = GET_METHOD_ID(GetMethodID,clazz,"getProperty","(Ljava/lang/String;)Ljava/lang/String;");
  env->DeleteLocalRef(clazz);
  jstring jservicestr= (jstring)env->NewStringUTF("android.media.property.OUTPUT_SAMPLE_RATE");
  jstring jrate = (jstring)env->CallObjectMethod(m_object,getPropertyID,jservicestr);
  env->DeleteLocalRef(jservicestr);

  const char * pathConst = env->GetStringUTFChars(jrate, JNI_FALSE);
  LOGD("################rate = %s\n", pathConst);
  env->ReleaseStringUTFChars(jrate, pathConst);
  env->DeleteLocalRef(jrate);
}


void CJNIAudioManager::getFrameNum()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID getPropertyID = GET_METHOD_ID(GetMethodID,clazz,"getProperty","(Ljava/lang/String;)Ljava/lang/String;");
  env->DeleteLocalRef(clazz);
  jstring jservicestr=env->NewStringUTF("android.media.property.OUTPUT_FRAMES_PER_BUFFER");
  jstring jrate = (jstring)env->CallObjectMethod(m_object,getPropertyID,jservicestr);
  env->DeleteLocalRef(jservicestr);

  const char * pathConst = env->GetStringUTFChars(jrate, JNI_FALSE);
  LOGD("################OUTPUT_FRAMES = %s\n", pathConst);
  env->ReleaseStringUTFChars(jrate, pathConst);
  env->DeleteLocalRef(jrate);
}

int CJNIAudioManager::requestAudioFocus(int streamType, int durationHint)
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID requestAudioFocusID = GET_METHOD_ID(GetMethodID,clazz,"requestAudioFocus","(Landroid/media/AudioManager$OnAudioFocusChangeListener;II)I");
  env->DeleteLocalRef(clazz);

  return JNI_CALL_INT(m_object,requestAudioFocusID,m_AudioFocusChangeListener,streamType,durationHint) == AUDIOFOCUS_REQUEST_GRANTED;
}
////
int CJNIAudioManager::abandonAudioFocus()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID abandonAudioFocusID = GET_METHOD_ID(GetMethodID,clazz,"abandonAudioFocus","(Landroid/media/AudioManager$OnAudioFocusChangeListener;)I");
  env->DeleteLocalRef(clazz);

  return JNI_CALL_INT(m_object,abandonAudioFocusID,m_AudioFocusChangeListener);
}

bool CJNIAudioManager::isBluetoothA2dpOn()
{
  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID isBluetoothA2dpOnID = GET_METHOD_ID(GetMethodID,clazz,"isBluetoothA2dpOn","()Z");
  env->DeleteLocalRef(clazz);
  return JNI_CALL_BOOL(m_object,isBluetoothA2dpOnID);
}

bool CJNIAudioManager::isWiredHeadsetOn()
{

  JNIEnv *env = movie_jnienv();
  jclass clazz = GET_CLASS("android/media/AudioManager");
  jmethodID isWiredHeadsetOnID = GET_METHOD_ID(GetMethodID,clazz,"isWiredHeadsetOn","()Z");
  env->DeleteLocalRef(clazz);
  return JNI_CALL_BOOL(m_object,isWiredHeadsetOnID);
}


