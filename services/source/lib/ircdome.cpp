/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/services.h"
#include "exordium/nickname.h"
#include <kineircd/str.h>
#include <kineircd/utils.h>
#include "exordium/ircdome.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

struct IRCDome::functionTableStruct const
  IRCDome::functionTable[] = {
  {"module", parseMODULE},
  {0}
};

void 
IRCDome::parseLine (String & line, String const &requestor)
{   
  StringTokens st (line);
  String origin = requestor;
  String command = st.nextToken ().toLower ();
  for (int i = 0; functionTable[i].command != 0; i++)
    {
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "IRCDome", requestor);
}

void
IRCDOME_FUNC (IRCDome::parseMODULE)
{
String command = tokens.nextToken();
if(command=="list")
	{
		String foo = Services::serviceM.dumpModules();
		Services::serviceNotice("The currently loaded service modules are","IRCDome",origin);
		Services::serviceNotice(foo,"IRCDome",origin);		
		return;
	}
if(command=="unload")
	{
		String name = tokens.nextToken();
		Services::serviceNotice("Unloading module "+name,"IRCDome",origin);
		Services::unloadModule(name);
	}
if(command=="load")
	{
		String name = tokens.nextToken();
		String filename = tokens.nextToken();
		Services::serviceNotice("Attempting to load module","IRCDome",origin);
		if(!Services::loadModule(name,filename))
		{
			Services::serviceNotice("Error loading module","IRCDome",origin);
			return;
		}
		Services::serviceNotice("Module loaded successfully","IRCDome",origin);

	}

}
