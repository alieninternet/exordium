/* $Id$
 * Exordi8 card game
 * 
 * Copyright (c) 2002 Simon Butcher <simonb@alien.net.au>
 *
 * This file is a part of Exordium.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __EXORDI8_H__
# define __EXORDI8_H__

# include <kineircd/str.h>
# include <utility>
# include <list>
# include <stack>

# include "channelgame.h"
# include "cards/hand.h"
# include "cards/card.h"

# define EXORDI8_FUNC(x) \
     bool x(const Kine::String& origin, Kine::StringTokens& line)


class Exordi8 : public ChannelGame {
 private:
   struct functionTable_type {
      const char* command;
      EXORDI8_FUNC((Exordi8::* const function));
   } static const functionTable[];

   // Maximum number of players..
   static const unsigned int maxPlayers = 50;
   
   // Is the game in play or still seeking players?
   bool playing;
   
   // The last card to be discarded
   Cards::Card lastDiscardedCard;
   
   // Are we playing in a forward or backwards direction?
   bool forwardDirection;

   // Are we in a sequent?
   bool sequentTrick;

   /* Next suit must be one of these. If this is 0, it means we are waiting
    * for a suit. If it is a suit *AND* the lastDicardedCard is an 8, then
    * the next card must be of this suit. If the next card is not an 8 and
    * this is not 0, then it is meaningless.
    */
   unsigned char nextSuit;
   
   // A list of players (the .front() is always the caller)
   typedef std::pair <Kine::String, Cards::Hand> player_type;
   typedef std::list <player_type> players_type;
   players_type players;

   // The stock of cards
   typedef std::stack <Cards::Card> stock_type;
   stock_type stock;

   // The current player
   players_type::iterator currentPlayer;

   // Grab a player's details from our players list
   const player_type* const getPlayer(const Kine::String& player) const;
   player_type* const getPlayer(const Kine::String& player);
   
   // Is a specific player playing?
   bool isPlaying(const Kine::String& player) const
     { return (getPlayer(player) != 0); };

   // Check a player's player status (returning their info if possible)
   player_type* const checkPlayerStatus(const Kine::String& player, 
					bool quiet = false);
   
   // Show a given players hand to them
   void showHand(const player_type& player) const;

   // Jump to the next player, telling the channel why
   void nextPlayer(const Kine::String& why, bool withMatchNotify = true);
   
   // Function handlers
   EXORDI8_FUNC(parseDEAL);
   EXORDI8_FUNC(parseDISCARD);
   EXORDI8_FUNC(parsePASS);
   EXORDI8_FUNC(parsePICKUP);
   EXORDI8_FUNC(parsePLAY);
   EXORDI8_FUNC(parseSHOWHAND);
   EXORDI8_FUNC(parseSTATUS);
   EXORDI8_FUNC(parseSUIT);

 public:
   // Constructor
   Exordi8(Game& game, const Kine::String& channel, 
	   const Kine::String& caller);

   // Parse a line..
   bool parseLine(Kine::String& origin, Kine::StringTokens& tokens);
};
   
#endif // __EXORDI8_H__
   
