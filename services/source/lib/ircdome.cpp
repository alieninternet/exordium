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

const struct IRCDome::functionTableStruct IRCDome::functionTable[] =
{
     { "module", &IRCDome::parseMODULE },
     { 0, 0 }
};

void IRCDome::parseLine(const String &line, const String &requestor)
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
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   services.serviceNotice ("Unrecognized Command", "IRCDome", requestor);
}

void
  IRCDOME_FUNC (IRCDome::parseMODULE)
{
   String command = tokens.nextToken();
   if(command=="list")
     {
	String foo = services.serviceM.dumpModules();
	services.serviceNotice("The currently loaded service modules are","IRCDome",origin);
	services.serviceNotice(foo,"IRCDome",origin);
	return;
     }
   if(command=="unload")
     {
	String name = tokens.nextToken();
	services.serviceNotice("Unloading module "+name,"IRCDome",origin);
	services.unloadModule(name);
     }
   if(command=="load")
     {
	String name = tokens.nextToken();
	String filename = tokens.nextToken();
	services.serviceNotice("Attempting to load module","IRCDome",origin);
	if(!services.loadModule(name,filename))
	  {
	     services.serviceNotice("Error loading module","IRCDome",origin);
	     return;
	  }
	services.serviceNotice("Module loaded successfully","IRCDome",origin);

     }

}
