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

#include <map>
#include <sstream>

#ifdef DEBUG
# include <cassert>
#endif

#include "love.h"
#include "language.h"

#include <kineircd/config.h>


using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::LoveModule;


/* Our lovely list of commands in all their glorious ascii beauty. The names
 * should be in lower-case as it's the most common way people type stuff, and
 * our case-insensitive searchy thingy looks on the premise that the incoming
 * command will be converted to lower-case..
 */
const Service::commandTable_type Service::commandTable[] =
{
     { "commands",		&Service::handleCOMMANDS },
     { 0, 0 }
};


/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Service::parseLine(StringTokens& line, User& origin, const bool safe)
{
   // Start breaking up the line
   String command = line.nextToken().toLower();

   // Run through the list of commands to find a match
   for (int i = 0; commandTable[i].command != 0; i++) {
      // Does this match?
      if (command == commandTable[i].command) {
#ifdef DEBUG
	 // I hope the function really exists
	 assert(commandTable[i].handler != 0);
#endif
	 
	 // Run the command and leave
	 (this->*(commandTable[i].handler))(origin, line);
	 return;
      }
   }
   
   // Bitch and moan.. bitch and moan..
   sendMessage(origin, GETLANG(ERROR_UNKNOWN_COMMAND));
}


/* handleCOMMANDS - Throw a quick 'n' dirty list of available commands out
 * Original 20/08/2002 pickle
 * Notes: Probably better to move this to the base class, along with getName()
 *        and services..
 */
LOVE_FUNC(Service::handleCOMMANDS)
{
   // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength =
     Kine::config().getLimitsMaxMessageLength() - 20;

   // Send the banner (this shouldn't be hard-coded)
   sendMessage(origin, GETLANG(COMMAND_LIST_START,getNickname()));

   // Start formulating the data..
   std::ostringstream list(" -=>");
   for (int i = 0; commandTable[i].command != 0; i++) {
      // Add the command to the list
      list << " " << commandTable[i].command;
      
      // How are we for size?
      if (list.str().length() >= lineLength) {
	 // Dump it and reset the string stream thingy
	 sendMessage(origin, list.str());
	 list.str() = " -=>";
      }
   }
   
   // Anything left to send still?
   if (list.str().length() > 4) {
      sendMessage(origin, list.str());
   }
   
   // Send the footer (this shouldn't be hard-coded)
   sendMessage(origin, GETLANG(COMMAND_LIST_END));
}
