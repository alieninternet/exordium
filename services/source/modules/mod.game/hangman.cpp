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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>

#include <exordium/services.h>

#include "hangman.h"

using AISutil::String;
using AISutil::StringTokens;

// This is needed for translate since it doesn't like tolower gcc 3.x
//inline char tolower_wrapper (char ch) { return tolower(ch); }

const char* hangman[10][7] = {{"",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "+------"}, 
                              {"",
                               "|      ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|    o ",
                               "|      ",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|    o ",
                               "|    | ",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|    o ",
                               "|   /| ",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|    o ",
                               "|   /|\\",
                               "|      ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|    o ",
                               "|   /|\\",
                               "|   /  ",
                               "|      ",
                               "+------"}, 
                              {"+----+ ",
                               "|    | ",
                               "|    o ",
                               "|   /|\\",
                               "|   / \\",
                               "|      ",
                               "+------"}};
 
CHANNEL_GAME_CREATOR_FUNCTION
{
     return new Hangman(module, channel, caller);
}

/* Hangman - Constructor for a new Hangman game being played on a channel
 * Original 17/09/2002 josullivan
 */
Hangman::Hangman(Exordium::GameModule::Service& module, const Kine::ChannelName& channel, 
		 Exordium::User& caller)
  : ChannelGame(module, channel.IRCtoLower()),
    playing(false)
//    sequentTrick(false),
{
   // Add the person who fired up the game to the players list
   players.push_back(&caller);
};


// The list of functions - yay.
const Hangman::functionTable_type Hangman::functionTable[] =
{
     { "play",			&Hangman::parsePLAY },
     { "start",			&Hangman::parseSTART },
     { "level",			&Hangman::parseSTART },
     { "guess",			&Hangman::parseGUESS },
     { 0, 0 }
};


/* parseLine - Parse an incoming command from someone
 * Original 29/08/2002 simonb
 */
bool Hangman::parseLine(Exordium::User& origin, String& command, 
			StringTokens& tokens)
{
   // Run through the command list and find a matching command..
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Is this a match?
      if (command == functionTable[i].command) {
	 // Run the function handler, and leave
	 return (this->*(functionTable[i].function))(origin, tokens);
	 break;
      }
   }
   
   // For now, we will just consider that the channel game should remain alive
   return true;
}


/* getPlayer [Various Forms] - Find a player
 * Original 29/08/2002 simonb
 * Note: Could be more efficient, but I'm a little too lazy at the moment
 */
const Exordium::User* const 
  Hangman::getPlayer(const Exordium::User& player) const
{
   // Run through the list and find this player.. *sigh*
   for (players_type::const_iterator it = players.begin(); it !=
players.end();
	it++) {
      // Does this match?
      if (*(*it) == player) {
	 return (*it);
      }
   }
   
   // We could not find that player obviously
   return 0;
}

Exordium::User* const Hangman::getPlayer(const Exordium::User& player)
{
   // Run through the list and find this player.. *sigh*
   for (players_type::iterator it = players.begin(); it != players.end(); 
	it++) {
      // Does this match?
      if (*(*it) == player) {
	 return (*it);
      }
   }
   
   // We could not find that player obviously
   return 0;
}


/* Check a player's player status (returning their info if possible)
 * Original 30/08/2002 simonb
 */
Exordium::User* const 
  Hangman::checkPlayerStatus(Exordium::User& player, bool quiet)
{
   // Find the player's info
   Exordium::User* const playerInfo = getPlayer(player);
   
   // Make sure we found something
   if (playerInfo == 0) {
      if (!quiet) {
	 sendMessage(player, "You're not playing Hangman!");
      }
      return 0;
   }
   
   return playerInfo;
}


/* nextPlayer - Jump to the next player, telling the channel why parse
 * Original 30/08/2002 simonb
 */
