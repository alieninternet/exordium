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
#include "exordium/services.h"

using Kine::String;

namespace Exordium {

class Log
{
private:
	String logFilename;
	std::ofstream logFile;
	Services &services;
	Config &config;
public:
	Log(Services &s, Config &c)
		: services(s),
		  config(c),
		 {};
	 void init();
	 void logLine (String const &);

};

};
#endif

