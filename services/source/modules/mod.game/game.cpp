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

#include <exordium/channel.h>
#include <exordium/service.h>
#include <exordium/services.h>
#include <exordium/utils.h>
#include <aisutil/string.h>

#include "game.h"
#include "factory.h"

using namespace Exordium::GameModule;
using AISutil::String;
using AISutil::StringTokens;
using Exordium::Channel;
using Exordium::User;


/* service_init - Register ourselves to the core
 * Original 13/07/2002 james
 */
EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }


// Module information structure
const Module::moduleInfo_type Module::moduleInfo = {
   "Game Service",
     0, 0 ,
     Exordium::Service::moduleInfo_type::Events::NONE
};


// Our command table for directly sent commands (commands must be lower-case)
const Module::commandTable_type Module::directCommandTable[] =
{
     { "quote",		&Module::handleQUOTE },
     { "help",		&Module::handleHELP },
     { "start",		&Module::handleSTART },
     { "list",		&Module::handleLIST },
     { 0, 0 }
};


// Our command table for channel commands (commands must be lower-case)
const Module::commandTable_type Module::channelCommandTable[] =
{
     { "quote",		&Module::handleQUOTE },
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
   // Leave all the channels we're on..
   while (!channelGames.empty()) {
      // Part the channel... In the future we should be able to give a reason
      services->servicePart(getNickname(), 
			    (*(channelGames.begin())).second->getChannel());
      
      // Delete this game..
      delete (*(channelGames.begin())).second;
      channelGames.erase(channelGames.begin());
   }
   
   // Quit - bye bye!
   services->serviceQuit(getNickname(), reason);
}


/* parseLine - Parse an incoming message (which was sent to a channel)
 * Original 13/07/2002 james
 */
void Module::parseLine(StringTokens& line__, User& origin,
		       const Kine::ChannelName& channel)
{
   // dirty kludge.. at least until the core strips the char properly??
   StringTokens line(line__.rest().substr(1));
   
   // Grab the command
   String command = line.nextToken().toLower();
   
   // Run through the list of commands to find a match
   for (int i = 0; channelCommandTable[i].command != 0; i++) {
      // Does this match?
      if (command == channelCommandTable[i].command) {
	 // Run the command and leave
	 (this->*(channelCommandTable[i].handler))(origin, line, channel);
	 return;
      }
   }
   
   // Okay no matches, check if this channel is one playing a game
   channelGames_type::iterator game = channelGames.find(channel.IRCtoLower());
   
   if (game != channelGames.end()) {
      // If the parser returns false, it means we can leave the channel
      if (!(*game).second->parseLine(origin, command, line)) {
	 // Leave the channel and delete this game..
	 services->servicePart(getNickname(), channel);
	 delete (*game).second;
	 channelGames.erase(game);
      }
   }
   
   // No complaints - Unknown commands are silently ignored :)
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
GAME_FUNC(Module::handleHELP)
{
   services->doHelp(origin, getNickname(), line.nextToken(),
		    line.nextToken());
}


/* handleQUOTE - Parse the QUOTE command
 * Original 13/07/2002 james
 * Note: Mess :(
 */
GAME_FUNC(Module::handleQUOTE) 
{
   return; // eek
   
   String chan = "";
   if(chan != "") {
      chan = channel;
   } else {
      chan = line.nextToken();
   }
   
   if(channel == "") {
      origin.sendMessage("Usage: quote #channel", getNickname());
      return;
   }
   
   
   int j;
  
   String numb = String::convert(services->getDatabase().dbCount("fortunes"));
   j = Utils::random(numb.toInt());

   // Grab the quote
   if (services->getDatabase().dbSelect("body", "fortunes", 
					"id='" + String::convert(j) + 
					"'") < 1) {
      return; // eek
   }

   String thequote = services->getDatabase().dbGetValue();
   StringTokens st (thequote);
   bool more = false;
   more = st.hasMoreTokens();
   
   while (more == true) {
      String tq = st.nextToken('\n');
      services->servicePrivmsg(tq, getNickname(), chan);
      more = st.hasMoreTokens();
   }   
}

/* handleSTART - Parse a 'start' command, to trigger the start of game
 * Original 29/08/2002 - pickle
 */
GAME_FUNC(Module::handleSTART)
{
   Kine::ChannelName chan = line.nextToken();
   chan = chan.IRCtoLower();
   String game = line.nextToken().toLower();
#ifdef DEBUG
   std::cout << "have been asked to start "<<game<< " in " << chan <<
std::endl;
#endif
   
   // Check for the game
   ChannelGame* chanGame = 
     Factory::Instance().createGame(game, *this, chan, origin);

   if(chanGame)
   {
     channelGames[chan] = chanGame;
     // Join the channel and say hello
     services->serviceJoin(getNickname(), chan);
     services->serverMode(chan, "+o", getNickname());
     services->serviceNotice("Hello " + chan + " (" + origin.getNickname() + 
         " wanted to play " + game + ')', "Game", chan);
   }

   // give them an error???!!
}

/* handleSTART - Parse a 'list' command, to list all available games
 * Original 16/09/2002 - josullivan
 */
GAME_FUNC(Module::handleLIST)
{
   typedef std::list<AISutil::String> StringList;
   StringList::const_iterator iter;

   // Get module list
   StringList list = Factory::Instance().listModules();

   // Check for the game
   origin.sendMessage("List of available games:", getNickname());
   for(iter = list.begin(); iter != list.end(); iter++)
   {
   //for (int i = 0; ChannelGame::channelGameTable[i].game != 0; i++) {
      String str = "--- ";
      //str += ChannelGame::channelGameTable[i].game;
      str += (*iter);
      origin.sendMessage(str, getNickname());
   }
}


