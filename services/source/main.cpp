/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
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
