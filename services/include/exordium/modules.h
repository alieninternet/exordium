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

# include <kineircd/str.h>
# include <map>
# include <cassert>

extern "C" {
# include <dlfcn.h>
};

# include <exordium/service.h>

namespace Exordium {
   class Modules {
    private:
      struct ServiceModule {
	 Service *service;
	 void *handle;
	 
	 ServiceModule(Service *s, void *h)
	   : service(s), handle(h)
	     {};
	 ~ServiceModule(void)
	   {
	      std::cout << "Dead module??" << std::endl;
	      dlclose(handle);
	   }
      };
      typedef std::map <LibAIS::String, ServiceModule *> modules_type;
      modules_type serviceModules;
      
    public:
      Modules(void) {
	 // I'm anal, so shoot me.
	 serviceModules.clear();
      };
      
      void addModule(Service &s, void *h) {
	 // Just add it - this will overwrite anything already there
	 serviceModules[s.getName().IRCtoLower()] = new ServiceModule(&s,h);
      };
      
      void delModule(LibAIS::String const &name) {
	 ServiceModule *sm = serviceModules[name.IRCtoLower()];
	 if (sm != 0) {
	    delete sm;
	    serviceModules.erase(name.IRCtoLower());
	    return;
	 }
	 std::cout << "Umm... i couldn't find " << name << std::endl;
      };
      
      bool exists(LibAIS::String const &name)
	{
	   ServiceModule *sm = serviceModules[name.IRCtoLower()];
	   if ( sm == 0 )
	     {
		serviceModules.erase(name.IRCtoLower());
		return false;
	     }
	   return true;
	}
      
      // Throw a line at the appropriate service
      void throwLine(LibAIS::String const &name, LibAIS::StringTokens& line, 
		     User &origin) {
	 ServiceModule *sm = serviceModules[name.IRCtoLower()];
	 if (sm == 0) {
	    // Give up.. delete what we just made and go bye byes
	    serviceModules.erase(name.IRCtoLower());
	    return;
	 }
	 Service *serv = sm->service;
	 serv->parseLine(line, origin);
	 return;
      };
      
      void throwLine(LibAIS::String const &name, LibAIS::StringTokens& line,
		     User& origin, LibAIS::String const &ch) {
	 // Find it...
	 ServiceModule *sm = serviceModules[name.IRCtoLower()];
	 
	 // It will be null if we did not find it..
	 if (sm == 0) {
	    // Give up.. delete what we just made and go bye byes
	    serviceModules.erase(name.IRCtoLower());
	    return;
	 }
	 Service *serv = sm->service;
	 // Otherwise we must have it.. send something to the service
	 serv->parseLine(line, origin, ch);
	 return;
      };
      
      // Dump a list of modules
      LibAIS::String dumpModules(void) {
	 std::cout << "Modules loaded: ";
	 LibAIS::String tmp = "";
	 for (modules_type::iterator it = serviceModules.begin();
	      it != serviceModules.end(); it++) {
	    // Output the key..
	    std::cout << (*it).first << ' ';
	    tmp = LibAIS::String(tmp)+" "+(*it).first;
	 }
	 std::cout << std::endl;
	 return tmp;
      }
   };
}; // namespace Exordium

#endif // _MODULELIST_H_

