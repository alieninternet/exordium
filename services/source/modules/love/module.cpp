/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

// Define if we are debugging
#ifndef DEBUG
# define DEBUG 1
#endif

#include "include/love.h"

#include <exordium/service.h>
#include <exordium/services.h>
#include <exordium/module.h>

#include <map>
#include <sstream>

// Use the stuff from the KineIRCd library using lovely short form
using Kine::String;
using Kine::StringTokens;

/* We are in the exordium namespace, or supposed to be anyway.. I don't know,
 * I'm too busy rebelling from the Wilkins Regime :(
 */
using namespace Exordium;


/* service_init - Register ourselves to the wonderous enigma that is the core
 * Original 13/07/2002 james
 * Note: Using C-style symbols for dlsym()
 */
extern "C" Module *service_init(void) {
   return new Module("love", new Love());
}


// Our lovely list of commands in all their glorious ascii beauty
const Love::functionTableStruct Love::functionTable[] = {
     { 0 }
};


/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Love::parseLine(const String &line, const String &origin)
{   
   // Start breaking up the line
   StringTokens st(line);
   String command = st.nextToken().toLower();
  
   // Run through the list of commands to find a match
   for (int i = 0; functionTable[i].command != 0; i++) {
#ifdef DEBUG
      ostringstream debugLine;
      debugLine << "Love::parseLine() -> '" << line << "' from " << origin;
      Services::Debug(debugLine.str());
#endif
      
      // Does this match?   
      if (command == functionTable[i].command) {
	 // Run the command and leave early
	 functionTable[i].function(origin, st);
	 return;
      }
   }
   
#ifdef DEBUG
   // Bitch and moan.. bitch and moan..
   Services::serviceNotice("Unrecognised Command", "Love", origin);
#endif
}
