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

#ifndef _INCLUDE_EXORDIUM_MODULES_H_
# define _INCLUDE_EXORDIUM_MODULES_H_ 1

# include <map>
# include <aisutil/string.h>
# include <kineircd/clientname.h>

extern "C" {
# include <ltdl.h>
};

namespace Exordium {
   class Services;
   class User;
   class Module;
   class dChan;
   
   class Modules {
    public:
      // Handy class for module information
      struct ModuleDescriptor {
	 Module* const module;				// Module class
	 lt_dlhandle handle;				// lt_dlopen() handle
	 
	 ModuleDescriptor(Module* const m, lt_dlhandle& h)
	   : module(m), handle(h)
	     {};
	 
	 ~ModuleDescriptor(void);
      };

      typedef std::map <const char* const, ModuleDescriptor *> modules_type;
      
    private:
      // The list of modules
      modules_type modules;

    public:
      // Constructor
      Modules(void) 
	{};
      
      // Destructor (unload all modules)
      ~Modules(void);
      /* Event handler(s) */
      
      void handleAway(User&, const AISutil::String &);
      void handleClientSignon(User&);
      void handleTopic(const AISutil::String &origin, dChan& channel, const AISutil::String& newTopic);
      void handleChannelJoin(User& origin, dChan& channel, const int& status);
      void handleChannelPart(User& origin, dChan& channel);
      void handleChannelMode(dChan& channel, const AISutil::String& modes, const AISutil::String& target,
                             const AISutil::String& source);



      // Add a module to the list
      Module* const loadModule(const AISutil::String& fileName, 
			       AISutil::String& errString);
      
      // Remove a module from the list, and unload it
      void unloadModule(const char* const name,
			const AISutil::String* const reason = 0);

      // Start all modules in the list
      void startAll(Exordium::Services& services);
      
      // Stop and unload all the modules in the list
      void unloadAll(const AISutil::String* const reason = 0);
      
      // Check if a module exists
      bool exists(const char* const name) const;
      
      // Dump a list of modules
      AISutil::String dumpModules(void) const;
   };
}; // namespace Exordium

# include <exordium/module.h>
# include <exordium/user.h>

#endif // _INCLUDE_EXORDIUM_MODULES_H_

