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

#include "oper.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include <map>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;

struct Oper::functionTableStruct const
  Oper::functionTable[] =
{
     {"help", &Oper::parseHELP},
     {"jupe", &Oper::parseJUPE},
     {0, 0}
};

void
  Oper::parseLine (StringTokens& line, User& origin, String const &ch)
{
   return;
}

void
  Oper::parseLine (StringTokens& line, User& origin)
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

void
  OPER_FUNC (Oper::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getName(),word,parm);
}

void
  OPER_FUNC (Oper::parseJUPE)
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
{
   return new Oper();
}

// Module information structure
const Oper::moduleInfo_type Oper::moduleInfo = {
   "Operator Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::NONE
};


// Start the service
void Oper::start(Exordium::Services& s)
{
   services = &s;
   services->registerService(getName(),getName(),"ircdome.org", "+dz",
			    "IRC Operator Services");
   services->serviceJoin(getName(),"#Debug");
}
