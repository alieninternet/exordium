/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/log.h"
#include <time.h>
#include <iomanip>
#include <iostream>
#include <kineircd/str.h>
#include <fstream>

using Kine::String;
using namespace Exordium;


Log::Log(const Config& c)
  : config(c)
{
   logFile.open(config.getLogfile().c_str(),
		std::ios::out | std::ios::ate | std::ios::app);
}


void Log::logLine (const String& line)
{
   struct tm *d;
   time_t current_time = time (0);
   d = localtime (&current_time);
   logFile << "[" << std::setfill ('0') << std::setw (2)
     << d->tm_mday << "/" << std::setfill ('0') << std::setw (2)
       << d->tm_mon + 1 << "/"
     << d->tm_year + 1900 << " - " << std::setfill ('0') << std::setw (2)
       << d->tm_hour << ":" << std::setfill ('0') << std::setw (2)
	 << d->tm_min << ":" << std::setfill ('0') << std::setw (2)
	   << d->tm_sec << "] " << line << std::endl;
}
