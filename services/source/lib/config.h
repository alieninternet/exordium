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

# include <kineircd/clientname.h>
# include <kineircd/channelname.h>

# include "exordium/config.h"
# include "exordium/database/database.h"
# include "modules.h"
# include "logger.h"

namespace Exordium {
   class ConfigInternal : public Exordium::Config {
    public:
      // The definition table used by Kine's configuration parser
      static const AISutil::ConfigParser::defTable_type definitionTable;

    private:
      // Variables (top class from the definition table above)
      Kine::Logger::Mask::lazy_type defLogMask;	// Database logging mask
      AISutil::String defUnderlingDescription;	// Underling server description
      AISutil::String defUplinkHost;           	// Uplink Host (temporary)
      unsigned short defUplinkPort;		// Uplink Port (temporary)
      AISutil::String defUplinkPass;		// Uplink Pass (temporary)

      // 'CONSOLE' class and variables
      static const AISutil::ConfigParser::defTable_type defClassConsole;
      AISutil::String defConsoleDescription;	// Console's real name field
      bool defConsoleEnabled;			// Is the console active?
      AISutil::String defConsoleHostname;	// The console's host name
      Kine::ClientName defConsoleName;		// The console's nickname
      Kine::ChannelName defConsoleChan;		// Console's channel

      // 'DATABASE' class
      CDatabase* defDatabase;			// Our database engine
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleDatabase);
      
      // 'MODULE' class/variable stuff
      Modules defModules;			// Loaded modules
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleModule);
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleModule);
      
      // 'SQL' class
      static const AISutil::ConfigParser::defTable_type defClassSql;

    public:
      // Constructor
      ConfigInternal(void);
      
      // Destructor
      ~ConfigInternal(void);
      
      // Check the configuration (fail-safe for when the module starts)
      const bool checkConfig(void) const;
      
      // Top variables
      const AISutil::String& getUnderlingDescription(void) const
	{ return defUnderlingDescription; };      
      const AISutil::String& getUplinkHost(void) const
	{ return defUplinkHost; };      
      const unsigned short getUplinkPort(void) const
        { return defUplinkPort; };
      const AISutil::String& getUplinkPass(void) const
	{ return defUplinkPass; };
      const Kine::Logger::Mask::lazy_type getLogMask(void) const
	{ return defLogMask; };
      
      // 'CONSOLE' class
      const AISutil::String& getConsoleDescription(void) const
	{ return defConsoleDescription; };
      const bool getConsoleEnabled(void) const
	{ return defConsoleEnabled; };
      const AISutil::String& getConsoleHostname(void) const
	{ return defConsoleHostname; };
      const Kine::ClientName& getConsoleName(void) const
	{ return defConsoleName; };
      const Kine::ChannelName& getConsoleChan(void) const
        { return defConsoleChan; };

      // 'DATABASE' class
      CDatabase& getDatabase(void)
	{ return *defDatabase; };
      
      // 'MODULE' class
      Modules& getModules(void)
	{ return defModules; };
   };
}; // namespace Exordium

#endif // _SOURCE_LIB_CONFIG_H_