void Hangman::nextPlayer(const String& why, bool withMatchNotify)
{
   std::ostringstream out;
   out << (*currentPlayer)->getNickname() << ' ' << why << ". It is ";

   // Check if this player is at the end of the players list
   if (currentPlayer == --players.end()) {
      // Jump to the first player in the list
      currentPlayer = players.begin();
   } else {
      // Move on to the next player in the list
      currentPlayer++;
   }

   // Add the current player's name to the string
   out << (*currentPlayer)->getNickname();
   
   // ... get our grammar right.. :)
   if ((*currentPlayer)->getNickname()
       [(*currentPlayer)->getNickname().length() - 1] == 's') {
      out << "' turn to guess.";
   } else {
      out << "'s turn to guess.";
   }
   
   // Tell the channel what is happening
   sendMessage(out.str());
}

/* getLevelData - reads in a random word depending on the level
 * Original 29/08/2002 simonb
 */
bool Hangman::getLevelData(unsigned int numChars)
{
   WordList wordList;
   String str;

   std::ifstream words("/usr/share/dict/words");
   if(words.fail())
   {
#ifdef DEBUG
      std::cout << "File /usr/share/dict/words does not exist\n";
#endif
      return false;
   }

   while(!words.eof())
   {
      std::getline(words,str);

      if(words.eof())
         break;
 
      if(str.length() != numChars)
         continue;

      wordList.push_back(str);
   }

   int id = (int)(((float)wordList.size()+1.0) * rand() / RAND_MAX);

#ifdef DEBUG
   std::cerr << "ID = " << id << std::endl;
   std::cerr << "SIZE = " << wordList.size() << std::endl;
#endif
   word = wordList[id].toLower();

   return true;
}

/* parsePLAY - Parse the PLAY command, which registers players
 * Original 29/08/2002 simonb
 */
EXORDI8_FUNC(Hangman::parsePLAY)
{
   // If we are already playing, tell the user they cannot join mid-game
   if (playing) {
      sendMessage(origin,
		  "You cannot join the game in the middle of play, sorry");
      return true; // Keep the game alive
   }
   
   // Make sure there aren't TOO MANY players!
   if (players.size() >= maxPlayers) {
      sendMessage(origin,
		  "Sorry, there are too many players in this game!");
      return true; // Keep the game alive
   }
   
   // Check if the user is already playing the game
   if (isPlaying(origin)) {
      sendMessage(origin, "You are already registered to play the game!");
      return true; // Keep the game alive
   }
   
   // Add them..
   players.push_back(&origin);
   sendMessage(origin,
	       "You are now registered to play, " + 
	       players.front()->getNickname() +
	       " must select a level before game begins.");
   sendMessage(*(players.front()),
	       origin.getNickname() + " has registered to play.");
   
   // If we are at the maximum number of players, tell the dealer
   if (players.size() == maxPlayers) {
      sendMessage(*(players.front()),
		  "You have reached the maximum number of players for "
		  "Hangman - No more players can join the game. You must "
		  "select a level to begin the game.");
   }
   
   return true;
}

/* parseSTART - Parse the START command, which tells the game that all players
 *              are ready to play
 *
 * Original 18/09/2002 josullivan
 */
EXORDI8_FUNC(Hangman::parseSTART)
{
   std::ostringstream out;

   if(playing)
   {
      sendMessage(origin, "The game is already in play!");
      return true;
   }

   int level = atoi(line.rest().c_str());
   if(!(level >= 4 && level <= 14))
   {
      sendMessage(origin, "Invalid level: Must be between 4 and 14");
      return true;
   }

   if(!getLevelData(level))
   {
#ifdef DEBUG
      std::cerr << "Error couldn't load dictionary\n";
#endif
      return false;
   }

   numWrongGuesses = 0;
   wrongGuesses = "";
   correctGuesses = "";
   playing = true;
   currentPlayer = players.begin();

   out << "It's " << (*(*currentPlayer)).getNickname();

   // ... get our grammar right.. :)
   if ((*currentPlayer)->getNickname()
       [(*currentPlayer)->getNickname().length()] == 's') {
      out << "' turn to guess.";
   } else {
      out << "'s turn to guess.";
   }

   showWord(*(*currentPlayer));

   // Send the message..
   sendMessage(out.str());

   return true;
}

