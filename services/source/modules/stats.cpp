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

#include "include/stats.h"
#include "exordium/services.h"
#include "exordium/nickname.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include "exordium/sql.h"
#include "exordium/module.h"
#include <sys/time.h>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

namespace Exordium
{

   struct Stats::functionTableStruct const
     Stats::functionTable[] =
     {
	  {"help", &Stats::parseHELP},
	  {0, 0}
     };
   void
     Stats::parseLine (String const &line, String const &requestor, String const &ch)
       {
	  return;
       }

   void
     Stats::parseLine (String const &line, String const &requestor)
       {
	  StringTokens st (line);
	  String origin = requestor;
	  String command = st.nextToken ().toLower ();
	  for (int i = 0; functionTable[i].command != 0; i++)
	    {
	       // Does this match?
	       if (command == functionTable[i].command)
		 {
		    (this->*(functionTable[i].function))(origin, st);
		    return;
		 }
	    }
	  services.serviceNotice ("Unrecognized Command", "Stats", requestor);
       }
   
   void
     STATS_FUNC (Stats::parseHELP)
       {
	  String word = tokens.nextToken();
	  String parm = tokens.nextToken();
	  services.doHelp(origin,"Stats",word,parm);
	  String tolog = "Did HELP on word " + word + " parm " + parm;
	  services.log(origin,"Stats",String(tolog));
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	services.registerService(name,name,"ircdome.org","+z",
				 "Statistics Collection Service");
	services.serviceJoin(name,"#Debug");
	return new Module("stats", new Stats(services));
     }

};

