/*
    LOVE.CPP   - Main entry point for IRCDome Network Services
    Version   - 0.1
    Date      - 8th May 2002
    Revisions - $Id$
  
    Copyright 2002 James Ian Wilkins <james@ircdome.org>

 */

#include "exordium/love.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include "kineircd/str.h"
#include "exordium/module.h"
#include <map>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;
struct Love::functionTableStruct const
  Love::functionTable[] = {
  {0}
};
void
Love::parseLine (String const &line, String const &requestor, String const &ch)
{
return;
}
void 
Love::parseLine (String const &line, String const &requestor)
{   
  StringTokens st (line);
  String origin = requestor;
  String command = st.nextToken ().toLower ();
  for (int i = 0; functionTable[i].command != 0; i++)
    {
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "Love", requestor);
}


extern "C" Module *service_init(void) {
   return new Module("love", new Love());
}

