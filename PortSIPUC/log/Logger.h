#ifndef LOGGER_H_
#define LOGGER_H_
#include"stdafx.h"
#include <tchar.h>
#include <log4cplus/logger.h>
#include <string>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;

class LoggerLocal  {
public:
    static bool init(std::string despath, const wchar_t* format);
    static log4cplus::Logger logger_obj;
	static void setloglevel(std::string strloglevel);
private:
	LoggerLocal() {}
};

#define LOG4_TRACE(logEvent) LOG4CPLUS_TRACE(LoggerLocal::logger_obj, logEvent)
#define LOG4_DEBUG(logEvent) LOG4CPLUS_DEBUG(LoggerLocal::logger_obj, logEvent)
#define LOG4_INFO(logEvent)  LOG4CPLUS_INFO(LoggerLocal::logger_obj, logEvent)
#define LOG4_ERROR(logEvent) LOG4CPLUS_ERROR(LoggerLocal::logger_obj, logEvent)
#define LOG4_FATAL(logEvent) LOG4CPLUS_FATAL(LoggerLocal::logger_obj, logEvent)

#endif