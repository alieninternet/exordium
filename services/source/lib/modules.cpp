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

#include "exordium/modules.h"

using namespace Exordium;
using LibAIS::String;
using LibAIS::StringTokens;


/* ~Modules - Shut down all existing modules in the list
 * Original 17/09/2002 simonb
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
 * Original 17/09/2002 simonb
 */
Modules::Module::~Module(void) 
{ 
   dlclose(handle);
//   delete service; // this needs fixing :( is this being deleted before here?
};


/* addModule - Shut down all existing modules in the list
 * Original 17/09/2002 simonb
 * 17/09/2002 simonb - Added checking to confirm the module is not already open
 */
bool Modules::addModule(Service& s, void* const h) 
{
   // Fix up the name, since we use it twice (may as well convert it once)
   String name = s.getName().IRCtoLower();
   
   // Make sure this does not exist..
   if (!exists(name)) {
      // Add it, and return happy status
      modules[name] = new Module(&s, h);
      return true;
   };

   // If the module exists already, we shouldn't load it..
   return false;
}


/* delModule - Remove a module from the list, and unload it
 * Original 07/06/2002 simonb
 */
void Modules::delModule(const String& name) {
   // Locate the module..
   modules_type::iterator moduleLocation = modules.find(name.IRCtoLower());
   
   // If the module exists then stop it, delete it, and erase it - bye bye!
   if (moduleLocation != modules.end()) {
      (*moduleLocation).second->service->stop();
      delete (*moduleLocation).second;
      modules.erase(moduleLocation);
      return;
   }
   
   // output to stdout.. temporary??
   std::cout << "Umm... i couldn't find " << name << std::endl;
}


/* exists - Check if a module exists
 * Original 07/06/2002 james
 */
bool Modules::exists(const LibAIS::String& name) const
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
 * Original 07/06/2002 simonb
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
 * Original 07/06/2002 simonb
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


/* dumpModules - Dump a list of modules
 * Original 07/06/2002 simonb
 * 09/06/2002 james - Added the ability to return the line as a string
 * 17/09/2002 simonb - Went const mad, and stopped all that horrible copying
 */
LibAIS::String Modules::dumpModules(void) const 
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
