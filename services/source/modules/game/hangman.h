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

#ifndef __HANGMAN_H__
# define __HANGMAN_H__

# include <kineircd/str.h>
# include <utility>
# include <list>
# include <stack>

# include "channelgame.h"
# include "cards/hand.h"
# include "cards/card.h"

# define EXORDI8_FUNC(x) \
     bool x(Exordium::User& origin, LibAIS::StringTokens& line)

class Hangman : public ChannelGame {
 private:
   struct functionTable_type {
      const char* command;
      EXORDI8_FUNC((Hangman::* const function));
   } static const functionTable[];

   // Maximum number of players..
   static const unsigned int maxPlayers = 50;

   // The number of wrong guesses that can be made before the game ends
   static const unsigned int maxWrongGuesses = 5;
   
   // Is the game in play or still seeking players?
   bool playing;
   
   // A list of players 
   typedef std::list <Exordium::User*> players_type;
   players_type players;

   // The current player
   players_type::iterator currentPlayer;

   // Grab a player's details from our players list
   const Exordium::User* const getPlayer(const Exordium::User& player) const;
   Exordium::User* const getPlayer(const Exordium::User& player);
   
   // Is a specific player playing?
   bool isPlaying(const Exordium::User& player) const
     { return (getPlayer(player) != 0); };

   // Check a player's player status (returning their info if possible)
   Exordium::User* const checkPlayerStatus(Exordium::User& player,
					bool quiet = false);
   
   // Jump to the next player, telling the channel why
   void nextPlayer(const LibAIS::String& why, bool withMatchNotify = true);

   // Read in a random word depending on the level set
   bool getLevelData(int level);

   // Display the hangman
   void showWord(const Exordium::User& player);
   void showHangman(const Exordium::User& player);
   
   // Function handlers
   EXORDI8_FUNC(parsePLAY);
   EXORDI8_FUNC(parseSTART);
   EXORDI8_FUNC(parseGUESS);
//   EXORDI8_FUNC(parseSTATUS);
//   EXORDI8_FUNC(parseREVEAL);
//   EXORDI8_FUNC(parseDIFFICULTY);

   // This is the word to be guessed
   LibAIS::String word;

   // Letters already tried
   LibAIS::String wrongGuesses;
   LibAIS::String correctGuesses;

 public:
   // Constructor
   Hangman(Game::Module& module, const LibAIS::String& channel, Exordium::User& caller);
   
   // Handy creation function
   static CHANNEL_GAME_CREATOR_FUNC(createGame)
     { return new Hangman(module, channel, caller); }
   
   // Parse a line..
   bool parseLine(Exordium::User& origin, LibAIS::String& command,
		  LibAIS::StringTokens& tokens);
};
   
#endif // __HANGMAN_H__