/* parseGUESS - Parse the GUESS command, which lets a player guess a character
 *
 * Original 21/09/2002 josullivan
 */
EXORDI8_FUNC(Hangman::parseGUESS)
{
   std::ostringstream out;
   const Exordium::User* player;
   bool isCorrect = false;
   bool isComplete = true;

   String guess = line.rest().toLower();

   if ((player = checkPlayerStatus(origin)) == 0) {
      return true; // Keep the game alive
   }

   if (!playing) {
      sendMessage(origin, players.front()->getNickname() + 
          " hasn't started the game yet");
      return true;
   }

   // Make sure this player is the current player..
   if (player != (*currentPlayer)) {
      sendMessage(origin, "It's not your turn, it's " + 
          (*currentPlayer)->getNickname() + "'s turn");
      return true; // Keep the game alive
   }

   if(guess.length() > 1)
   {
      sendMessage(origin.getNickname() + " is trying to solve with '" + guess + "'");
      if(guess != word)
      {
         isComplete = false;
         numWrongGuesses++;
      }
   }
   else
   {
      int idx = word.find(guess[0]);
#ifdef DEBUG
      std::cout << "Index = " << idx << " of letter " << guess[0] << 
        " in word " << word << std::endl;
#endif

      if(idx < 0)
      {
         wrongGuesses += guess[0];
         isComplete = false;
         numWrongGuesses++;
      }
      else 
      {
         idx = correctGuesses.find(guess); 
         if(idx < 0)
         {
            isCorrect = true;
            correctGuesses += guess[0];

            // Check to see if the word complete
            for(unsigned int i = 0; i < word.length(); i++)
            {
               int idx = correctGuesses.find(word[i]);
               if(idx < 0)
               {
                  isComplete = false;
                  break;
               }
            }
         }
         else
         {
            numWrongGuesses++;
            isComplete = false;
         }
      }
   }

   // If the word is complete exit
   if(isComplete)
   {
      sendMessage(origin.getNickname() + " has won!!");
      sendMessage("The word was '" + word + "'!");
      playing = false;
      return false;
   }

   showWord(*player);

   if(numWrongGuesses >= maxWrongGuesses)
   {
      sendMessage(origin.getNickname() + " has been hung!");
      sendMessage("The word was '" + word + "'!");
      playing = false;
      return false; // End the game!
   }
   
   // If we got here, do a standard message..
   out << "has chosen '" << guess << "'.";

   out << (isCorrect ? " There are " : " Sorry no ") << guess << "'s";

   nextPlayer(out.str());

   return true;
}

/* showWord - displays the word and hangman
 * Original 20/09/2002 josullivan
 */
void Hangman::showWord(const Exordium::User& player)
{
   std::ostringstream out;

   // Needs to be a space else it will be ignored
   sendMessage(" ");
   showHangman(player);

   out << "Word: ";

   for(unsigned int i = 0; i < word.length(); i++)
   {
      int idx = correctGuesses.find(word[i]);
#ifdef DEBUG
      std::cout << "Index = " << idx << " of letter " << word[i] << 
         " in correctGuesses " << correctGuesses << std::endl;
#endif

      if(idx < 0)
      {
         out << "#";
      }
      else
      {
         out << word[i];
      }
   }

   out << "          " << "Letters Used: " << wrongGuesses << correctGuesses;

   sendMessage(out.str());
   sendMessage(" ");
}

/* showHangman - displays the hangman depending on the number of guesses
 * Original 20/09/2002 josullivan
 */
void Hangman::showHangman(const Exordium::User& player)
{
#ifdef DEBUG
   std::cout << "Wrong guesses" << numWrongGuesses << std::endl;
#endif
   if(numWrongGuesses > 0)
   {
      for(int i = 0; i < 7; i++)
      {
         sendMessage(hangman[numWrongGuesses - 1][i]);
      }
   }
}

