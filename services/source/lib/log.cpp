/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 *
 * This file is a part of Exordium.
 * 
 * Exordium is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Exordium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Exordium; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * For contact details for the IRCDome Development Team please see the
 * enclosed README file.
 *
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
