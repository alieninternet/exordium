/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _INCLUDE_EXORDIUM_CONFIG_H_
# define _INCLUDE_EXORDIUM_CONFIG_H_ 1

# include <aisutil/config/data.h>
# include <aisutil/string/string.h>
# include <exordium/database/database.h>

namespace Exordium {
   class Config : public AISutil::ConfigData {
    protected:
      // Variables (top class from the definition table above)
      AISutil::String defUnderlingHostname;	// Underling server hostname
      AISutil::String defUplinkPass;

      // 'SQL' variables
      AISutil::String defSqlDatabase;		// MySql server database name
      AISutil::String defSqlHostname;		// MySql server hostname
      AISutil::String defSqlPassword;		// MySql server password
      unsigned short defSqlPort;		// MySql server port
      bool defSqlSSLEnabled;			// Mysql SSL Boolean
      AISutil::String defSqlUsername;		// MySql server username
      AISutil::String defSqlEngine;             // Database engine
      
      // Constructor (sets up defaults mainly)
      Config(void);

    public:
      // Destructor
      virtual ~Config(void)
	{};
      
      // Top variables
      const AISutil::String& getUnderlingHostname(void) const
	{ return defUnderlingHostname; };      
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
      const bool getSqlSSLEnabled(void) const
	{ return defSqlSSLEnabled; };
	    
   };
}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_CONFIG_H_
