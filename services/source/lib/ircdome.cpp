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

#include "exordium/services.h"
#include <kineircd/str.h>
#include <kineircd/utils.h>
#include "exordium/ircdome.h"

using LibAIS::String;
using LibAIS::StringTokens;
using namespace Exordium;

const struct IRCDome::functionTableStruct IRCDome::functionTable[] =
{
     { "module", &IRCDome::parseMODULE },
     { 0, 0 }
};

void IRCDome::parseLine(const String &line, const String &requestor)
{
   StringTokens st (line);
   String origin = requestor;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   services.serviceNotice ("Unrecognized Command", 
			   services.getConfig().getConsoleName(), requestor);
}

void
  IRCDOME_FUNC (IRCDome::parseMODULE)
{
   String command = tokens.nextToken();
   if(command=="list")
     {
	String foo = services.getConfig().getModules().dumpModules();
	services.serviceNotice("The currently loaded service modules are:",
			       services.getConfig().getConsoleName(), origin);
	services.serviceNotice(foo, services.getConfig().getConsoleName(),
			       origin);
	return;
     }
   if(command=="unload")
     {
	String name = tokens.nextToken();
	services.serviceNotice("Unloading module " + name,
			       services.getConfig().getConsoleName(), origin);
	services.getConfig().getModules().unloadModule(name,
						       origin +
						       " unloaded me :(");
     }
   if(command=="load")
     {
//	String name = tokens.nextToken();
	(void)tokens.nextToken();
	String filename = tokens.nextToken();
	services.serviceNotice("Attempting to load module",
			       services.getConfig().getConsoleName(), origin);
	String errString;
	Service* const service = 
	  services.getConfig().getModules().loadModule(filename, errString);
	if(service == 0) {
	   services.serviceNotice("Error loading module: " + errString,
				  services.getConfig().getConsoleName(),
				  origin);
	   return;
	}
	
	// Start the module, now that it has been loaded..
	service->start(services);
	
	services.serviceNotice("Module loaded successfully",
			       services.getConfig().getConsoleName(), origin);

     }

}
