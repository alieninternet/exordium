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

#include "help.h"
#include <exordium/service.h>
#include <exordium/services.h>


using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::HelpModule;


const Module::functionTableStruct Module::functionTable[] =
{
     { "help",		&Module::parseHELP },
     { 0, 0 }
};

void Module::parseLine(StringTokens& line, User& origin, const bool safe)
{
   String command = line.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Does this match?
      if (command == functionTable[i].command) {
         (this->*(functionTable[i].function))(origin, line);
         return;
      }
   }
   origin.sendMessage("Unrecognised Command", getNickname());
}

HELP_FUNC(Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getNickname(),word,parm);
}

EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }


// Module information structure
const Module::moduleInfo_type Module::moduleInfo = {
   "Help Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::NONE
};


// Start the service
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;

   // Register ourself to the network
   services->registerService(getNickname(), getUsername(),
			     getHostname(), getDescription());
   
   // We started okay :)
   return true;
}
