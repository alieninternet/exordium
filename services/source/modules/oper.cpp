/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "include/oper.h"

#include "exordium/channel.h"
#include "exordium/nickname.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;


struct Oper::functionTableStruct const
  Oper::functionTable[] = {
  {"help", parseHELP},
  {0}
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
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st, ch);
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
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st, ch);
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
	Services::doHelp(origin,"oper",word,parm);
}

EXORDIUM_SERVICE_INIT_FUNCTION {
	Services::registerService(name,name,"ircdome.org", "+dz",
					"IRC Operator Services");
	Services::serviceJoin(name,"#Debug");
	return new Module("oper",new Oper(name));
}




