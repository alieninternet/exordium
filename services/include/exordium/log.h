/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <kineircd/str.h>
#include "exordium/conf.h"

using Kine::String;

namespace Exordium {
   class Log {
    private:
      const Config& config;
      
      std::ofstream logFile;
      
    public:
      Log(const Config& c);
	
      void logLine(const String& line);
   };

};
#endif

