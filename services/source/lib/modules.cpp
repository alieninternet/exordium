/* $Id$
 *
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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

#include <sstream>
#include <algorithm>
#include <iomanip>
#include "modules.h"

using namespace Exordium;
using AISutil::String;
using AISutil::StringTokens;

/* ~Modules - Shut down all existing modules in the list
 * Original 17/09/2002 pickle
 */
Modules::~Modules(void)
{
   // Wipe the modules out, bye bye!
   while (!modules.empty())
     {
	delete (*(modules.begin())).second;
	modules.erase(modules.begin());
     }
}

/* ~ModuleDescriptor - Delete the server and unload the module
 * Original 17/09/2002 pickle
 */
Modules::ModuleDescriptor::~ModuleDescriptor(void)
{
   lt_dlclose(handle);
   //   delete module; // this needs fixing :( is this being deleted before here?
};

/* loadModule - Shut down all existing modules in the list
 * Original 17/09/2002 pickle
 * 17/09/2002 pickle - Added checking to confirm the module is not already open
 * 18/09/2002 pickle - Merged with Services::loadModule()
 * Note: Some of the variables passed here are temporary...
 */
Module* const Modules::loadModule(const String& fileName,
				  String& errString)
{
   // Try to load the module
   lt_dlhandle handle; 

   // Check if that loaded okay
   if ((handle = lt_dlopen(fileName.c_str())) == NULL) {
      // Set the error string appropriately
      errString = "Could not load " + fileName + ": " + lt_dlerror();
      return 0;
   }

   // Locate the initialisation function
   EXORDIUM_MODULE_INIT_FUNCTION_NO_EXTERN((* const initfunc)) =
     ((EXORDIUM_MODULE_INIT_FUNCTION_NO_EXTERN((*)))
      (lt_dlsym(handle, "exordium_module_init")));

   // Check if we could find the init function
   if (initfunc == 0) {
      errString = "Could not load " + fileName +
	": Module does not contain an initialisation function";
      return 0;
   }

   // Pull out the module data, this class contains all the other info we need
   Module* const module = (*initfunc)();
   
   // Make sure the module was returned appropriately...
   if (module == 0) {
      errString = "Could not load " + fileName + 
	": Module failed to initialise";
      return 0;
   }

#ifdef DEBUG
   std::cout << "Loaded module '" <<
     module->getModuleInfo().fullName << "' version " <<
     module->getModuleInfo().versionMajor << '.' <<
     module->getModuleInfo().versionMinor <<
     std::endl;
#endif

   // Make sure this does not exist..
   if (!exists(module->getModuleInfo().shortName)) {
      // Add it, and return happy status
      modules[module->getModuleInfo().shortName] = 
	new ModuleDescriptor(module, handle);
      return module;
   }

   // Umm, we should delete and unload module here!
   //
   // If the module exists already, we shouldn't load it..
   return 0;
}

/* unloadModule - Remove a module from the list, and unload it
 * Original 07/06/2002 pickle
 */
