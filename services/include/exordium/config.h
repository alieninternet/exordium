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

#ifndef _SOURCE_LIB_CONFIG_H_
# define _SOURCE_LIB_CONFIG_H_ 1

# include "exordium/config.h"
# include "exordium/database/database.h"
# include "exordium/singleton.h"
# include "modules.h"
# include "service.h"

namespace Exordium {

   #define configFwd (*Config::instance())

   class Config : public AISutil::ConfigData, public Singleton<Config> {
    public:
      friend class Singleton<Config>;

      // The definition table used by Kine's configuration parser
      static const AISutil::ConfigParser::defTable_type definitionTable;


    private:

      Config(void);

      // Variables (top class from the definition table above)
      AISutil::String defUnderlingHostname;     // Underling server hostname
      AISutil::String defServicesDescription;   // Services description (aka realname)
      AISutil::String defServicesHostname;      // Services hostname
      AISutil::String defUnderlingDescription;	// Underling server description
      AISutil::String defUplinkHost;           	// Uplink Host (temporary)
      unsigned short defUplinkPort;		// Uplink Port (temporary)

      // 'CONSOLE' class and variables
      static const AISutil::ConfigParser::defTable_type defClassConsole;
      AISutil::String defConsoleDescription;	// Console's real name field
      bool defConsoleEnabled;			// Is the console active?
      AISutil::String defConsoleHostname;	// The console's host name
      AISutil::String defConsoleName;		// The console's nickname

      // 'DATABASE' class
      AISutil::String defDatabase;		// Our database engine
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleDatabase);
      
      // 'MODULE' class/variable stuff
      Modules defModules;			// Loaded modules
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleModule);
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleModule);
      
      // 'SQL' class
      static const AISutil::ConfigParser::defTable_type defClassSql;


      // 'SQL' variables
      AISutil::String defSqlDatabase;           // MySql server database name
      AISutil::String defSqlHostname;           // MySql server hostname
      AISutil::String defSqlPassword;           // MySql server password
      unsigned short defSqlPort;                // MySql server port
      AISutil::String defSqlUsername;           // MySql server username
      AISutil::String defSqlEngine;             // Database engine

    public:
      
      // Destructor
      ~Config(void);
      
      // Check the configuration (fail-safe for when the module starts)
      const bool checkConfig(void) const;
      
      // Top variables
      const AISutil::String& getUnderlingHostname(void) const
        { return defUnderlingHostname; };
      const AISutil::String& getServicesHostname(void) const
        { return defServicesHostname; };
      const AISutil::String& getServicesDescription(void) const
        { return defServicesDescription; };
      const AISutil::String& getUnderlingDescription(void) const
	{ return defUnderlingDescription; };      
      const AISutil::String& getUplinkHost(void) const
	{ return defUplinkHost; };      
      const unsigned short getUplinkPort(void) const
        { return defUplinkPort; };

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
      const AISutil::String& getDatabase(void)
	{ return defDatabase; };
      
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

#endif // _SOURCE_LIB_CONFIG_H_
