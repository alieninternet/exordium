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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <kineircd/str.h>
#include <kineircd/utils.h>
#include "console.h"

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;

const struct Console::functionTableStruct Console::functionTable[] = {
     { "module",		&Console::parseMODULE },
     { 0, 0 }
};


void Console::parseLine(const String &line, const String &requestor) 
{
   StringTokens st (line);
   String origin = requestor;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Does this match?
      if (command == functionTable[i].command) {
	 // Run the command and leave
	 (this->*(functionTable[i].function))(origin, st);
	 return;
      }
   }
   
   services.serviceNotice("Unrecognized Command", 
			  services.getConfigInternal().getConsoleName(),
			  requestor);
}


void CONSOLE_FUNC(Console::parseMODULE)
{
   String command = tokens.nextToken();
   
   if(command == "list") {
      String foo = services.getConfigInternal().getModules().dumpModules();
      services.serviceNotice("The currently loaded service modules are:",
			     services.getConfigInternal().getConsoleName(),
			     origin);
      services.serviceNotice(foo,
			     services.getConfigInternal().getConsoleName(),
			     origin);
      return;
   }
   
   if (command == "unload") {
      String name = tokens.nextToken();
      services.serviceNotice("Unloading module " + name,
			     services.getConfigInternal().getConsoleName(),
			     origin);
      services.getConfigInternal().getModules().unloadModule(name,
							     origin +
							     " unloaded me :(");
      return;
   }
   
   if(command == "load") {
//    String name = tokens.nextToken();
      (void)tokens.nextToken();
      String filename = tokens.nextToken();
      services.serviceNotice("Attempting to load module",
			     services.getConfigInternal().getConsoleName(),
			     origin);
      String errString;
      Service* const service = 
	services.getConfigInternal().getModules().loadModule(filename,
							     errString);
      
      if (service == 0) {
	 services.serviceNotice("Error loading module: " + errString,
				services.getConfigInternal().getConsoleName(),
				origin);
	 return;
      }
	
      // Start the module, now that it has been loaded..
      service->start(services);
      
      services.serviceNotice("Module loaded successfully",
			     services.getConfigInternal().getConsoleName(),
			     origin);
   }
}
