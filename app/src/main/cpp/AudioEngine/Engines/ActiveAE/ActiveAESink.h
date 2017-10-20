#pragma once
/*
 *      Copyright (C) 2010-2013 Team XBMC
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

//#include "threads/Event.h"
//#include "threads/Thread.h"
//#include "utils/ActorProtocol.h"
#include "../../Interfaces/AE.h"
#include "../../Interfaces/AESink.h"
#include "../../AESinkFactory.h"
#include "../../../threads/SystemClock.h"
//#include "cores/AudioEngine/Engines/ActiveAE/ActiveAEBuffer.h"

class CAEBitstreamPacker;

namespace ActiveAE
{
//using namespace Actor;

//class CEngineStats;

struct SinkConfig
{
  AEAudioFormat format;
  //CEngineStats *stats;
  const std::string *device;
};

struct SinkReply
{
  AEAudioFormat format;
  float cacheTotal;
  float latency;
  bool hasVolume;
};

/*class CSinkControlProtocol : public Protocol
{
public:
  CSinkControlProtocol(std::string name, CEvent* inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {};
  enum OutSignal
  {
    CONFIGURE,
    UNCONFIGURE,
    STREAMING,
    APPFOCUSED,
    VOLUME,
    FLUSH,
    TIMEOUT,
    SETSILENCETIMEOUT,
    SETNOISETYPE,
  };
  enum InSignal
  {
    ACC,
    ERR,
    STATS,
  };
};

class CSinkDataProtocol : public Protocol
{
public:
  CSinkDataProtocol(std::string name, CEvent* inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {};
  enum OutSignal
  {
    SAMPLE = 0,
    DRAIN,
  };
  enum InSignal
  {
    RETURNSAMPLE,
    ACC,
  };
};
*/
class CActiveAESink
{
public:
  //CActiveAESink(CEvent *inMsgEvent);
    CActiveAESink();
  void EnumerateSinkList(bool force);
  void EnumerateOutputDevices(AEDeviceList &devices, bool passthrough);
  std::string GetDefaultDevice(bool passthrough);
  void Start();
  void Dispose();
  AEDeviceType GetDeviceType(const std::string &device);
  bool HasPassthroughDevice();
  bool SupportsFormat(const std::string &device, AEAudioFormat &format);
    void OpenSink(AEAudioFormat &Format, std::string &outdevice);
    void Play();
    void Pause();
    void Stop();
    void Flush();
    int getPlayState();
    bool check_err() {return m_extError;}
    unsigned int OutputSamples( uint8_t **buffer, uint32_t nb_samples, int32_t pause_burst_ms, AEDelayStatus &outstatus,
                                int &framesOrPackets_need_to_remove);
  //CSinkControlProtocol m_controlPort;
  //CSinkDataProtocol m_dataPort;

protected:
  //void Process();
  //void StateMachine(int signal, Protocol *port, Message *msg);
  void PrintSinks();
  void GetDeviceFriendlyName(std::string &device);
  void SetSilenceTimer();
  bool NeedIECPacking();

  void SwapInit();

  void GenerateNoise();

  //CEvent m_outMsgEvent;
  //CEvent *m_inMsgEvent;
  int m_state;
  bool m_bStateMachineSelfTrigger;
  int m_extTimeout;
  int m_silenceTimeOut;
  bool m_extError;
  unsigned int m_extSilenceTimeout;
  bool m_extAppFocused;
  bool m_extStreaming;
  XbmcThreads::EndTime m_extSilenceTimer;

  //CSampleBuffer m_sampleOfSilence;
  enum
  {
    CHECK_SWAP,
    NEED_CONVERT,
    NEED_BYTESWAP,
    SKIP_SWAP,
  } m_swapState;

  std::string m_deviceFriendlyName;
  std::string m_device;
  AESinkInfoList m_sinkInfoList;
  IAESink *m_sink;
  AEAudioFormat m_sinkFormat, m_requestedFormat;
  //CEngineStats *m_stats;
  float m_volume;
  int m_sinkLatency;
  CAEBitstreamPacker *m_packer;
  bool m_needIecPack;
  bool m_streamNoise;
};

}