void Modules::unloadModule(const char* const name,
			   const String* const reason)
{
   // Locate the module..
   modules_type::iterator moduleLocation = modules.find(name);

   // If the module exists then stop it, delete it, and erase it - bye bye!
   if (moduleLocation != modules.end()) {
      (*moduleLocation).second->module->stop(reason);
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
     { (void)modulesData.second->module->start(services); };
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
void Modules::unloadAll(const String* const reason)
{
   while (!modules.empty()) {
      (*modules.begin()).second->module->stop(reason);
      delete (*modules.begin()).second;
      modules.erase(modules.begin());
      return;
   }
}

/* exists - Check if a module exists
 * Original 07/06/2002 james
 */
bool Modules::exists(const char* const name) const
{
   // Locate the module..
   modules_type::const_iterator moduleLocation = modules.find(name);

   // If the module exists, be happy
   if (moduleLocation != modules.end()) {
      return true;
   }
   
   // Could not find it..
   return false;
}


/* handleClientSignon - Handle the signing on of a new client, and
 * pass the information to any modules that wish to know.
 *
 */
void
  Modules::handleClientSignon(User& origin)
{
   for (modules_type::const_iterator it = modules.begin();
	it != modules.end(); it++) {
      
      String tmp =  (*it).first;
      if((*it).second->module->getService()->getEventsMask() &
	 Exordium::Service::Events::CLIENT_SIGNON) {
	 
	 /* Ok this module wants to know about signons */
#ifdef DEBUG
	 std::cout << tmp << " would like to know when someone signs on" << std::endl;
#endif
	 (*it).second->module->getService()->handleClientSignon(origin);
      }
      
   }
   
}

/* handleAway - Handle an AWAY message from the parser and throw it
 * to the appropiate modules who are listening for this event type.
 *
 */

void Modules::handleAway(User& origin, const AISutil::String &message) {
   
   for (modules_type::const_iterator it = modules.begin();
	it != modules.end(); it++) {
      String tmp =  (*it).first;
      if((*it).second->module->getService()->getEventsMask() &
	 Exordium::Service::Events::CLIENT_AWAY) {
	 /* Ok this module wants to know about aways */
#ifdef DEBUG
	 std::cout << tmp << " would like to know when someone goes away" << std::endl;
#endif
	 (*it).second->module->getService()->handleAway(origin,message);
      }
      
   }
   
}


/* handleTopic - Handle a TOPIC message from the parser and throw it
 * to the appropiate modules who are listening for this event type.
 *
 */
void Modules::handleTopic(const AISutil::String &origin, dChan& channel, const AISutil::String &newTopic) {

   for (modules_type::const_iterator it = modules.begin();
        it != modules.end(); it++) {
      String tmp =  (*it).first;
      if((*it).second->module->getService()->getEventsMask() &
         Exordium::Service::Events::CHANNEL_TOPIC) {
         /* Ok this module wants to know about TOPIC */
#ifdef DEBUG
         std::cout << tmp << " would like to know when a TOPIC is received" << std::endl;
#endif
         (*it).second->module->getService()->handleTopic(origin,channel, newTopic);
      }

   }

}


void Modules::handleChannelJoin(User& origin, dChan &channel, const int& status)
{

   for (modules_type::const_iterator it = modules.begin();
        it != modules.end(); it++) {
      String tmp =  (*it).first;
      if((*it).second->module->getService()->getEventsMask() &
         Exordium::Service::Events::CHANNEL_JOIN) {
         /* Ok this module wants to know about channel joins */
#ifdef DEBUG
         std::cout << tmp << " would like to know when someone joins a channel" << std::endl;
#endif
         (*it).second->module->getService()->handleChannelJoin(origin,channel,status);
      }

   }

}



void Modules::handleChannelPart(User& origin, dChan &channel) {

   for (modules_type::const_iterator it = modules.begin();
        it != modules.end(); it++) {
      String tmp =  (*it).first;
      if((*it).second->module->getService()->getEventsMask() &
         Exordium::Service::Events::CHANNEL_PART) {
         /* Ok this module wants to know about channel parts */
#ifdef DEBUG
         std::cout << tmp << " would like to know when someone leaves a channel" << std::endl;
#endif
         (*it).second->module->getService()->handleChannelPart(origin,channel);
      }

   }

}



void Modules::handleChannelMode(dChan &channel, const AISutil::String &modes, const AISutil::String &target,
                                const AISutil::String& source) {

   for (modules_type::const_iterator it = modules.begin();
        it != modules.end(); it++) {
      String tmp =  (*it).first;
      if((*it).second->module->getService()->getEventsMask() &
         Exordium::Service::Events::CHANNEL_MODE) {
         /* Ok this module wants to know about channel modes */
#ifdef DEBUG
         std::cout << tmp << " would like to know when a mode command is received for a channel" << std::endl;
#endif
         (*it).second->module->getService()->handleChannelMode(channel,modes,target,source);
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
#ifdef DEBUG
   std::cout << "Modules loaded: ";
#endif
   std::ostringstream tmp;

   // Iterate through the module list
   for (modules_type::const_iterator it = modules.begin();
	it != modules.end(); it++) {
      tmp << (*it).first << ' ';
   }
   
#ifdef DEBUG
   // output it to stdout.. temporary??
   std::cout << tmp.str() << std::endl;
#endif
   
   // Return the list
   return tmp.str();
}
