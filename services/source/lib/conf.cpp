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
const Kine::ConfigParser::defTable_type Config::definitionTable =
{
     {
	"LOGFILE",
	  (void *)&Config::defLogfile, &varHandleString,
	  0, 0
     },
     {
	"UPLINK",
	  (void *)&Config::defUplink, &varHandleString,
	  0, 0
     },
     {
	"SQL",
	  0, 0,
	  &defClassSql, 0
     },
     { 0, 0, 0, 0, 0 }
};

// 'SQL' class definition table
const Kine::ConfigParser::defTable_type Config::defClassSql =
{
     {
	"DATABASE",
	  (void *)&Config::defSqlDatabase, &varHandleString,
	  0, 0
     },
     {
	"HOSTNAME",
	  (void *)&Config::defSqlHostname, &varHandleHostName,
	  0, 0
     },
     {
	"PASSWORD",
	  (void *)&Config::defSqlPassword, &varHandleString,
	  0, 0
     },
     {
	"PORT",
	  (void *)&Config::defSqlPort, &varHandleUnsignedShortNoZero,
	  0, 0
     },
     {
	"USERNAME",
	  (void *)&Config::defSqlUsername, &varHandleString,
	  0, 0
     },
     { 0, 0, 0, 0, 0 }
};

// Constructor to set up defaults, mainly. These defaults are dopey :(
Config::Config(void)
  : defLogfile("services.log"),

// 'SQL' class
defSqlDatabase("services"),
defSqlHostname("localhost"),
defSqlPassword(""),
defSqlPort(3306), // mysql port, as assigned by the iana
defSqlUsername("root")
{
   // nothing here!
};
