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


/* service_init - Register ourselves to the core
 * Original 13/07/2002 james
 */
EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }


// Module information structure
const Module::moduleInfo_type Module::moduleInfo = {
  "Credits Service", 
    0, 0,
    Exordium::Service::moduleInfo_type::Events::NONE
};


// Our command table for directly sent commands (commands must be lower-case)
const Module::commandTable_type Module::directCommandTable[] =
{
     { "help",          &Module::handleHELP },
     { "balance",       &Module::handleBALANCE },
     { 0, 0 }
};


// Our command table for channel commands (commands must be lower-case)
const Module::commandTable_type Module::channelCommandTable[] =
{
     { 0, 0 }
};


/* start - Start the service
 * Original 17/09/2002 pickle
 */
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;
   
   // Register ourself to the network
   services->registerService(getNickname(), getUsername(),
			     getHostname(), getDescription());
   
   // We started okay :)
   return true;
}


/* stop - Stop the service
 * Original 17/09/2002 pickle
 */
void Module::stop(const String& reason)
{
   // Umm, we should QUIT from the network here.. but how?
}


/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Module::parseLine(StringTokens& line, User& origin, const bool safe)
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
CREDITS_FUNC(Module::handleHELP)
{
   services->doHelp(origin, getNickname(), line.nextToken(),
		    line.nextToken());
}


/* handleBALANCE - Parse a 'balance' command, returns the users current balance
 * Original 20/10/2002 - josullivan
 */
CREDITS_FUNC(Module::handleBALANCE)
{
  int balance = bank.getBalance(origin);
  if(balance > 0)
  {
    origin.sendMessage(GETLANG(credits_BALANCE_current,balance), getNickname());
  }
  else
  {
    origin.sendMessage(GETLANG(credits_BALANCE_empty), getNickname());
  }
}
