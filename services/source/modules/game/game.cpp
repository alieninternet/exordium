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

#include <exordium/channel.h>
#include <exordium/service.h>
#include <exordium/services.h>
#include <exordium/sql.h>
#include <kineircd/str.h>
#include <map>

#include "game.h"

using LibAIS::String;
using LibAIS::StringTokens;
using namespace Exordium;


/* service_init - Register ourselves to the core
 * Original 13/07/2002 james
 */
EXORDIUM_SERVICE_INIT_FUNCTION
{
   return new Game();
}


// Module information structure
const Game::moduleInfo_type Game::moduleInfo = {
   "Game Service",
     0, 0
};


// Our command table for directly sent commands (commands must be lower-case)
const Game::commandTable_type Game::directCommandTable[] =
{
     { "quote",		&Game::handleQUOTE },
     { "help",		&Game::handleHELP },
     { "start",		&Game::handleSTART },
     { "list",		&Game::handleLIST },
     { 0, 0 }
};


// Our command table for channel commands (commands must be lower-case)
const Game::commandTable_type Game::channelCommandTable[] =
{
     { "quote",		&Game::handleQUOTE },
     { 0, 0 }
};


/* start - Start the service
 * Original 17/09/2002 simonb
 */
void Game::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;
   
   // Register ourself to the network
   services->registerService(getName(), getName(), 
			    getConfigData().getHostname(), "+dz",
			    getConfigData().getDescription());
}


/* stop - Stop the service
 * Original 17/09/2002 simonb
 */
void Game::stop(void)
{
   // Leave all the channels we're on..
   while (!channelGames.empty()) {
      // Part the channel...
      services->servicePart(getName(), 
			    (*(channelGames.begin())).second->getChannel());
      
      // Delete this game..
      delete (*(channelGames.begin())).second;
      channelGames.erase(channelGames.begin());
   }
   
   // Umm, we should QUIT from the network here.. but how?
}


/* parseLine - Parse an incoming message (which was sent to a channel)
 * Original 13/07/2002 james
 */
void Game::parseLine(StringTokens& line__, User& origin, const String& channel)
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
	 services->servicePart(getName(), channel);
	 delete (*game).second;
	 channelGames.erase(game);
      }
   }
   
   // No complaints - Unknown commands are silently ignored :)
}


/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Game::parseLine(StringTokens& line, User& origin)
{
   String command = line.nextToken().toLower();
   std::cout << "Trying to throw command to commandtable thingy" << command << std::endl;
   for (int i = 0; directCommandTable[i].command != 0; i++) {
      // Does this match?
      if (command == directCommandTable[i].command) {
	 // Run the command and leave
	 (this->*(directCommandTable[i].handler))(origin, line, "");
	 return;
      }
   }
   
   origin.sendMessage("Unrecognized Command", getName());
}


/* handleHELP - Parse the HELP command
 * Original 13/07/2002 james
 */
GAME_FUNC(Game::handleHELP)
{
   services->doHelp(origin, getName(), line.nextToken(),
		    line.nextToken());
}


/* handleQUOTE - Parse the QUOTE command
 * Original 13/07/2002 james
 * Note: Mess :(
 */
GAME_FUNC(Game::handleQUOTE) 
{
   return; // eek
   
   String chan = "";
   if(chan != "") {
      chan = channel;
   } else {
      chan = line.nextToken();
   }
   
   if(channel == "") {
      origin.sendMessage("Usage: quote #channel", getName());
      return;
   }
   
   String query = "SELECT count(*) from fortunes";
   MysqlRes res = services->getDatabase().query(query);
   MysqlRow row;
   int j;
   
   while ((row = res.fetch_row())) {
      String numb = ((std::string) row[0]).c_str();
      j = services->random(numb.toInt());
   }
   
   res.free_result();
   String thequote = services->getQuote(j);
   StringTokens st (thequote);
   bool more = false;
   more = st.hasMoreTokens();
   
   while (more == true) {
      String tq = st.nextToken('\n');
      services->servicePrivmsg(Sql::makeSafe(tq), getName(), chan);
      more = st.hasMoreTokens();
   }
   
}

/* handleSTART - Parse a 'start' command, to trigger the start of game
 * Original 29/08/2002 - simonb
 */
GAME_FUNC(Game::handleSTART)
{
   String chan = line.nextToken().IRCtoLower();
   String game = line.nextToken().toLower();
   std::cout << "have been asked to start "<<game<< " in " << chan << std::endl;
   
   // Check for the game
   for (int i = 0; ChannelGame::channelGameTable[i].game != 0; i++) {
      // Does this match?
      if (game == ChannelGame::channelGameTable[i].game) {
	 // Create a new game..
	 channelGames[chan] =
	   ChannelGame::channelGameTable[i].creator(*this, chan, origin);
	 
	 // Join the channel and say hello
	 services->serviceJoin(getName(), chan);
	 services->serverMode(chan, "+o", getName());
	 services->serviceNotice("Hello " + chan + " (" + origin.getNickname() +
				 " wanted to play " + game + ')',
				 "Game", chan);
	 
	 // Leave the loop
	 return;
      }
   }
   
   // give them an error???!!
}

/* handleSTART - Parse a 'list' command, to list all available games
 * Original 16/09/2002 - josullivan
 */
GAME_FUNC(Game::handleLIST)
{
   // Check for the game
   origin.sendMessage("List of available games:", getName());
   for (int i = 0; ChannelGame::channelGameTable[i].game != 0; i++) {
      String str = "--- ";
      str += ChannelGame::channelGameTable[i].game;
      
      origin.sendMessage(str, getName());
   }
}


