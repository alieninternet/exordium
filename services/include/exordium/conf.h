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

# include <libais/config/data.h>
# include <libais/string/string.h>
# include <exordium/modules.h>

namespace Exordium {
   class Config : public LibAIS::ConfigData {
    public:
      // The definition table used by Kine's configuration parser
      static const LibAIS::ConfigParser::defTable_type definitionTable;

    private:
      // Variables (top class from the definition table above)
      LibAIS::String defLogfile;		// Log filename
      LibAIS::String defServicesDescription;    // Services description (aka realname)
      LibAIS::String defServicesHostname;       // Services hostname
      LibAIS::String defUplinkHost;           	// Uplink Host (temporary)
      unsigned short defUplinkPort;		// Uplink Port (temporary)

      // 'CONSOLE' class and variables
      static const LibAIS::ConfigParser::defTable_type defClassConsole;
      LibAIS::String defConsoleDescription;	// Console's real name field
      bool defConsoleEnabled;			// Is the console active?
      LibAIS::String defConsoleHostname;	// The console's host name
      LibAIS::String defConsoleName;		// The console's nickname
      
      // 'MODULE' class/variable stuff
      Modules defModules;			// Loaded modules
      static LIBAIS_CONFIG_CLASS_HANDLER(classHandleModule);
      static LIBAIS_CONFIG_VARIABLE_HANDLER(varHandleModule);
      
      // 'SQL' class and variables
      static const LibAIS::ConfigParser::defTable_type defClassSql;
      LibAIS::String defSqlDatabase;		// MySql server database name
      LibAIS::String defSqlHostname;		// MySql server hostname
      LibAIS::String defSqlPassword;		// MySql server password
      unsigned short defSqlPort;		// MySql server port
      LibAIS::String defSqlUsername;		// MySql server username
      
    public:
      // Constructor (sets up defaults mainly)
      Config(void);
      
      // Top variables
      const LibAIS::String& getLogfile(void) const
	{ return defLogfile; };
      const LibAIS::String& getUplinkHost(void) const
	{ return defUplinkHost; };      
      const unsigned short getUplinkPort(void) const
        { return defUplinkPort; };
      const LibAIS::String& getServicesHostname(void) const
        { return defServicesHostname; };
      const LibAIS::String& getServicesDescription(void) const
        { return defServicesDescription; };

      // 'CONSOLE' class
      const LibAIS::String& getConsoleDescription(void) const
	{ return defConsoleDescription; };
      const bool getConsoleEnabled(void) const
	{ return defConsoleEnabled; };
      const LibAIS::String& getConsoleHostname(void) const
	{ return defConsoleHostname; };
      const LibAIS::String& getConsoleName(void) const
	{ return defConsoleName; };
      
      // 'MODULE' class
      Modules& getModules(void)
	{ return defModules; };
      
      // 'SQL' class
      const LibAIS::String& getSqlDatabase(void) const
	{ return defSqlDatabase; };
      const LibAIS::String& getSqlHostname(void) const
	{ return defSqlHostname; };
      const LibAIS::String& getSqlPassword(void) const
	{ return defSqlPassword; };
      const unsigned short getSqlPort(void) const
	{ return defSqlPort; };
      const LibAIS::String& getSqlUsername(void) const
	{ return defSqlUsername; };
   };
}; // namespace Exordium

#endif

