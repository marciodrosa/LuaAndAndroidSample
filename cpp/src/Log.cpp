#include "../include/Log.h"
#include "../include/Android.h"

using namespace std;
using namespace androidandlua;

void Log::Info(std::string message)
{
	((void)__android_log_print(ANDROID_LOG_INFO, "androidandlua", message.c_str()));
}

void Log::Error(std::string message)
{
	((void)__android_log_print(ANDROID_LOG_ERROR, "androidandlua", message.c_str()));
}
