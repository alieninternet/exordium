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

#ifndef _MODULELIST_H_
# define _MODULELIST_H_ 1

# include <map>
# include <kineircd/str.h>

extern "C" {
# include <dlfcn.h>
};

namespace Exordium {
   class Services;
   class User;
   class Service;
   
   class Modules {
    private:
      // Handy class for module information
      class ServiceModule {
       public:
	 Service* const service;			// Service class
	 void* const handle;				// dlopen() handle
	 
	 ServiceModule(Service* const s, void* const h)
	   : service(s), handle(h)
	     {};
	 
	 ~ServiceModule(void) 
	   { dlclose(handle); };
      };

      // The list of modules
      typedef std::map <LibAIS::String, ServiceModule *> modules_type;
      modules_type modules;

    public:
      // Constructor
      Modules(void) 
	{};
      
      // Destructor (unload all modules)
      ~Modules(void);
      
      // Add a module to the list
      bool addModule(Service& s, void* const h);
      
      // Remove a module from the list, and unload it
      void delModule(const LibAIS::String&name);

      // Check if a module exists
      bool exists(const LibAIS::String& name) const;
      
      // Throw a line at the appropriate service (sent directly)
      void throwLine(const LibAIS::String& name, LibAIS::StringTokens& line, 
		     User &origin);
      
      // Throw a line at the appropriate service (sent to a channel)
      void throwLine(const LibAIS::String& name, LibAIS::StringTokens& line,
		     User& origin, const LibAIS::String& channel);
      
      // Dump a list of modules
      LibAIS::String dumpModules(void) const;
   };
}; // namespace Exordium

# include <exordium/service.h>
# include <exordium/user.h>
# include <exordium/service.h>

#endif // _MODULELIST_H_

