
# Uncomment this if you're using STL in your project
# You can find more information here:
# https://developer.android.com/ndk/guides/cpp-support
APP_STL := c++_shared
APP_CFLAGS += -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB
APP_CPPFLAGS += -fexceptions -frtti

APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

# Min runtime API level
APP_PLATFORM=android-16
