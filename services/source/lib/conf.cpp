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
        "SERVICESHOSTNAME",
          (void *)&Config::defServicesHostname, &varHandleHostName,
          0, 0
     },
     {
	"UPLINKHOST",
	  (void *)&Config::defUplinkHost, &varHandleHostName,
	  0, 0
     },
     {
        "SERVICESDESCRIPTION",
          (void *)&Config::defServicesDescription, &varHandleString,
          0, 0
     },
     {
        "CONSOLEHOSTNAME",
          (void *)&Config::defServicesConsoleHostname, &varHandleHostName,
          0, 0
     },
     {
        "CONSOLEDESCRIPTION",
          (void *)&Config::defServicesConsoleDescription, &varHandleString,
          0, 0
     },
     {
        "UPLINKPORT",
          (void *)&Config::defUplinkPort, &varHandleUnsignedShortNoZero,
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
