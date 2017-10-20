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
#include <memory>
#include <algorithm>
#include <string>
#include <iostream>
#include <cctype>			//header for ::toupper
#include "AESinkFactory.h"
#include "Interfaces/AESink.h"
#include "Sinks/AESinkAUDIOTRACK.h"
//#include "Sinks/AESinkNULL.h"
#include <android/log.h>

#define TAG    "AudioEngine" // 这个是自定义的LOG的标识
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__) // 定义LOGD类型


void CAESinkFactory::ParseDevice(std::string &device, std::string &driver)
{
  int pos = device.find_first_of(':');
  if (pos > 0) {
    driver = device.substr(0, pos);
    std::transform(driver.begin(), driver.end(), driver.begin(), ::toupper);

    // check that it is a valid driver name
    if (driver == "AUDIOTRACK")
      device = device.substr(pos + 1, device.length() - pos - 1);
    else
      driver.clear();
  } else
    driver.clear();
}

IAESink *CAESinkFactory::TrySink(std::string &driver, std::string &device, AEAudioFormat &format)
{
  IAESink *sink = NULL;

  if (driver == "NULL")
    return NULL;
  else
  {
    sink = new CAESinkAUDIOTRACK();
  }

  if (!sink)
    return NULL;

  if (sink->Initialize(format, device))
  {
    // do some sanity checks
    if (format.m_sampleRate == 0)
      LOGD("Sink %s:%s returned invalid sample rate", driver.c_str(), device.c_str());
    else if (format.m_channelLayout.Count() == 0)
      LOGD("Sink %s:%s returned invalid channel layout", driver.c_str(), device.c_str());
    else if (format.m_frames < 256)
      LOGD("Sink %s:%s returned invalid buffer size: %d", driver.c_str(), device.c_str(), format.m_frames);
    else
      return sink;
  }
  sink->Deinitialize();
  delete sink;
  return NULL;
}

IAESink *CAESinkFactory::Create(std::string &device, AEAudioFormat &desiredFormat, bool rawPassthrough)
{
  // extract the driver from the device string if it exists
  std::string driver;
  ParseDevice(device, driver);

  AEAudioFormat  tmpFormat = desiredFormat;
  IAESink       *sink;
  std::string    tmpDevice = device;

  sink = TrySink(driver, tmpDevice, tmpFormat);
  if (sink)
  {
    desiredFormat = tmpFormat;
    return sink;
  }

  return NULL;
}

void CAESinkFactory::EnumerateEx(AESinkInfoList &list, bool force)
{
  AESinkInfo info;
  info.m_deviceInfoList.clear();
  info.m_sinkName = "AUDIOTRACK";
  CAESinkAUDIOTRACK::EnumerateDevicesEx(info.m_deviceInfoList, force);
  if(!info.m_deviceInfoList.empty())
    list.push_back(info);

}
