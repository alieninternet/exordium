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

class Log
{
private:
	String logFilename;
	static std::ofstream logFile;

public:
	static void init(const Config &);
	static void logLine (String const &);

};

};
#endif

