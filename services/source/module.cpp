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
	// Check the configuration was completely setup, otherwise get cranky
/* Uncomment this when the database engine config thing is complete       
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
	delete services;
	delete config;
        delete db;
     }

   /* This is a zero-terminated array of lines to be appended to the list for
    * /INFO. Lines must be 60 visible characters long or shorter. Do not use
    * control characters other than those used in formatting IRC text. The
    * character set for data here should be UTF-8. A ruler is provided to
    * aid you keeping the data within 60 chars..
    * 
    * Note to developers: This should probably be placed in another file and
    * passed through AutoGen.. It'll probably want a logo made up too..
    */
   static const Kine::Module::versionInfo_type versionInfo = 
     {/* Ruler:
       *          1         2         3         4         5         6
       * 123456789012345678901234567890123456789012345678901234567890 */
	"Exordium Network Services, Copyright (C) 2002 IRCDome ",
	"Development Team",
	"",
	"Exordium comes with ABSOLUTELY NO WARRANTY; for details see",
	"The enclosed LICENSE file.  This is free software",
	"And you are welcome to redistribute it under certain",
	"conditions; please see the enclosed LICENSE file",
	0
     };
   
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
	  (Kine::Module::versionInfo_type*)&versionInfo,

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
