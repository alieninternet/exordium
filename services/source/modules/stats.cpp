/*
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
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

