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

#include <iostream>
#ifdef DEBUG
# include <cassert>
#endif
#include <exordium/config.h>
#include <exordium/services.h>
#include <kineircd/module.h>
#include "version.h"

#include <exordium/regchannel.h>
#include <exordium/reguser.h>

using namespace Exordium;


// This namespace is intentionally anonymous
namespace {

   void initListes(void)
   {
     RegisteredUserList::instance()->SyncDB();
     RegisteredChannelList::instance()->SyncDB();

   }  



   // Full version/copyright/author information (see version.h)
   static const char* const versionInfo[] = EXORDIUM_VER_INFO;
   
   // information about ourselves
   static const Kine::Module::Info moduleInfo = {
      // List our version/copyright information (see version.h)
      EXORDIUM_NAME_SHORT,
      EXORDIUM_NAME_LONG,
      EXORDIUM_COPYRIGHT,
      EXORDIUM_VER_MAJOR,
      EXORDIUM_VER_MINOR,
      EXORDIUM_VER_PATCH,
      EXORDIUM_VER_EXTRA,
      (const char** const)&versionInfo,
	
      /* Note that I set up the version numbers above on purpose to
       * illustrate how the version fields come together. The above comes
       * out as follows:
       *     exordium-0.1.2.beta
       * If ".beta" was replaced with 0, it would look like this:
       *     exordium-0.1.2
       * If then you replaced the patch-level with 0, it would look like
       * this:
       *     exordium-0.1
       * The other fields are always visible. The full name is used in other
       * places.. same with the copyright.. probably on module loading or
       * something similar *shrugs*
       */

      // Flags -- We do not want to be loaded more than once :)
      Kine::Module::Flags::UNIQUE_INSTANCE,

      // Configuration information
      &Config::definitionTable
   };

   
   class mod_exordium : public Kine::Module {

    public:
      // Constructor
      mod_exordium(void)
	{};
      
      // Destructor
      ~mod_exordium(void) {
      };

      // Return the information about ourselves
      const Kine::Module::Info& getInfo(void) const
	{ return moduleInfo; };

      // Return our configuration data class
      AISutil::ConfigData* const getConfigData(void)
	{ return Config::instance(); };
      
      // this is called just before the module is actually going to be used
      bool start(void) {
/* Uncomment this when the database engine config thing is complete       
	 // Check the configuration was completely setup, otherwise get cranky
	 if (!ConfigInternal::instance()->checkConfig()) {
	    // Tell KineIRCd that we refuse to start
	    return false;
	 }
 */
	   

#ifdef DEBUG
	 std::clog << "Services started, beginning initalisation" << std::endl;
#endif

         // Initialize our lists
         initListes();

	 servicesFwd.run();
#ifdef DEBUG
	 std::clog << "Services terminated - Normal exit" << std::endl;
#endif
	 exit(0); // we are naughty using this here..... very naughty.. :(
	 
	 // Tell Kine that we started happily
	 return true;
      };
   }; // class mod_exordium
}; // namespace {anonymous}


// this is called when the module is initially loaded
KINE_MODULE_INIT { return new mod_exordium(); };
