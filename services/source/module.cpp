/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include <iostream>
#include <exordium/services.h>
#include <exordium/log.h>
#include <exordium/sql.h>
#include <kineircd/module-service.h>


using namespace Exordium;

namespace mod_exordium {

   // Ourself! We will want to delete this when stopping
   static Services *services;
   
   // called just before the module is actually going to be used
   static KINE_MODULE_START(moduleStart)
     {
	cout << "mod_exordium::moduleStart()" << endl;

	// My, it looks an awful lot like main.cpp from here on... :) This is
	// temporary, naturally.
        Sql db;
	
	// Create the new services instance
	services = new Services(daemon);
	
	Log::init();
	Log::logLine("Services started, beginning initalisation");
	me.load_config();
	db.init(me.mysqlHost,me.mysqlUser,me.mysqlPass,me.mysqlDb);
	me.init();
	me.run();
	Log::logLine("Services terminated - Normal exit");
	exit(0); // we are naughty using this here..
	
	// Tell Kine that we started happily
	return true;
     }

   
   // called just before unloading the module
   static KINE_MODULE_STOP(moduleStop)
     {
	cout << "mod_exordium::moduleStop()" << endl;
	delete services;
     }
   
   
   // information about ourselves
   static const Kine::Module::basicInfo_type moduleInfo = {
      // List our version/copyright information (fill this in James)
      "exordium", // short name of the module, usually one word
      "Exordium IRC Network Services", // long name of the module
      "Copyright (c) 2002 IRCDome Development Team", // Copyright information
      0, // Major version number
      1, // Minor version number
      2, // Patch-level (may be set to 0 if none)
      ".beta", // extra information (may be set to null or 0 if none)

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
      Kine::Module::basicInfo_type::FLAG_UNIQUE_INSTANCE |
	Kine::Module::basicInfo_type::FLAG_CONFIGURE_MANDATORY,

      // Configuration information (optional, but we will use it later)
      0,
	
      // Our start and stop functions
      &moduleStart,
      &moduleStop
   };
}; // namespace mod_exordium


// called when the module is initially loaded
KINE_MODULE_INIT
{
   return new Kine::ModuleService(mod_exordium::moduleInfo);
}
