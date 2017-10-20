//
// Created by Shi on 2017/10/20.
//

#include "audioengine_utils.h"
#include <jni.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <android/log.h>
#include "movieconfig.h"
#include "platform/android/jni/Context.h"
#include "AudioEngine/Utils/AEChannelData.h"
#include "AudioEngine/Utils/AEDeviceInfo.h"
#include "AudioEngine/AESinkFactory.h"
#include "audioengine_priv.h"

bool NeedIECPacking(std::string &dev, std::string &dri, AESinkInfoList &m_sinkInfoList)
{
//	std::string dev = m_device;
//	std::string dri;

    CAESinkFactory::ParseDevice(dev, dri);
    for (AESinkInfoList::iterator itt = m_sinkInfoList.begin(); itt != m_sinkInfoList.end(); ++itt)
    {
        if (dri == itt->m_sinkName)
        {
            for (AEDeviceInfoList::iterator itt2 = itt->m_deviceInfoList.begin(); itt2 != itt->m_deviceInfoList.end(); ++itt2)
            {
                CAEDeviceInfo& info = *itt2;
                if (info.m_deviceName == dev)
                {
                    return info.m_wantsIECPassthrough;
                }
            }
        }
    }
    return true;
}


void GetDeviceFriendlyName(std::string &device, std::string &deviceFriendlyName, AESinkInfoList &m_sinkInfoList)
{
    deviceFriendlyName = "Device not found";
    /* Match the device and find its friendly name */
    for (AESinkInfoList::iterator itt = m_sinkInfoList.begin(); itt != m_sinkInfoList.end(); ++itt)
    {
        AESinkInfo sinkInfo = *itt;
        for (AEDeviceInfoList::iterator itt2 = sinkInfo.m_deviceInfoList.begin(); itt2 != sinkInfo.m_deviceInfoList.end(); ++itt2)
        {
            CAEDeviceInfo& devInfo = *itt2;
            if (devInfo.m_deviceName == device)
            {
                deviceFriendlyName = devInfo.m_displayName;
                break;
            }
        }
    }
    return;
}
