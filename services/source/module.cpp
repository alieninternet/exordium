/*
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include <iostream>
#include <exordium/log.h>
#include <exordium/sql.h>
#include <exordium/conf.h>
#include <exordium/services.h>
#include <kineircd/module.h>

using namespace Exordium;

namespace
{
   // Our special little classes, we need these so we can delete them later
   static Config *config = 0;
   static Log *logger = 0;
   static Services *services = 0;
   static Sql *db = 0;

   // called just before the module is actually going to be used
   static KINE_MODULE_START(moduleStart)
     {
	std::cout << "mod_exordium::moduleStart()" << std::endl;

	// New Logger (the config file will have been created by now)
	logger = new Log(*config);

	// Create new SQL Instance
	db = new Sql(*logger, *config);

	// Create the new services instance - Passing sql + logger YAY :|
	services = new Services(daemon, *logger, *db, *config);

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

	  // Flags.. ignore this for now
	  Kine::Module::basicInfo_type::FLAG_UNIQUE_INSTANCE,

	  // Configuration information (optional, but we will use it later)
	  &Config::definitionTable,

	  // Our start and stop functions
	  &moduleStart,
	  &moduleStop
     };
}; // namespace

// called when the module is initially loaded
KINE_MODULE_INIT
{
   // Make a new config class, where our configuration data will be stored
   config = new Config();

   // Make a new module for Kine
   return new Kine::Module(moduleInfo, config);
}
