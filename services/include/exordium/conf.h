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

#ifndef _CONF_H_
# define _CONF_H_

# include <aisutil/config/data.h>
# include <aisutil/string/string.h>
# include <exordium/database/database.h>
# include <exordium/modules.h>

namespace Exordium {
   class Config : public AISutil::ConfigData {
    public:
      // The definition table used by Kine's configuration parser
      static const AISutil::ConfigParser::defTable_type definitionTable;

    private:
      // Variables (top class from the definition table above)
      AISutil::String defServicesDescription;   // Services description (aka realname)
      AISutil::String defServicesHostname;      // Services hostname
      AISutil::String defUplinkHost;           	// Uplink Host (temporary)
      unsigned short defUplinkPort;		// Uplink Port (temporary)

      // 'CONSOLE' class and variables
      static const AISutil::ConfigParser::defTable_type defClassConsole;
      AISutil::String defConsoleDescription;	// Console's real name field
      bool defConsoleEnabled;			// Is the console active?
      AISutil::String defConsoleHostname;	// The console's host name
      AISutil::String defConsoleName;		// The console's nickname

      // 'DATABASE' class
      CDatabase* defDatabase;			// Our database engine
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleDatabase);
      
      // 'MODULE' class/variable stuff
      Modules defModules;			// Loaded modules
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleModule);
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleModule);
      
      // 'SQL' class and variables
      static const AISutil::ConfigParser::defTable_type defClassSql;
      AISutil::String defSqlDatabase;		// MySql server database name
      AISutil::String defSqlHostname;		// MySql server hostname
      AISutil::String defSqlPassword;		// MySql server password
      unsigned short defSqlPort;		// MySql server port
      AISutil::String defSqlUsername;		// MySql server username
      AISutil::String defSqlEngine;             // Database engine
      
    public:
      // Constructor (sets up defaults mainly)
      Config(void);
      
      // Destructor
      ~Config(void);
      
      // Check the configuration (fail-safe for when the module starts)
      bool checkConfig(void);
      
      // Top variables
      const AISutil::String& getUplinkHost(void) const
	{ return defUplinkHost; };      
      const unsigned short getUplinkPort(void) const
        { return defUplinkPort; };
      const AISutil::String& getServicesHostname(void) const
        { return defServicesHostname; };
      const AISutil::String& getServicesDescription(void) const
        { return defServicesDescription; };

      // 'CONSOLE' class
      const AISutil::String& getConsoleDescription(void) const
	{ return defConsoleDescription; };
      const bool getConsoleEnabled(void) const
	{ return defConsoleEnabled; };
      const AISutil::String& getConsoleHostname(void) const
	{ return defConsoleHostname; };
      const AISutil::String& getConsoleName(void) const
	{ return defConsoleName; };

      // 'DATABASE' class
      CDatabase& getDatabase(void)
	{ return *defDatabase; };
      
      // 'MODULE' class
      Modules& getModules(void)
	{ return defModules; };
      
      // 'SQL' class
      const AISutil::String& getSqlDatabase(void) const
	{ return defSqlDatabase; };
      const AISutil::String& getSqlHostname(void) const
	{ return defSqlHostname; };
      const AISutil::String& getSqlPassword(void) const
	{ return defSqlPassword; };
      const unsigned short getSqlPort(void) const
	{ return defSqlPort; };
      const AISutil::String& getSqlUsername(void) const
	{ return defSqlUsername; };
      const AISutil::String& getSqlEngine(void) const
        { return defSqlEngine; };
   };
}; // namespace Exordium

#endif

