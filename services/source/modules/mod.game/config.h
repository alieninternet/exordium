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

#ifndef _SOURCE_MODULES_GAME_CONFIG_H_
# define _SOURCE_MODULES_GAME_CONFIG_H_ 1

# include <kineircd/config.h>
# include <exordium/module.h>

namespace Exordium {
   namespace GameModule {
      // Configuration data class (inherited from the primary module config)
      class ConfigData : public Exordium::Module::ConfigData {
       private:
	 // Configuration information
	 static const AISutil::ConfigParser::defTable_type defTable;
	 
	 const AISutil::String defModule;
	 static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleModule);
	 
       public:
      // Constructor
	 ConfigData(const char* const d, const char* const n)
	   : Exordium::Module::ConfigData(d,
					  Kine::config().getOptionsServerName(),
					  n, n)
	     {};
	 
	 // Grab the configuration definition table (over-write the original)
	 const AISutil::ConfigParser::defTable_type& getDefinitions(void) const
	   { return defTable; };
	 
	 const AISutil::String& getModule(void) const
	   { return defModule; };
      }; // class ConfigData
   }; // namespace GameModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_GAME_CONFIG_H_


