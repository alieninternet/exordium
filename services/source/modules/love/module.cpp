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

#ifdef DEBUG
# include <cassert>
#endif

// Use the stuff from the KineIRCd library using lovely short form
using Kine::String;
using Kine::StringTokens;

/* We are in the exordium namespace, or supposed to be anyway.. I don't know,
 * I'm too busy rebelling from the Wilkins Facsist Regime :( 
 * DOWN WITH THE W.F.R.!!!
 */
using namespace Exordium;


/* service_init - Register ourselves to the wonderous enigma that is the core
 * Original 13/07/2002 james
 * Note: Using C-style symbols for dlsym()
 */
extern "C" Module *service_init(void) {
   return new Module("love", new Love());
}


/* Our lovely list of commands in all their glorious ascii beauty. The names
 * should be in lower-case as it's the most common way people type stuff, and
 * our case-insensitive searchy thingy looks on the premise that the incoming
 * command will be converted to lower-case..
 */
const Love::commandTable_type Love::commandTable[] = {
     { 
	"test",		0,	0,	&Love::handleTEST,
	  0 
     },
     { 0, 0, 0, 0, 0 }
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
   for (int i = 0; commandTable[i].command != 0; i++) {
#ifdef DEBUG
      ostringstream debugLine;
      debugLine << "Love::parseLine() -> '" << line << "' from " << origin;
      Services::Debug(debugLine.str());
#endif
      
      // Does this match?   
      if (command == commandTable[i].command) {
#ifdef DEBUG
	 // Oh "golly gosh", I hope the function really exists
	 assert(commandTable[i].handler != 0);
#endif
	 
	 // Run the command and leave early
	 (this->*(commandTable[i].handler))(origin, st);
	 return;
      }
   }
   
#ifdef DEBUG
   // Bitch and moan.. bitch and moan..
   Services::serviceNotice("Unrecognised Command", "Love", origin);
#endif
}


/* handleTEST - Test thingy, temporary :)
 * Original 14/07/2002 simonb
 */
LOVE_FUNC(Love::handleTEST)
{
   // something here.. other than this.. perhaps...
   Services::servicePrivmsg("It worked, AND I AM A MEMBER FUNCTION!!", 
			    "Love", origin);
}
