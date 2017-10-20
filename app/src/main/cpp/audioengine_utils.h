//
// Created by Shi on 2017/10/20.
//

#ifndef AUDIOENGINE_AUDIOENGINE_UTILS_H
#define AUDIOENGINE_AUDIOENGINE_UTILS_H

bool NeedIECPacking(std::string &dev, std::string &dri, AESinkInfoList &m_sinkInfoList);
void GetDeviceFriendlyName(std::string &device, std::string &deviceFriendlyName, AESinkInfoList &m_sinkInfoList);


#endif //AUDIOENGINE_AUDIOENGINE_UTILS_H
