/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "credits.h"
#include "language.h"

#include <exordium/channel.h>
#include <exordium/service.h>
#include <exordium/services.h>


using namespace Exordium::CreditsModule;
using AISutil::String;
using AISutil::StringTokens;
using Exordium::User;


// Our command table for directly sent commands (commands must be lower-case)
const Service::commandTable_type Service::directCommandTable[] =
{
     { "help",          &Service::handleHELP },
     { "balance",       &Service::handleBALANCE },
     { 0, 0 }
};


// Our command table for channel commands (commands must be lower-case)
const Service::commandTable_type Service::channelCommandTable[] =
{
     { 0, 0 }
};


/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Service::parseLine(StringTokens& line, User& origin, const bool safe)
{
   String command = line.nextToken().toLower();
#ifdef DEBUG
   std::cout << "Trying to throw command to commandtable thingy" << command <<
std::endl;
#endif
   for (int i = 0; directCommandTable[i].command != 0; i++) {
      // Does this match?
      if (command == directCommandTable[i].command) {
	 // Run the command and leave
	 (this->*(directCommandTable[i].handler))(origin, line, "");
	 return;
      }
   }
   
   origin.sendMessage("Unrecognized Command", getNickname());
}


/* handleHELP - Parse the HELP command
 * Original 13/07/2002 james
 */
CREDITS_FUNC(Service::handleHELP)
{
   services.doHelp(origin, getNickname(), line.nextToken(),
		   line.nextToken());
}


/* handleBALANCE - Parse a 'balance' command, returns the users current balance
 * Original 20/10/2002 - josullivan
 */
CREDITS_FUNC(Service::handleBALANCE)
{
  int balance = bank.getBalance(origin);
  if(balance > 0)
  {
     origin.sendMessage(GETLANG(credits_BALANCE_current,String::convert(balance)), getNickname());
  }
  else
  {
    origin.sendMessage(GETLANG(credits_BALANCE_empty), getNickname());
  }
}
