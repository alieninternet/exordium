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

#include "include/oper.h"

#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include <map>
#include "exordium/sql.h"

using LibAIS::String;
using LibAIS::StringTokens;
using namespace Exordium;

struct Oper::functionTableStruct const
  Oper::functionTable[] =
{
     {"help", &Oper::parseHELP},
     {"jupe", &Oper::parseJUPE},
     {0, 0}
};

void
  Oper::parseLine (String const &line, String const &requestor, String const &chan)
{
   StringTokens st (line);
   String origin = requestor;
   String command = st.nextToken ().toLower ();
   String ch = chan;
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st, ch);
	     return;
	  }
     }

   return;
}

void
  Oper::parseLine (String const &line, String const &requestor)
{
   StringTokens st (line);
   String origin = requestor;
   String command = st.nextToken ().toLower ();
   String ch = "";
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st, ch);
	     return;
	  }
     }
   sendMessage (requestor,"Unrecognized Command");
}

void
  OPER_FUNC (Oper::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services.doHelp(origin,"oper",word,parm);
}

void
  OPER_FUNC (Oper::parseJUPE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	services.serviceNotice("\002[\002Incorrect Usage\002]\002 jupe add/list/del","Oper",origin);
	return;
     }
   if(command=="add")
     {
	String nickname = tokens.nextToken();
	String reason = tokens.rest();
	if(nickname=="" | reason=="")
	  {
	     services.serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: add nickname reason for jupe","oper",origin);
	     return;
	  }
     }
}
EXORDIUM_SERVICE_INIT_FUNCTION
{
   return new Oper(services, name);
}

// Module information structure
const Oper::moduleInfo_type Oper::moduleInfo = {
   "Operator Service",
     0, 0
};


// Start the service
void Oper::start(void)
{
   services.registerService(getName(),getName(),"ircdome.org", "+dz",
			    "IRC Operator Services");
   services.serviceJoin(getName(),"#Debug");
}
