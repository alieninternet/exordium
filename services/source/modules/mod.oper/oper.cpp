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
     { "serverjupe",		&Module::parseSERVERJUPE },
     { "join",		&Module::parseJOIN },
     { "part",		&Module::parsePART },
     { "commands",	&Module::parseCOMMANDS },
     { 0, 0 }
};

void Module::parseLine(StringTokens& line, User& origin, const bool safe)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     int required = services->getRequiredAccess(getName(),command.toLower());
	     int access = origin.getAccess(getName());
	     if(required>access)
		{
		origin.sendMessage("You do not have enough access for that command",getName());
		String togo = origin.getNickname()+" tried to use \002"+command+"\002";
		services->sendGOper(getName(),togo);
		return;
		}
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


OPER_FUNC(Module::parseSERVERJUPE)
{
	String serverName = tokens.nextToken();
	String reason = tokens.rest();
	if(serverName=="" | reason=="")
	  {
	     origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: serverjupe serverName reason for jupe",getName());
	     return;
	  }
	services->queueAdd("SERVER "+serverName+" 2 :"+reason);
	String togo = origin.getNickname()+" \002juped\002 the server \002"+
		serverName+"\002 for \002"+reason+"\002";
	services->sendGOper(getName(),togo);
	services->log(origin,getName(),togo);
}

OPER_FUNC(Module::parseJOIN)
{
   String chan = tokens.nextToken().IRCtoLower();
   if(chan=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 join #channel",getName());
	return;
     }
   if(chan[0] != '#')
     {
        origin.sendMessage("Error: Channel names must begin with the '#' symbol",getName());
        return;
     }
   else
     {
       services->serviceJoin(getName(), chan);
       services->serverMode(chan, "+o", getName());
       String togo = "\002"+origin.getNickname()+"\002 made "+getName()+" join \002"+chan+"\002";
       services->sendGOper(getName(),togo);
       services->log(origin,getName(),togo);
     }
}

OPER_FUNC(Module::parsePART)
{
   String chan = tokens.nextToken().IRCtoLower();
   if(chan=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 part #channel",getName());
	return;
     }
   if(chan[0] != '#')
     {
        origin.sendMessage("Error: Channel names must begin with the '#' symbol",getName());
        return;
     }
   else
     {
       services->servicePart(getName(), chan);
       services->sendGOper(getName(),"\002"+origin.getNickname()+"\002 made "+getName()+" part \002"+chan+"\002");
     }
}

  OPER_FUNC (Module::parseCOMMANDS)
{
  // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength = 200;

   // Send the banner (this shouldn't be hard-coded)
   // sendMessage(origin, "Command list for " + getName() + ":");
   origin.sendMessage("Command list for " + getName() + ":",getName());
   // Start formulating the data..
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Add the command to the list
      list << " " << functionTable[i].command;

   // How are we for size?
      if (list.str().length() >= lineLength) {
         // Dump it and reset the string stream thingy
         origin.sendMessage(list.str(),getName());
         list.str() = " -=>";
      }
   }

   // Anything left to send still?
   if (list.str().length() > 4) {
      origin.sendMessage(list.str(),getName());
   }

   // Send the footer (this shouldn't be hard-coded)
   origin.sendMessage("End of command list",getName());
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
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;

   // Register ourself to the network
   services->registerService(getName(), getName(),
			     getConfigData().getHostname(),
			     getConfigData().getDescription());
   
   // We started okay :)
   return true;
}
