/*
    MAIN.CPP   - Main entry point for IRCDome Network Services
    Version   - 0.1
    Date      - 8th May 2002
    Revisions -
  
    Copyright 2002 James Ian Wilkins <james@ircdome.org>

*/

#include <iostream>
#include "exordium/services.h"
#include "exordium/love.h"
#include "exordium/log.h"
#include "exordium/sql.h"
#include <signal.h>
/* #include <dlfcn.h> */

using Kine::String;
using namespace Exordium;

Sql db;

int main(void)
{
	Services me;
	signal(SIGPIPE, SIG_IGN);
	Log::init();
	Log::logLine("Services started, beginning initalisation");
	me.load_config();
	db.init(me.mysqlHost,me.mysqlUser,me.mysqlPass,me.mysqlDb);
	me.init();
	me.run();
	Log::logLine("Services terminated - Normal exit");
	exit(0); // If we ever get here, its a normal exit.
}
