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


#include <exordium/channel.h>
#include <exordium/nickname.h>
#include <exordium/service.h>
#include <exordium/services.h>
#include <exordium/module.h>
#include <exordium/sql.h>
#include <kineircd/str.h>
#include <map>

#include "game.h"
#include "exordi8.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;


/* service_init - Register ourselves to the core
 * Original 13/07/2002 james
 */
EXORDIUM_SERVICE_INIT_FUNCTION
{
   return new Module("game", new Game(services, name));
}


// Our command table (all commands must be lower-case)
const Game::commandTable_type Game::commandTable[] =
{
     { "quote",		&Game::handleQUOTE },
     { ".quote",	&Game::handleQUOTE },
     { "help",		&Game::handleHELP },
     { "start",		&Game::handleSTART },
     { 0, 0 }
};


/* Game - Our constructor which joined appropriate channels, etc.
 * Original 11/09/2002
 */
Game::Game(Exordium::Services& s, const String& mn)
  : services(s), 
    myName(mn)
{
   services.registerService(myName, myName, "ircdome.org", "+dz", 
			    "Network Games!");
}


/* parseLine - Parse an incoming message (which was sent to a channel)
 * Original 13/07/2002 james
 */
void Game::parseLine(const String& line, const String& origin, 
		     const String& channel)
{
   StringTokens st(line);
   String command = st.nextToken().toLower();
   
   // Run through the list of commands to find a match
   for (int i = 0; commandTable[i].command != 0; i++) {
      // Does this match?
      if (command == commandTable[i].command) {
	 // Run the command and leave
	 (this->*(commandTable[i].handler))(origin, st, channel);
	 return;
      }
   }
   
   // Okay no matches, check if this channel is one playing a game
   channelGames_type::iterator game = channelGames.find(channel.IRCtoLower());
   
   if (game != channelGames.end()) {
      // Too lazy to work around the mess above
      StringTokens tokens(line.substr(1));
      
      // If the parser returns false, it means we can leave the channel
      if (!(*game).second->parseLine(origin, tokens)) {
	 // Leave the channel and delete this game..
	 services.servicePart(myName, channel);
	 delete (*game).second;
	 channelGames.erase(game);
      }
   }
   
   // No complaints - Unknown commands are silently ignored :)
}


/* parseLine - Parse an incoming message (which was sent directly to us)
 * Original 13/07/2002 james
 */
void Game::parseLine(const String& line, const String& origin)
{
   StringTokens st (line);
   String command = st.nextToken().toLower ();
   
   for (int i = 0; commandTable[i].command != 0; i++) {
      // Does this match?
      if (command == commandTable[i].command) {
	 // Run the command and leave
	 (this->*(commandTable[i].handler))(origin, st, "");
	 return;
      }
   }
   
   services.serviceNotice("Unrecognized Command", myName, origin);
}


/* handleHELP - Parse the HELP command
 * Original 13/07/2002 james
 */
GAME_FUNC(Game::handleHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services.doHelp(origin, myName, word, parm);
}


/* handleHELP - Parse the HELP command
 * Original 13/07/2002 james
 */
GAME_FUNC(Game::handleQUOTE) 
{
   return; // eek
   
   String channel = "";
   if(chan != "") {
      channel = chan;
   } else {
      channel = tokens.nextToken();
   }
   
   if(channel == "") {
      services.serviceNotice("Usage: quote #channel",myName,origin);
      return;
   }
   
   String query = "SELECT count(*) from fortunes";
   MysqlRes res = services.getDatabase().query(query);
   MysqlRow row;
   int j;
   
   while ((row = res.fetch_row())) {
      String numb = ((std::string) row[0]).c_str();
      j = services.random(numb.toInt());
   }
   
   res.free_result();
   String thequote = services.getQuote(j);
   StringTokens st (thequote);
   bool more = false;
   more = st.hasMoreTokens();
   
   while (more == true) {
      String tq = st.nextToken('\n');
      services.servicePrivmsg(services.getDatabase().makeSafe(tq),"Game",channel);
      more = st.hasMoreTokens();
   }
   
}

/* handleSTART - Parse a 'start' command, to trigger the start of game
 * Original 29/08/2002 - simonb
 */
GAME_FUNC(Game::handleSTART)
{
   String channel = tokens.nextToken().IRCtoLower();
   String game = tokens.nextToken().toLower();
   
   // Check for the game
   for (int i = 0; ChannelGame::channelGameTable[i].game != 0; i++) {
      // Does this match?
      if (game == ChannelGame::channelGameTable[i].game) {
	 // Create a new game..
	 channelGames[channel] =
	   ChannelGame::channelGameTable[i].creator(*this, channel, origin);
	 
	 // Join the channel and say hello
	 services.serviceJoin(myName, channel);
	 services.serverMode(channel, "+o", myName);
	 services.serviceNotice("Hello " + channel + " (" + origin +
				" wanted to play " + game + ')',
				"Game", channel);
	 
	 // Leave the loop
	 return;
      }
   }
//   if (game == "EXORDI8") {
//      channelGames[channel] = new Exordi8(*this, channel, origin);
//   } else {
//      // give them an error???!!
//      return;
//   }

   // give them an error???!!
}
