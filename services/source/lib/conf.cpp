/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/config.h"
#include "exordium/conf.h"

using namespace Exordium;


// Information used by the configuration parser
const Kine::ConfigParser::defTable_type Config::definitionTable = {
     {
	"LOGFILE",
	  (void *)&Config::defLogFile, &varHandleString,
	  0, 0
     },
     {
	"MYSQLDB",
	  (void *)&Config::defMySqlDb, &varHandleString,
	  0, 0
     },
     {
	"MYSQLHOST",
	  (void *)&Config::defMySqlHost, &varHandleHostName,
	  0, 0
     },
     {
	"MYSQLPASS",
	  (void *)&Config::defMySqlPass, &varHandleString,
	  0, 0
     },
     {
	"MYSQLPORT",
	  (void *)&Config::defMySqlPort, &varHandleUnsignedShortNoZero,
	  0, 0
     },
     {
	"MYSQLUSER",
	  (void *)&Config::defMySqlUser, &varHandleString,
	  0, 0
     },
     {
	"UPLINKHOST",
	  (void *)&Config::defUplinkHost, &varHandleString,
	  0, 0
     },
     { 0, 0, 0, 0, 0 }
};


// Constructor to set up defaults, mainly. These defaults are dopey :(
Config::Config(void)
  : defLogFile("services.log"),
    defMySqlDb("exordium"),
    defMySqlHost("localhost"),
    defMySqlPass("exordium"),
    defMySqlPort(3306), // mysql port, as assigned by the iana
    defMySqlUser("exordium"),
    defUplinkHost("127.0.0.1") // this is obsolete.. well eventually it will be
{
   // nothing here!
};
