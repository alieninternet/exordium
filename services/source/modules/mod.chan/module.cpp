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

#include "chan.h"
#include "tables.h"
#include "language.h"

#include <aisutil/string/string.h>
#include <kineircd/config.h>
#include <exordium/module.h>

using AISutil::String;

// This anonymous namespace is intentional
namespace {
   // Module information structure
   static const Exordium::Module::Info moduleInfo = {
      "Chan", "Channel Service",
      0, 0
   };
      
   class Module : public Exordium::Module {
    private:
      // Pointer to the service this module contains
      Exordium::ChanModule::Service* service;

      // Configuration data
      ConfigData configData;
      
    public:
      // Constructor
      Module(void)
	: service(0),
          configData(moduleInfo.fullName,
		     Kine::config().getOptionsServerName(),
		     moduleInfo.shortName, moduleInfo.shortName)
	{};
      
      // Destructor
      ~Module(void) {
	 // Deregister the language tag map
	 (void)Kine::langs().
	   deregisterMap(Exordium::ChanModule::Language::tagMap);
	 
	 // If the service was created, deregister it and delete it from memory
	 if (service != 0) {
	    delete service;
	 }
      }

      // Start the service
      bool start(Exordium::Services& s) {
	 // Initialise the language tag map thingy
	 (void)Kine::langs().
	   registerMap(Exordium::ChanModule::Language::tagMap);
	 
	 // Attempt to affirm our database tables..
	 unsigned int i = 0;
	 while (Exordium::ChanModule::Tables::tables[i] != 0) {
	    // Try to affirm this table..
	    if (!s.getDatabase().
		affirmTable(*(Exordium::ChanModule::Tables::tables[i]))) {
	       s.logLine(String("Unable to affirm mod_chan database "
				"table '") +
			 Exordium::ChanModule::Tables::tables[i]->name + "'",
			 Exordium::Log::Fatality); 
	       return false;
	    }
	    
	    // Next table..
	    ++i;
	 }
	 
	 // Make a new service
	 if ((service = new Exordium::ChanModule::Service(configData, s)) ==
	     0) {
	    return false;
	 }
	 
	 // All is well
	 return true;
      }
      
      // Return the module info
      const Info& getModuleInfo(void) const
	{ return moduleInfo; };
      
      // Return an appropriate instance of a configuration data class
      const ConfigData& getConfigData(void) const
	{ return configData; };
      ConfigData& getConfigData(void)
	{ return configData; };
      
      // Return the service this module has
      Exordium::Service* const getService(void) const
	{ return service; };
   }; // struct Module
}; // namespace {anonymous}


EXORDIUM_MODULE_INIT_FUNCTION
{
   return new Module();
}
