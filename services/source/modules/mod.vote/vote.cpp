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

#include "vote.h"
#include <exordium/services.h>
#include <exordium/nickname.h>
#include <exordium/channel.h>
#include <kineircd/str.h>
#include <exordium/sql.h> // <=- is this correct?
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;

   struct Vote::functionTableStruct const
     Vote::functionTable[] =
     {
	  {"help", &Vote::parseHELP},
	  {0, 0}
     };
   void
     Vote::parseLine (String const &line, String const &requestor, String const &ch)
       {
	  return;
       }

   void
     Vote::parseLine (String const &line, String const &requestor)
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
	  services.serviceNotice ("Unrecognized Command", "Vote", requestor);
       }
   
   void
     VOTE_FUNC (Vote::parseHELP)
       {
	  String word = tokens.nextToken();
	  String parm = tokens.nextToken();
	  services.doHelp(origin,"Vote",word,parm);
	  String tolog = "Did HELP on word " + word + " parm " + parm;
	  services.log(origin,"Vote",String(tolog));
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	return new Vote(services);
     }

   // Module information structure
   const Vote::moduleInfo_type Vote::moduleInfo = {
      "Voting Service",
	0, 0
   };


// Start the service
void Vote::start(void)
{
   services.registerService(name,name,"ircdome.org","+dz",
			    "Network Voting Service");
   services.serviceJoin(name,"#Debug");
}
