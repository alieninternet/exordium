/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "include/bot.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include "kineircd/str.h"
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;


namespace Exordium {

struct Bot::functionTableStruct const
  Bot::functionTable[] = {
  {"quote", parseQUOTE},
  {".quote", parseQUOTE},
  {"help", parseHELP},
  {0}
};

void
Bot::parseLine (String const &line, String const &requestor, String const &chan)
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
Bot::parseLine (String const &line, String const &requestor)
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
  Services::serviceNotice ("Unrecognized Command", "Bot", requestor);
}

void
BOT_FUNC (Bot::parseHELP)
{
	String word = tokens.nextToken();
	String parm = tokens.nextToken();
	Services::doHelp(origin,"bot",word,parm);
}

EXORDIUM_SERVICE_INIT_FUNCTION {
   return new Module("bot", new Bot());
}



};

