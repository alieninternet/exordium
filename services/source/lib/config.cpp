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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "config.h"

#ifdef DEBUG
# include <cassert>
#endif

using namespace Exordium;


// Information used by the configuration parser
const AISutil::ConfigParser::defTable_type ConfigInternal::definitionTable =
{
     {
	"CONSOLE", 7,
	  0, 0,
	  &defClassConsole, 0
     },
     {
	"DATABASE", 4,
	  (void *)&ConfigInternal::defDatabase, 0,
	  0, &classHandleDatabase
     },
     {
	"MODULE", 3,
	  (void *)&ConfigInternal::defModules, &varHandleModule,
	  0, &classHandleModule
     },
     {
	"SQL", 3,
	  0, 0,
	  &defClassSql, 0
     },
     {
	"UNDERLINGDESCRIPTION", 13,
	  (void *)&ConfigInternal::defUnderlingDescription, &varHandleString,
	  0, 0
     },
     {
	"UNDERLINGHOSTNAME", 13,
	  (void *)&Config::defUnderlingHostname, &varHandleHostName,
	  0, 0
     },
     { // This should be temporary, being a server is Kine's job
	"UPLINKHOST", 10,
	  (void *)&ConfigInternal::defUplinkHost, &varHandleHostName,
	  0, 0
     },
     { // This should be temporary, being a server is Kine's job
        "UPLINKPORT", 10,
          (void *)&ConfigInternal::defUplinkPort, &varHandleUnsignedShortNoZero,
          0, 0
     },
     {
	"UPLINKPASS", 10,
	  (void *)&ConfigInternal::defUplinkPass, &varHandleString,
	  0, 0
     },
     { 0, 0, 0, 0, 0, 0 }
};


// 'CONSOLE' class definition table
const AISutil::ConfigParser::defTable_type ConfigInternal::defClassConsole =
{
     {
        "DESCRIPTION", 4,
          (void *)&ConfigInternal::defConsoleDescription, &varHandleString,
          0, 0
     },
     {
        "ENABLED", 6,
          (void *)&ConfigInternal::defConsoleEnabled, &varHandleBoolean,
          0, 0
     },
     {
        "HOSTNAME", 4,
          (void *)&ConfigInternal::defConsoleHostname, &varHandleHostName,
          0, 0
     },
     {
        "NAME", 4,
          (void *)&ConfigInternal::defConsoleName, &varHandleStringOneWord,
          0, 0
     },
     {
	"CHAN", 4,
	  (void *)&ConfigInternal::defConsoleChan, &varHandleStringOneWord,
	  0, 0
     },
     { 0, 0, 0, 0, 0, 0 }
};


// 'SQL' class definition table
const AISutil::ConfigParser::defTable_type ConfigInternal::defClassSql =
{
     {
        "DATABASE", 8,
          (void *)&Config::defSqlDatabase, &varHandleStringOneWord,
          0, 0
     },
     {
        "ENGINE", 6,
          (void *)&Config::defSqlEngine, &varHandleString,
          0, 0
     },
     {
        "HOSTNAME", 4,
          (void *)&Config::defSqlHostname, &varHandleHostName,
          0, 0
     },
     {
        "PASSWORD", 4,
          (void *)&Config::defSqlPassword, &varHandleString,
          0, 0
     },
     {
        "PORT", 4,
          (void *)&Config::defSqlPort, &varHandleUnsignedShortNoZero,
          0, 0
     },
     {
        "USERNAME", 4,
          (void *)&Config::defSqlUsername, &varHandleStringOneWord,
          0, 0
     },
     { 0, 0, 0, 0, 0, 0 }
};


/* Config - Constructor to set up defaults, mainly. These defaults are dopey :(
 * Original 25/07/2002 pickle
 */
Config::Config(void)
  : defUnderlingHostname(/* intentionally empty */),

    // 'SQL' class
    defSqlDatabase("services"),
    defSqlHostname("localhost"),
    defSqlPassword(""),
    defSqlPort(3306), // mysql port, as assigned by the iana
    defSqlUsername("root")
{}


/* ConfigInternal - Constructor
 * Original 25/07/2002 pickle
 */
ConfigInternal::ConfigInternal(void)
  : defUnderlingDescription(/* intentionally empty */),
    defUplinkHost("irc.somenetwork.somewhere"), // temporary
    defUplinkPort(6667), // temporary

    // 'CONSOLE' class
    defConsoleDescription("Exordium Services Console"),
    defConsoleEnabled(false),
    defConsoleHostname("somewhere.org"),
    defConsoleName("Exordium"),

    // 'DATABASE' class
    defDatabase(0)
{}


/* ~ConfigInternal - Destructor
 * Original 03/10/2002 pickle
 */
ConfigInternal::~ConfigInternal(void)
{
   // Delete the database engine
   if (defDatabase != 0) {
      delete defDatabase;
   }
}


/* checkConfig - Check the configuration has been done correctly (fail-safe)
 * Original 03/10/2002 pickle
 */
const bool ConfigInternal::checkConfig(void) const
{
   // If the database engine has not been initialised, we are broken
   if (defDatabase == 0) {
      return false;
   }
   
   // Otherwise, everything is okay
   return true;
}


/* classHandleDatabase - Handle a database{}; configuration class
 * Original ...
 */
LIBAISUTIL_CONFIG_CLASS_HANDLER(ConfigInternal::classHandleDatabase)
{
   // Check if the first value is empty (the database engine type field)
   if (values.empty() || values.front().empty()) {
      errString = "No engine specified";
      return false;
   }
   return false;
}


/* classHandleModule - Handle a module{}; configuration class
 * Original 21/07/2002 pickle
 * 18/09/2002 pickle - Modified to suit Exordium
 */
LIBAISUTIL_CONFIG_CLASS_HANDLER(ConfigInternal::classHandleModule)
{
#ifdef DEBUG
   // Preserve sanity..
   assert(dataVariable != 0);
#endif

   // Check if the first value is empty (the filename field)
   if (values.front().empty()) {
      // Get cranky
      errString = "No module filename supplied!";
      return false;
   }

   // Attempt to open the module
   Service* const service = 
     ((dataClass.*((Modules ConfigData::*)dataVariable)).
      loadModule(values.front().trim(), errString));
   
   // Make sure the module was loaded
   if (service == 0) {
      return false;
   }
   
   // Parse the module's configuration data
   return
     AISutil::ConfigParser::parse(configData, position,
				  ((void *)
				   &(service->getConfigData().getDefinitions())),
				  service->getConfigData());
}


/* varHandleModule - Handle a module; configuration variable
 * Original 21/07/2002 pickle
 * 18/09/2002 pickle - Modified to suit Exordium
 * Note: This is the same as the class handler, except the module is 
 *       responsible for the configuration of its defaults. Remember that 
 *       the AISutil::ConfigParser routines are designed to be totally 
 *       passive..
 */
LIBAISUTIL_CONFIG_VARIABLE_HANDLER(ConfigInternal::varHandleModule)
{
#ifdef DEBUG
   // Preserve sanity..
   assert(dataVariable != 0);
#endif

   // Check if the first value is empty (the filename field)
   if (values.front().empty()) {
      // Get cranky
      errString = "No module filename supplied!";
      return false;
   }

   // Attempt to open the module
   return ((dataClass.*((Modules ConfigData::*)dataVariable)).
	   loadModule(values.front().trim(), errString) != 0);
}
