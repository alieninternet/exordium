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

#include <sstream>
#include <algorithm>
#include <iomanip>
#include "exordium/modules.h"

using namespace Exordium;
using AISutil::String;
using AISutil::StringTokens;


/* ~Modules - Shut down all existing modules in the list
 * Original 17/09/2002 pickle
 */
Modules::~Modules(void)
{
   // Wipe the modules out, bye bye!
   while (!modules.empty()) {
      delete (*(modules.begin())).second;
      modules.erase(modules.begin());
   }
}


/* ~Module - Delete the server and unload the module
 * Original 17/09/2002 pickle
 */
Modules::Module::~Module(void) 
{ 
   dlclose(handle);
//   delete service; // this needs fixing :( is this being deleted before here?
};


/* loadModule - Shut down all existing modules in the list
 * Original 17/09/2002 pickle
 * 17/09/2002 pickle - Added checking to confirm the module is not already open
 * 18/09/2002 pickle - Merged with Services::loadModule()
 * Note: Some of the variables passed here are temporary...
 */
Service* const Modules::loadModule(const String& fileName, 
				   String& errString)
{
   // Try to load the module
   void* const handle = dlopen(fileName.c_str(), RTLD_NOW);
   
   // Check if that loaded okay
   if (handle == 0) {
      // Set the error string appropriately
      errString = "Could not load " + fileName + ": " + dlerror();
      return 0;
   }
   
   // Locate the initialisation function
   EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN((* const initfunc)) =
     ((EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN((*)))
      (dlsym(handle, "service_init")));
   
   // Check if we could find the init function
   if (initfunc == 0) {
      errString = "Could not load " + fileName + 
	": Module does not contain an initialisation function";
      return 0;
   }

   // Pull out the service data, this class contains all the other info we need
   Service* const service = (*initfunc)();
   
   // Make sure the service was returned appropriately...
   if (service == 0) {
      errString = "Could not load " + fileName +
	": Module failed to initialise";
      return 0;
   }
	  
   std::cout << "Loaded module '" << 
     service->getModuleInfo().fullName << "' version " <<
     service->getModuleInfo().versionMajor << '.' <<
     service->getModuleInfo().versionMinor << 
     std::endl;

   // Fix up the name, since we use it twice (may as well convert it once)
   String moduleName = service->getName().IRCtoLower();
   
   // Make sure this does not exist..
   if (!exists(moduleName)) {
      // Add it, and return happy status
      modules[moduleName] = new Module(service, handle);
      return service;
   }

   // Umm, we should delete and unload module here!
   
   // If the module exists already, we shouldn't load it..
   return 0;
}


/* unloadModule - Remove a module from the list, and unload it
 * Original 07/06/2002 pickle
 */
void Modules::unloadModule(const String& name, const String& reason) {
   // Locate the module..
   modules_type::iterator moduleLocation = modules.find(name.IRCtoLower());
   
   // If the module exists then stop it, delete it, and erase it - bye bye!
   if (moduleLocation != modules.end()) {
      (*moduleLocation).second->service->stop(reason);
      delete (*moduleLocation).second;
      modules.erase(moduleLocation);
      return;
   }
   
   // output to stdout.. temporary??
   #ifdef DEBUG
   std::cout << "Umm... i couldn't find " << name << std::endl;
   #endif
}


// Helper for the 'startAll' function below
struct startModule {
   // Where services is
   Services& services;
   
   // Constructor
   startModule(Services& s)
     : services(s)
     {};

   // Operator which performs the starting
   inline void operator()(Modules::modules_type::value_type& modulesData)
     { modulesData.second->service->start(services); };
};


/* startAll - Start all modules in the list
 * Original 20/09/2002 pickle
 */
void Modules::startAll(Services& services)
{
   (void)std::for_each(modules.begin(), modules.end(), startModule(services));
}


/* unloadAll - Stop all the modules in the list
 * Original 21/09/2002 pickle
 */
void Modules::unloadAll(const String& reason)
{
   while (!modules.empty()) {
      (*modules.begin()).second->service->stop(reason);
      delete (*modules.begin()).second;
      modules.erase(modules.begin());
      return;
   }
}

      
/* exists - Check if a module exists
 * Original 07/06/2002 james
 */
bool Modules::exists(const String& name) const
{
   // Locate the module..
   modules_type::const_iterator moduleLocation =
     modules.find(name.IRCtoLower());
   
   // If the module exists, be happy
   if (moduleLocation != modules.end()) {
      return true;
   }
   
   // Could not find it..
   return false;
}


/* throwLine - Throw a line at the appropriate service (sent directly)
 * Original 07/06/2002 pickle
 */
void Modules::throwLine(const String& name, StringTokens& line, User& origin) 
{
   // Locate the module..
   modules_type::iterator moduleLocation = modules.find(name.IRCtoLower());
   
   // If the module exists, throw the line at it
   if (moduleLocation != modules.end()) {
      (*moduleLocation).second->service->parseLine(line, origin);
   }
}


/* throwLine - Throw a line at the appropriate service (sent to a channel)
 * Original 07/06/2002 pickle
 */
void Modules::throwLine(const String& name, StringTokens& line, User& origin,
			const String& channel) 
{
   // Locate the module..
   modules_type::iterator moduleLocation = modules.find(name.IRCtoLower());
   
   // If the module exists, throw the line at it
   if (moduleLocation != modules.end()) {
      (*moduleLocation).second->service->parseLine(line, origin, channel);
   }
}

/* handleAway - Handle an AWAY message from the parser and throw it 
 * to the appropiate modules who are listening for this event type.
 * 
 */

void
  Modules::handleAway(User& origin, const AISutil::String &message)
{
    for (modules_type::const_iterator it = modules.begin();
	it != modules.end(); it++)
     {
	String tmp =  (*it).first;
	if((*it).second->service->getModuleInfo().eventsMask & 
	   Exordium::Service::moduleInfo_type::Events::CLIENT_AWAY)
	  {
	     /* Ok this module wants to know about aways */
	     std::cout << tmp << " would like to know when someone goes away" << std::endl;
	     (*it).second->service->handleAway(origin,message);
	  }
	
     }
   
   
}


/* dumpModules - Dump a list of modules
 * Original 07/06/2002 pickle
 * 09/06/2002 james - Added the ability to return the line as a string
 * 17/09/2002 pickle - Went const mad, and stopped all that horrible copying
 */
String Modules::dumpModules(void) const
{
   std::cout << "Modules loaded: ";
   std::ostringstream tmp;
   
   // Iterate through the module list
   for (modules_type::const_iterator it = modules.begin(); 
	it != modules.end(); it++) {
      tmp << (*it).first << ' ';
   }

   // output it to stdout.. temporary??
   std::cout << tmp.str() << std::endl;

   // Return the list
   return tmp.str();
}
