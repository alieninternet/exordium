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

#include "serv.h"
#include "tables.h"
#include "language.h"

#include <kineircd/config.h>
#include <exordium/module.h>


// This anonymous namespace is intentional
namespace {
   // Module information structure
   static const Exordium::Module::Info moduleInfo = {
      "Serv", "Service Service", // <=- hehehe :)
      0, 0
   };
      
   class Module : public Exordium::Module {
    private:
      // Configuration data
      ConfigData configData;
      
    public:
      // Constructor
      Module(void)
	: configData(moduleInfo.fullName,
		     Kine::config().getOptionsServerName(),
		     moduleInfo.shortName, moduleInfo.shortName)
	{};
      
      // Destructor
      ~Module(void) {
	 // Deregister the language tag map
	 (void)Kine::langs().
	   deregisterMap(Exordium::ServModule::Language::tagMap);
      }

      // Start the service
      Exordium::Service* const realStart(Exordium::Services& s) {
	 // Initialise the language tag map thingy
	 (void)Kine::langs().
	   registerMap(Exordium::ServModule::Language::tagMap);
	 
	 // Attempt to affirm our database table..
	 if (!s.getDatabase().
	     affirmTable(Exordium::ServModule::Tables::serverlistTable)) {
	    s.logLine("Unable to affirm mod_serv database table "
		      "'serverlist'",
		      Exordium::Log::Fatality);
	    return 0;
	 }
	 
	 // Make a new service
	 return new Exordium::ServModule::Service(configData, s);
      }
      
      // Return the module info
      const Info& getModuleInfo(void) const
	{ return moduleInfo; };
      
      // Return an appropriate instance of a configuration data class
      const ConfigData& getConfigData(void) const
	{ return configData; };
      ConfigData& getConfigData(void)
	{ return configData; };
   }; // struct Module
}; // namespace {anonymous}


EXORDIUM_MODULE_INIT_FUNCTION
{
   return new Module();
}
