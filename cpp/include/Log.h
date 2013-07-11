#ifndef ANDROIDANDLUA_LOG_H
#define ANDROIDANDLUA_LOG_H

#include <string>

namespace androidandlua
{
	class Log
	{
	public:
		static void Info(std::string);
		static void Error(std::string);
	};
}

#endif
