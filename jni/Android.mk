LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := lua-prebuilt
LOCAL_SRC_FILES := liblua.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := bridge
LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti
LOCAL_LDLIBS += -llog -landroid -ljnigraphics -lEGL -lGLESv1_CM
LOCAL_SRC_FILES := /../cpp/src/Bridge.cpp /../cpp/src/Log.cpp
LOCAL_SHARED_LIBRARIES := lua-prebuilt
include $(BUILD_SHARED_LIBRARY)
