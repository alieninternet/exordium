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

#include <vector>
#include <sstream>

#include "hangman.h"

using LibAIS::String;
using LibAIS::StringTokens;

/* Hangman - Constructor for a new Hangman game being played on a channel
 * Original 17/09/2002 josullivan
 */
Hangman::Hangman(Game::Module& module, const String& channel, 
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
   for (players_type::const_iterator it = players.begin(); it != players.end();
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
   
   /*
   // Make sure we are playing too
   if (!playing) {
      if (!quiet) {
	 sendMessage(player,
		     "The game isn't being played - " + 
		     players.front().first->getNickname() +
		     " must deal the pack out first.");
      }
      return 0;
   }
   */
   
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
      out << "' turn.";
   } else {
      out << "'s turn.";
   }
   
   // Tell the channel what is happening
   sendMessage(out.str());
   
   /* Show the new player their hand, since it's more than likely that they
    * need refreshing - especially in games with large numbers of players.
    */
   /*
   showHand(*currentPlayer);

   if (withMatchNotify) {
      // Tell the player what they need to get..
      if ((lastDiscardedCard.getIndex() == 8) && (nextSuit != 0)) {
	 sendMessage(*((*currentPlayer).first), 
		     String("To discard, you need to put down a card of "
				  "the ") + Cards::Card::nameSuit(nextSuit) + 
				  " suit");
      } else if ((lastDiscardedCard.getSuit() == Cards::Card::Suit::Spades) &&
		 (lastDiscardedCard.getIndex() == Cards::Card::Rank::Queen)) {
	 sendMessage(*((*currentPlayer).first),
		     "You can put down any card you like, since the last "
		     "card discarded was the Queen of Spades");
      } else if (lastDiscardedCard.getIndex() == Cards::Card::Rank::Jack) {
	 sendMessage(*((*currentPlayer).first), 
		     String("To discard, you need to match the colour "
				  "of the last card discarded (") +
		     lastDiscardedCard.getColourName() + ")");
      } else {
	 sendMessage(*((*currentPlayer).first),
		     String("To discard, you need to match the ") +
		     lastDiscardedCard.getName());
      }
   }
   */
}

/* getLevelData - reads in a random word depending on the level
 * Original 29/08/2002 simonb
 */
bool Hangman::getLevelData(int level)
{
   String str;
   int numchars = 0;

   std::ifstream words("/usr/share/dict/words");
   if(words.fail())
   {
      std::cout << "File /usr/share/dict/words does not exist\n";
      return false;
   }

   numchars = level + 3;

   while(!words.eof())
   {
      std::getline(words,str);

      if(words.eof())
         break;
 
      if(str.length() != numchars)
         continue;

      // This will eventually be a random selection
      word = str;
   }

   std::transform(word.begin(), word.end(), word.begin(), tolower);

//   for(int i = 0; i < tmpWord.length(); i++)
//   {
//      letterMap.insert(LetterMap::value_type(word[i], false)).second;
//   }
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
   if(!(level >= 1 && level <= 10))
   {
      sendMessage(origin, "Invalid level: Must be between 1 and 10");
      return true;
   }

   getLevelData(level);

   wrongGuesses = "";
   playing = true;
   currentPlayer = players.begin();

   out << (*(*currentPlayer)).getNickname();

   // ... get our grammar right.. :)
   if ((*currentPlayer)->getNickname()
       [(*currentPlayer)->getNickname().length()] == 's') {
      out << "' turn.";
   } else {
      out << "'s turn.";
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
   const Exordium::User* player;
   String guess = line.rest();

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
      sendMessage(origin, "Try again.. your guess be only one character");
      return true;
   }

   int idx = word.find(guess[0]);
   std::cout << "Index = " << idx << " of letter " << guess[0] << " in word " 
      << word << std::endl;

   if(idx < 0)
   {
      sendMessage(origin, "The word doesn't contain the letter ");// + guess[0]);
      wrongGuesses += guess[0];
   }
   else 
   {
      if(wrongGuesses.find(guess[0]) < 0)
      {
         sendMessage(origin, "That letter has already been guessed");
      }              
      else
      {
         sendMessage(origin, "Good guess");
         correctGuesses += guess[0];
      }
   }

//   letterMap.insert(LetterMap::value_type(guess[0], true)).second;

   showWord(*player);

   if(wrongGuesses.length() > maxWrongGuesses)
   {
      sendMessage(origin.getNickname() + " has been hung!");
      playing = false;
      return false; // End the game!
   }

   // If we got here, do a standard message..
   nextPlayer(String("has chosen '") + guess + String("'"));

   return true;
}

/* showWord - displays the word and hangman
 * Original 20/09/2002 josullivan
 */
void Hangman::showWord(const Exordium::User& player)
{
   std::ostringstream out;

   for(int i = 0; i < word.length(); i++)
   {
      int idx = correctGuesses.find(word[i]);
      std::cout << "Index = " << idx << " of letter " << word[i] << 
         " in correctGuesses " << correctGuesses << std::endl;

      out << "Word = ";

      if(idx < 0)
      {
         out << "X";
      }
      else
      {
         out << word[i];
      }
   }

   showHangman(player);
   sendMessage(out.str());
}

/* showHangman - displays the hangman depending on the number of guesses
 * Original 20/09/2002 josullivan
 */
void Hangman::showHangman(const Exordium::User& player)
{
   std::cout << "Wrong guesses" << wrongGuesses.length() << std::endl;
}
