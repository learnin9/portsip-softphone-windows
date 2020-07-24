#include "stdafx.h"
#include "Logger.h" 
#include <string.h>
#include <iostream>
#include <tchar.h>
#include <log4cplus/configurator.h>  
#include <log4cplus/helpers/loglog.h>  
#include <log4cplus/helpers/stringhelper.h>
using namespace std;
log4cplus::Logger LoggerLocal::logger_obj;
bool LoggerLocal::init(std::string  config) {
	log4cplus::tstring file = LOG4CPLUS_C_STR_TO_TSTRING(config);
	log4cplus::initialize();
    log4cplus::PropertyConfigurator::doConfigure(file);
	Logger logger = Logger::getRoot();
	logger_obj = logger;
	return true;
}

void LoggerLocal::setloglevel(std::string strloglevel)
{

	int nLogLevel = OFF_LOG_LEVEL;
	if (strloglevel.compare("TRACE") == 0)
	{
		nLogLevel  =  TRACE_LOG_LEVEL; 
	}
	else if (strloglevel.compare("DEBUG") == 0)
	{
		nLogLevel =  DEBUG_LOG_LEVEL;
	}
	else if (strloglevel.compare("INFO") == 0)
	{
		nLogLevel =  INFO_LOG_LEVEL;
	}
	else if (strloglevel.compare("ERROR") == 0)
	{
		nLogLevel =  ERROR_LOG_LEVEL;
	}
	else if (strloglevel.compare("FATAL") == 0)
	{
		nLogLevel = FATAL_LOG_LEVEL;
	}
	logger_obj.setLogLevel(nLogLevel);
	return;
}