/*
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 *
 * This file is a part of Exordium.
 *
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,USA
 *
 * For contact details for the IRCDome Development Team please see the
 * enclosed README file.
 *
 */

#include <map>
#include <sstream>

#include <exordium/config.h>
#include <exordium/module.h>

#ifdef DEBUG
# include <cassert>
#endif

#include "love.h"

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
EXORDIUM_SERVICE_INIT_FUNCTION
{
   return new Module("love", new Love(services, name));
}

/* Our lovely list of commands in all their glorious ascii beauty. The names
 * should be in lower-case as it's the most common way people type stuff, and
 * our case-insensitive searchy thingy looks on the premise that the incoming
 * command will be converted to lower-case..
 */
const Love::commandTable_type Love::commandTable[] =
{
     {
	"commands",	0,	0,	&Love::handleCOMMANDS,
	  0
     },
     {
	"test",		0,	0,	&Love::handleTEST,
	  0
     },
     { 0, 0, 0, 0, 0 }
};

/* Love - Our constructor, mainly gets us ready to spread our loving..
 * Original 26/07/2002 simonb
 */
Love::Love(Exordium::Services& s, const String& mn)
  : services(s),
myName(mn)
{
   services.registerService(myName, myName, "ircdome.org", "+dz",
			    "Your local love slave(tm)");
   services.serviceJoin(myName, "#Chapel"); // hehehe, temporary maybe
}

/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Love::parseLine(const String& line, const String& origin)
{
   // Start breaking up the line
   StringTokens st(line);
   String command = st.nextToken().toLower();

   // Run through the list of commands to find a match
   for (int i = 0; commandTable[i].command != 0; i++)
     {
#ifdef DEBUG
	std::ostringstream debugLine;
	debugLine << "Love::parseLine() -> '" << line << "' from " << origin;
	services.Debug(debugLine.str());
#endif

	// Does this match?
	if (command == commandTable[i].command)
	  {
#ifdef DEBUG
	     // Oh "golly gosh", I hope the function really exists
	     assert(commandTable[i].handler != 0);
#endif

	     // Check if the minimum number of parameters is achieved
	     if (st.countTokens() < commandTable[i].minParams)
	       {
		  // Complain! We should send help here, really..
		  sendMessage(origin, "You need to use more parameters");
		  return;
	       }

	     // Check if the maximum number of parameters is set, if we have to
	     if ((commandTable[i].maxParams !=
		  Love::commandTable_type::MAX_PARAMS_UNLIMITED) &&
		 ((st.countTokens() - 1) > commandTable[i].maxParams))
	       {
		  // Complain.. THIS IS CRAP.. like above..
		  sendMessage(origin, "Too many parameters");
		  return;
	       }

	     // Run the command and leave early
	     (this->*(commandTable[i].handler))(origin, st);
	     return;
	  }
     }

   // Bitch and moan.. bitch and moan..
   sendMessage(origin, "Unrecognised Command");
}

/* handleCOMMANDS - Throw a quick 'n' dirty list of available commands out
 * Original 20/08/2002 simonb
 * Notes: Probably better to move this to the base class, along with myName
 *        and services..
 */
LOVE_FUNC(Love::handleCOMMANDS)
{
   // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength =
     services.getDaemon().getConfig().getOptionsLimitsMaxMessageLength() - 20;

   // Send the banner (this shouldn't be hard-coded)
   sendMessage(origin, "Command list for " + myName + ":");

   // Start formulating the data..
   std::ostringstream list(" -=>");
   for (int i = 0; commandTable[i].command != 0; i++)
     {
	// Add the command to the list
	list << " " << commandTable[i].command;

	// How are we for size?
	if (list.str().length() >= lineLength)
	  {
	     // Dump it and reset the string stream thingy
	     sendMessage(origin, list.str());
	     list.str() = " -=>";
	  }
     }

   // Anything left to send still?
   if (list.str().length() > 4)
     {
	sendMessage(origin, list.str());
     }

   // Send the footer (this shouldn't be hard-coded)
   sendMessage(origin, "End of command list");
}

/* handleTEST - Test thingy, temporary :)
 * Original 14/07/2002 simonb
 */
LOVE_FUNC(Love::handleTEST)
{
   // something here.. other than this.. perhaps...
   sendMessage(origin,
	       "It worked, and it's a member function, and it's calling a "
	       "mate of his which is also a member function!");
}
