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

#include "oper.h"
#include <exordium/service.h>
#include <exordium/services.h>
#include <kineircd/str.h>


using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::OperModule;


const Module::functionTableStruct Module::functionTable[] =
{
     { "help",		&Module::parseHELP },
     { "jupe",		&Module::parseJUPE },
     { 0, 0 }
};

void
  Module::parseLine (StringTokens& line, User& origin, String const &ch)
{
   return;
}

void
  Module::parseLine (StringTokens& line, User& origin)
{
   StringTokens& st = line;
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
   origin.sendMessage ("Unrecognized Command", getName());
}


OPER_FUNC(Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getName(),word,parm);
}


OPER_FUNC(Module::parseJUPE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 jupe add/list/del",getName());
	return;
     }
   if(command=="add")
     {
	String nickname = tokens.nextToken();
	String reason = tokens.rest();
	if(nickname=="" | reason=="")
	  {
	     origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: add nickname reason for jupe",getName());
	     return;
	  }
     }
}


EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }


// Module information structure
const Module::moduleInfo_type Module::moduleInfo = {
   "Operator Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::NONE
};


// Start the service
bool Module::start(void)
{
   // Register ourself to the network
   servicesFwd.registerService(getName(), getName(),
			     getConfigData().getHostname(),
			     getConfigData().getDescription());
   
   // We started okay :)
   return true;
}
