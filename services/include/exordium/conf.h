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
#define _CONF_H_

#include <kineircd/configdata.h>

using Kine::String;

namespace Exordium {

   class Config : public Kine::ConfigData {
    public:
      // The definition table used by Kine's configuration parser
      static const Kine::ConfigParser::defTable_type definitionTable;

    private:
      // Variables (top class from the definition table above)
      String defLogfile;			// Log filename
      String defUplinkHost;        		// Uplink Host
      String defServicesHostname;               // Services hostname
      String defServicesDescription;            // Services description (aka realname)
      String defServicesConsoleHostname;        // Console's hostname
      String defServicesConsoleDescription;     // Console's description
      unsigned short defUplinkPort;             // Uplink Port
      
      // 'SQL' class and variables
      static const Kine::ConfigParser::defTable_type defClassSql;
      String defSqlDatabase;			// MySql server database name
      String defSqlHostname;			// MySql server hostname
      String defSqlPassword;			// MySql server password
      unsigned short defSqlPort;		// MySql server port
      String defSqlUsername;			// MySql server username
      
    public:
      // Constructor (sets up defaults mainly)
      Config(void);
      
      // Top variables
      const String &getLogfile(void) const
	{ return defLogfile; };
      const String &getUplinkHost(void) const
	{ return defUplinkHost; };      
      const unsigned short getUplinkPort(void) const
        { return defUplinkPort; };
      const String &getServicesHostname(void) const
        { return defServicesHostname; };
      const String &getServicesDescription(void) const
        { return defServicesDescription; };
      const String &getServicesConsoleHostname(void) const
        { return defServicesConsoleHostname; };
      const String &getServicesConsoleDescription(void) const
        { return defServicesConsoleDescription; };
      // 'SQL' class
      const String &getSqlDatabase(void) const
	{ return defSqlDatabase; };
      const String &getSqlHostname(void) const
	{ return defSqlHostname; };
      const String &getSqlPassword(void) const
	{ return defSqlPassword; };
      const unsigned short getSqlPort(void) const
	{ return defSqlPort; };
      const String &getSqlUsername(void) const
	{ return defSqlUsername; };
   };
}; // namespace Exordium

#endif

