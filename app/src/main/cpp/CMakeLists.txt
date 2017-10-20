cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -UNDEBUG")

add_library(audioengine-lib SHARED
            native-lib.cpp
            audioengine.cpp
            avtimer.cpp
            audioengine_utils.cpp
            platform/android/jni/AudioFormat.cpp
            platform/android/jni/AudioManager.cpp
            platform/android/jni/AudioTrack.cpp
            platform/android/jni/Context.cpp
            utils/AMLUtils.cpp
            utils/SysfsUtils.cpp
            utils/TimeUtils.cpp
            threads/SystemClock.cpp
            AudioEngine/Utils/AEStreamInfo.cpp
            AudioEngine/Utils/AEChannelInfo.cpp
            AudioEngine/Utils/AEDeviceInfo.cpp
            AudioEngine/AESinkFactory.cpp
            AudioEngine/Utils/AEUtil.cpp
            AudioEngine/Sinks/AESinkAUDIOTRACK.cpp
            AudioEngine/Engines/ActiveAE/ActiveAESink.cpp
            )

# Include libraries needed for native-codec-jni lib
target_link_libraries(audioengine-lib
                      android
                      log
                      )