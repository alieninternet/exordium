/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _SOURCE_MODULES_GAME_POKER_H_
# define _SOURCE_MODULES_GAME_POKER_H_ 1

# include <kineircd/str.h>
# include <utility>
# include <list>
# include <stack>

# include "channelgame.h"
# include "cards/hand.h"
# include "cards/card.h"
# include "cards/stock.h"

# define EXORDI8_FUNC(x) \
     bool x(Exordium::User& origin, AISutil::StringTokens& line)

class Poker : public ChannelGame {
 private:
   struct functionTable_type {
      const char* command;
      EXORDI8_FUNC((Poker::* const function));
   } static const functionTable[];

   struct gameTable_type {
      const char* command;
      EXORDI8_FUNC((Poker::* const function));
   } static const gameTable[];

   // Maximum number of players..
   static const unsigned int maxPlayers = 50;
   
   // Is the game in play or still seeking players?
   bool playing;
   
   // The last card to be discarded
   std::list<Cards::Card> lastDiscardedCards;

   // A temporary list of cards being discarded in one turn
   std::list<Cards::Card> discardList;
   
   // Are we in a sequent?
   bool sequentTrick;

   // A list of players (the .front() is always the caller)
   typedef std::pair <Exordium::User*, Cards::Hand> player_type;
   typedef std::list <player_type> players_type;
   players_type players;

   // The stock and discard piles 
   Cards::Stock< Cards::Card > stock;
   Cards::Stock< Cards::Card > discard;

   // The current player
   players_type::iterator currentPlayer;

   // Grab a player's details from our players list
   const player_type* const getPlayer(const Exordium::User& player) const;
   player_type* const getPlayer(const Exordium::User& player);
   
   // Is a specific player playing?
   bool isPlaying(const Exordium::User& player) const
     { return (getPlayer(player) != 0); };

   // Check a player's player status (returning their info if possible)
   player_type* const checkPlayerStatus(Exordium::User& player,
					bool quiet = false);
   
   // Show a given players hand to them
   void showHand(const player_type& player) const;

   // Jump to the next player, telling the channel why
   void nextPlayer(const AISutil::String& why, bool withMatchNotify = true);
   
   // Function handlers
   EXORDI8_FUNC(parseDEAL);
   EXORDI8_FUNC(parseDISCARD);
//   EXORDI8_FUNC(parsePASS);
   EXORDI8_FUNC(parsePLAY);
//   EXORDI8_FUNC(parseSHOWHAND);
//   EXORDI8_FUNC(parseSTATUS);
//   EXORDI8_FUNC(parseSUIT);

 public:
   // Constructor
   Poker(Game::Module& module, const AISutil::String& channel,
	   Exordium::User& caller);
   
   // Handy creation function
   static CHANNEL_GAME_CREATOR_FUNC(createGame)
     { return new Poker(module, channel, caller); }
   
   // Parse a line..
   bool parseLine(Exordium::User& origin, AISutil::String& command,
		  AISutil::StringTokens& tokens);
};
   
#endif // _SOURCE_MODULES_GAME_POKER_H_
