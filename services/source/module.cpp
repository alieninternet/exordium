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

#include <iostream>
#include <exordium/log.h>
#include <exordium/conf.h>
#include <exordium/services.h>
#include <kineircd/module.h>
#include <exordium/database/database.h>

using namespace Exordium;

// This namespace is intentionally anonymous
namespace {
   // Our special little classes, we need these so we can delete them later
   static Config *config = 0;
   static Log *logger = 0;
   static Services *services = 0;
   static CDatabase *db = 0;

   // called just before the module is actually going to be used
   static KINE_MODULE_START(moduleStart)
     {
	// Dump some copyright info.
	std::cout << std::endl << "Exordium Network Services, Copyright
				    (C) 2002 IRCDome Development Team" << std::endl;
	std::cout << "Exordium comes with ABSOLUTELY NO WARRANTY; for details see" << std::endl;
	std::cout << "The enclosed LICENSE file.  This is free software"<<std::endl;
	std::cout << "And you are welcome to redistribute it under certain" << std::endl;
	std::cout << "conditions; please see the enclosed LICENSE file" << std::endl;
	std::cout << "mod_exordium::moduleStart()" << std::endl;

	// Check the configuration was completely setup, otherwise get cranky
/* I will uncomment this shortly
	if (!config.checkConfig()) {
	   // Tell KineIRCd that we refuse to start
	   return false;
	}
 */
	
	// New Logger (the config file will have been created by now)
	logger = new Log(*config);

	// Create new database Instance
        db = new CDatabase(*config, *logger); 

	// Create the new services instance - Passing sql + logger YAY :|
	services = new Services(daemon, *logger, *config, *db);

	logger->logLine("Services started, beginning initalisation");
	services->run();
	logger->logLine("Services terminated - Normal exit");
	exit(0); // we are naughty using this here..... very naughty.. :(

	// Tell Kine that we started happily
	return true;
     }

   // called just before unloading the module
   static KINE_MODULE_STOP(moduleStop)
     {
	std::cout << "mod_exordium::moduleStop()" << std::endl;
	delete services;
	delete config;
        delete db;
     }

   // information about ourselves
   static const Kine::Module::basicInfo_type moduleInfo =
     {
	// List our version/copyright information (fill this in James)
	"exordium", // short name of the module, usually one word
	  "Exordium IRC Network Services", // long name of the module
	  "Copyright (c) 2002 IRCDome Development Team", // Copyright information
	  0, // Major version number
	  0, // Minor version number
	  0, // Patch-level (may be set to 0 if none)
	  ".pre-alpha", // extra information (may be set to null or 0 if none)

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
	  Kine::Module::basicInfo_type::Flags::UNIQUE_INSTANCE,

	  // Configuration information
	  &Config::definitionTable,

	  // Our start and stop functions
	  &moduleStart,
	  &moduleStop
     };
}; // namespace {anonymous}

// called when the module is initially loaded
KINE_MODULE_INIT
{
   // Make a new config class, where our configuration data will be stored
   config = new Config();

   // Make a new module for Kine
   return new Kine::Module(moduleInfo, config);
}
