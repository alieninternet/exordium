/* $Id$
 * Exordi8 card game
 * 
 * Copyright (c) 2002 Simon Butcher <simonb@alien.net.au>
 *
 * This file is a part of SOME PROJECT.
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

#include <vector>
#include <sstream>

#include "exordi8.h"
#include "pack.h"


/* Exordi8 - Constructor for a new Exordi8 card game being played on a channel
 * Original 29/08/2002 simonb
 */
Exordi8::Exordi8(Game& game, const Kine::String& channel, 
		 const Kine::String& caller)
  : ChannelGame(game, channel.IRCtoLower()),
    playing(false),
    forwardDirection(true),
    sequentTrick(false),
    nextSuit(5) // an invalid suit, on purpose.
{
   // Add the person who fired up the game to the players list
   players.push_back(player_type(caller.IRCtoLower(), Cards::Hand()));
};


// The list of functions - yay.
const Exordi8::functionTable_type Exordi8::functionTable[] =
{
     { "deal",			&Exordi8::parseDEAL },
     { "discard",		&Exordi8::parseDISCARD },
     { "drop",			&Exordi8::parseDISCARD },
     { "hand",			&Exordi8::parseSHOWHAND },
     { "pass",			&Exordi8::parsePASS },
     { "pickup",		&Exordi8::parsePICKUP },
     { "play",			&Exordi8::parsePLAY },
     { "showhand",		&Exordi8::parseSHOWHAND },
     { "skip",			&Exordi8::parsePASS },
     { "stat",			&Exordi8::parseSTATUS },
     { "stats",			&Exordi8::parseSTATUS },
     { "status",		&Exordi8::parseSTATUS },
     { "suit",			&Exordi8::parseSUIT },
     { "take",			&Exordi8::parsePICKUP },
     { 0, 0 }
};


/* parseLine - Parse an incoming command from someone
 * Original 29/08/2002 simonb
 */
bool Exordi8::parseLine(Kine::String& origin, Kine::StringTokens& tokens)
{
   Kine::String command = tokens.nextToken().toLower();
   
   // Run through the command list and find a matching command..
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Is this a match?
      if (command == functionTable[i].command) {
	 // Run the function handler, and leave
	 (this->*(functionTable[i].function))(origin, tokens);
	 break;
      }
   }
   
   // For now, we will just consider that the channel game should remain alive
   return true;
}


/* getPlayer [Various Forms] - Is a specific player playing?
 * Original 29/08/2002 simonb
 */
const Exordi8::player_type* const 
  Exordi8::getPlayer(const Kine::String& player) const
{
   // Run through the list and find this player.. *sigh*
   for (players_type::const_iterator it = players.begin(); it != players.end();
	it++) {
      // Does this match?
      if ((*it).first == player) {
	 return &(*it);
      }
   }
   
   // We could not find that player obviously
   return 0;
}

Exordi8::player_type* const Exordi8::getPlayer(const Kine::String& player)
{
   // Run through the list and find this player.. *sigh*
   for (players_type::iterator it = players.begin(); it != players.end(); 
	it++) {
      // Does this match?
      if ((*it).first == player) {
	 return &(*it);
      }
   }
   
   // We could not find that player obviously
   return 0;
}


/* Check a player's player status (returning their info if possible)
 * Original 30/08/2002 simonb
 */
Exordi8::player_type* const 
  Exordi8::checkPlayerStatus(const Kine::String& player, bool quiet)
{
   // Find the player's info
   player_type* const playerInfo = getPlayer(player);
   
   // Make sure we found something
   if (playerInfo == 0) {
      if (!quiet) {
	 sendMessage(player, "You're not playing Exordi8!");
      }
      return 0;
   }
   
   // Make sure we are playing too
   if (!playing) {
      if (!quiet) {
	 sendMessage(player,
		     "The game isn't being played - " + players.front().first +
		     " must deal the pack out first.");
      }
      return 0;
   }
   
   return playerInfo;
}


/* showHand - Show a player's current hand to them
 * Original 30/08/2002 simonb
 */
void Exordi8::showHand(const player_type& player) const
{
   std::ostringstream out;
   
   out << "You have " << player.second.getCardCount() << 
     " cards in your hand: " << player.second.getHandStr();

   sendMessage(player.first, out.str());
}


/* nextPlayer - Jump to the next player, telling the channel why parse
 * Original 30/08/2002 simonb
 */
void Exordi8::nextPlayer(const Kine::String& why)
{
   std::ostringstream out;
   out << (*currentPlayer).first << ' ' << why << ". It is ";

   // Work out who is to play next..
   if (forwardDirection) {
      // Check if this player is at the end of the players list
      if (currentPlayer == --players.end()) {
	 // Jump to the first player in the list
	 currentPlayer = players.begin();
      } else {
	 // Move on to the next player in the list
	 currentPlayer++;
      }
   } else {
      // Check if this player is at the start of the players list
      if (currentPlayer == players.begin()) {
	 // Jump to the last player in the list
	 currentPlayer = --players.end();
      } else {
	 // Move to the previous player in the list
	 currentPlayer--;
      }
   }

   // Add the current player's name to the string
   out << (*currentPlayer).first;
   
   // ... get our grammar right.. :)
   if ((*currentPlayer).first[(*currentPlayer).first.length() - 1] == 's') {
      out << "' turn.";
   } else {
      out << "'s turn.";
   }
   
   // Tell the channel what is happening
   sendMessage(out.str());
   
   /* Show the new player their hand, since it's more than likely that they
    * need refreshing - especially in games with large numbers of players.
    */
   showHand(*currentPlayer);
}


/* parseDEAL - Parse the DEAL command, which starts the game by dealing cards
 * Original 29/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parseDEAL)
{
   // Only the dealer can deal (the first player in the players list)
   if (origin.IRCtoLower() != players.front().first) {
      sendMessage(origin,
		  "Only " + players.front().first + " can deal, since " +
		  players.front().first + " is the dealer!!");
      return;
   }
   
   // Make sure we aren't already playing
   if (playing) {
      sendMessage(origin,
		  "The game is already in play, there is no point in dealing "
		  "again!");
      return;
   }
   
   // Check there are enough players..
   if (players.size() < 2) {
      sendMessage(origin,
		  "There aren't enough players - you need at least two "
		  "players to play Exordi8.");
      return;
   }
   
   // Okay!! Let's start playing!
   sendMessage(players.front().first + " is now dealing...");
   
   // Work out how many packs we need..
   players_type::size_type numPlayers = players.size();
   unsigned int numPacks = 1;
   while (numPlayers >= 6) {
      numPacks++;
      numPlayers -= 6;
   };

   // Obtain our packs, shuffling each deck. 
   std::vector <Cards::Pack> packs;
   while (numPacks > 0) {
      packs.push_back(Cards::Pack());
      packs.back().shuffle();
      numPacks--;
   }
   
   // Deal five cards to each person, doing it like a real dealer (hehehe)
   for (int c = 0; c != 5; c++) {
      for (players_type::iterator p = players.begin();
	   p != players.end(); p++) {
	 // Add a card to this player's hand direct from the pack
	 (*p).second.addCard(packs.back().removeCard());
	 
	 // If this pack is now empty, ditch it and move onto the next one
	 if (packs.back().isEmpty()) {
	    packs.pop_back();
	 }
      }
   }

   // Show the players their hands
   for (players_type::const_iterator it = players.begin(); it != players.end();
	it++) {
      showHand((*it));
   }

   // The balance of the cards left in the shuffled pack(s) turn into the stock
   while (!packs.empty()) {
      while (!packs.back().isEmpty()) {
	 stock.push(packs.back().removeCard());
      }
      
      // Turf the empty pack
      packs.pop_back();
   }
   
   // Set the first player and note that we have begun play
   currentPlayer = players.begin();
   playing = true;

   // Start assembling a string to tell the channel what is happening
   std::ostringstream out;
   out << "There are " << stock.size() << " cards left in the stock. It is " <<
     (*currentPlayer).first;
   
   // ... get our grammar right.. :)
   if ((*currentPlayer).first[(*currentPlayer).first.length()] == 's') {
      out << "' turn.";
   } else {
      out << "'s turn.";
   }

   // Send the message..
   sendMessage(out.str());
}


/* parseDISCARD - Discard a card..
 * Original 30/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parseDISCARD)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin.IRCtoLower())) == 0) {
      return;
   }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer)) {
      sendMessage(origin, 
		  "It's not your turn, it's " + (*currentPlayer).first +
		  "'s turn");
      return;
   }

   /* Make sure this player has not already put down a card (an 8 to be
    * specific) since we would now be waiting for them to tell us what
    * suit to use..
    */
   if ((lastDiscardedCard.getIndex() == 8) && (nextSuit == 0)) {
      sendMessage(origin,
		  "You cannot drop another card - you must now select a "
		  "suit for the next player to play.");
      return;
   }
   
   // Okay, try to make a card which matches their description..
   Cards::Card cardToDiscard(line.rest());
   
   // Make sure that worked
   if (!cardToDiscard.isValid()) {
      sendMessage(origin, "I don't know what card you mean");
      return;
   }
   
   // Okay, make sure the player actually HAS this card first..
   if (!(*currentPlayer).second.hasCard(cardToDiscard)) {
      sendMessage(origin,
		  Kine::String("You don't have a ") + 
		  cardToDiscard.getName() + " to discard!");
      return;
   }
   
   /* Make sure they are able to drop this card following the rules. Any
    * card can be placed down on the first move, so we can skip it if that
    * is the case.
    */
   if (lastDiscardedCard.isValid()) {
      // If the queen of spades was the last discarded, skip the check
      if (!((lastDiscardedCard.getSuit() == Cards::Card::Suit::Spades) &&
	    (lastDiscardedCard.getIndex() == Cards::Card::Rank::Queen))) {
	 /* If the last card was an '8' and there is a suit selected, we must
	  * use it..
	  */
	 if (lastDiscardedCard.getIndex() == 8) {
	    /* Check the suits match. Note, if the stock is empty, they can
	     * drop any card they like.
	     */
	    if ((nextSuit != cardToDiscard.getSuit()) &&
		!stock.empty()) {
	       sendMessage(origin, 
			   Kine::String("You must discard a card that matches "
					"the suit of the last discarded "
					"card (") +
			   lastDiscardedCard.getName() + ") or take a card.");
	       return;
	    } else {
	       // Check the suits OR the ranks match
	       if (!((cardToDiscard.getSuit() == 
		     lastDiscardedCard.getSuit()) ||
		    (cardToDiscard.getIndex() == 
		     lastDiscardedCard.getIndex())) ||
		   !((lastDiscardedCard.getIndex() == 
		      Cards::Card::Rank::Jack) &&
		     (cardToDiscard.getColour() == 
		      lastDiscardedCard.getColour()))) {
		  sendMessage(origin,
			      Kine::String("You must match either the suit or "
					   "the rank of the last discarded "
					   "card (") + 
			      lastDiscardedCard.getName() + 
			      (stock.empty() ? 
			       ") or drop a card" : ") or take a card."));
		  return;
	       }
	    }
	 }
      }
   } else {
      // However, if this IS the first play, the queen of hearts cannot be used
      if ((cardToDiscard.getSuit() == Cards::Card::Suit::Hearts) &&
	  (cardToDiscard.getIndex() == Cards::Card::Rank::Queen)) {
	 sendMessage(origin, 
		     "You cannot discard the Queen of Hearts on the game's "
		     "first move, sorry.");
	 return;
      }
   }
   
   // Is trumping okay?
   bool trumpingOkay = true;
   
   // If the queen of spades was on the top of the discard pile, devalue trumps
   if ((cardToDiscard.getSuit() == Cards::Card::Suit::Spades) &&
       (cardToDiscard.getIndex() == Cards::Card::Rank::Queen)) {
      trumpingOkay = false;
   }

   if (trumpingOkay) {
      if (sequentTrick) {
	 // Make sure the last card is ONE more than this one
	 if (lastDiscardedCard.getIndex() != (cardToDiscard.getIndex() - 1)) {
	    sequentTrick = false;
	    trumpingOkay = false;
	    sendMessage(origin + " has broken the sequent!");
	    
	    // Make them pick up two cards
	    if (!stock.empty()) {
	       (*currentPlayer).second.addCard(stock.top());
	       stock.pop();
	       if (!stock.empty()) {
		  (*currentPlayer).second.addCard(stock.top());
		  stock.pop();
	       }
	    }
	    
	    // Show them their hand..
	    showHand(*currentPlayer);
	 } 
      } else {
	 // If the last card was a 2, and this card is a 3 - trigger a sequent
	 if ((lastDiscardedCard.getIndex() == 2) &&
	     (cardToDiscard.getIndex() == 3)) {
	    sequentTrick = true;
	    trumpingOkay = false;
	    sendMessage(origin + " has triggered a sequent!");
	 }
      }
   }
   
   // Discard the card..
   lastDiscardedCard = ((*currentPlayer).second.removeCard(cardToDiscard));

   // Hold on a second.. check if this player is the winner..
   if ((*currentPlayer).second.isEmpty()) {
      sendMessage(origin + " has no more cards left, and is the winner!!");
      playing = false;
      return;
   }
   
   // If trump cards still exist, do their thing..
   if (trumpingOkay) {
      // If the card is an '8', reset the suit and ask them to specify one.
      if (cardToDiscard.getIndex() == 8) {
	 nextSuit = 0;
	 
	 // Tell everyone what happened.. (notice - no next player)
	 sendMessage(origin + " has discarded the " + cardToDiscard.getName() +
		     '.');

	 // Tell the player what happens next
	 sendMessage(origin, 
		     "You have discarded an 8, you must specify which suit "
		     "the next player must discard.");
	 
	 return;
      }
   
      // If the card is an ace, reverse the play.. if it matters..
      if ((players.size() > 2) &&
	  (cardToDiscard.getIndex() == Cards::Card::Rank::Ace)) {
	 forwardDirection = !forwardDirection;
	 nextPlayer(Kine::String(" has discarded the ") + 
		    cardToDiscard.getName() +
		    " - the direction of play has been reversed");
	 return;
      }
      
      // If the card is the queen of hearts, the player gets another go.
      if ((cardToDiscard.getSuit() == Cards::Card::Suit::Hearts) &&
	  (cardToDiscard.getIndex() == Cards::Card::Rank::Queen)) {
	 sendMessage(origin + " has discarded the " + cardToDiscard.getName() +
		     " and gets another turn.");
	 return;
      }
      
      // If we are in a sequent and this card is the '8', reward the player
      if (sequentTrick && (cardToDiscard.getIndex() == 8)) {
	 sequentTrick = false;
	 sendMessage(origin + " has completed the sequent and gets another "
		     "turn");
	 return;
      }
   }

   // If we got here, do a standard message..
   nextPlayer(Kine::String("has discarded the ") + cardToDiscard.getName());
}


/* parsePASS - Skip/Pass a move (only when the stock is depleted)
 * Original 30/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parsePASS)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin.IRCtoLower())) == 0) {
      return;
   }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer)) {
      sendMessage(origin, 
		  "It's not your turn, it's " + (*currentPlayer).first +
		  "'s turn");
      return;
   }
   
   // Make sure the stock isn't empty
   if (!stock.empty()) {
      sendMessage(origin,
		  "You cannot pass a play while the stock is not empty. If "
		  "you cannot discard a card, you must take a card from the "
		  "stock pile.");
      return;
   }
   
   // Move to the next player (pass the move)
   nextPlayer("passes");
}


/* parsePICKUP - Pick-up a card
 * Original 30/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parsePICKUP)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin.IRCtoLower())) == 0) {
      return;
   }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer)) {
      sendMessage(origin, 
		  "It's not your turn, it's " + (*currentPlayer).first +
		  "'s turn");
      return;
   }
   
   // Make sure the stock isn't empty
   if (stock.empty()) {
      sendMessage(origin,
		  "The stock is empty, you can't pick up a card. If you "
		  "cannot discard a card, you may pass");
      return;
   }
   
   // Okay, it must be the current player.. Pick up a card for them
   sendMessage(origin, Kine::String("You picked up ") + stock.top().getName());
   (*currentPlayer).second.addCard(stock.top());
   stock.pop();
   
   // Move to the next player
   if (stock.empty()) {
      nextPlayer("picks up a card. The stock is now empty");
   } else {
      nextPlayer("picks up a card");
   }
}


/* parsePLAY - Parse the PLAY command, which registers players
 * Original 29/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parsePLAY)
{
   // If we are already playing, tell the user they cannot join mid-game
   if (playing) {
      sendMessage(origin,
		  "You cannot join the game in the middle of play, sorry");
      return;
   }
   
   // Make sure there aren't TOO MANY players!
   if (players.size() >= maxPlayers) {
      sendMessage(origin,
		  "Sorry, there are too many players in this game!");
      return;
   }
   
   // Convert the nickname over to be more usable
   Kine::String nick = origin.IRCtoLower();
   
   // Check if the user is already playing the game
   if (isPlaying(nick)) {
      sendMessage(origin, "You are already registered to play the game!");
      return;
   }
   
   // Add them..
   players.push_back(player_type(nick, Cards::Hand()));
   sendMessage(origin,
	       "You are now registered to play, " + players.front().first + 
	       " must deal before the game begins.");
   
   // If we are at the maximum number of players, tell the dealer
   if (players.size() == maxPlayers) {
      sendMessage(players.front().first,
		  "You have reached the maximum number of players for "
		  "Exordi8 - No more players can join the game. You must "
		  "deal the cards now to begin the game.");
   }
}


/* parseSHOWHAND - Show a player their hand
 * Original 30/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parseSHOWHAND)
{
   if (!playing) {
      sendMessage(origin, 
		  players.front().first + " needs to deal before you have a "
		  "hand to look at!");
      return;
   }
   
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin.IRCtoLower())) == 0) {
      return;
   }
   
   // OKAY... now show the player their cards
   showHand(*player);
}


/* parseSTATUS - Show a player information about the current game
 * Original 30/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parseSTATUS)
{
   // If we are not playing, we cannot be very handy
   if (!playing) {
      sendMessage(origin,
		  "The game has not begun yet - " + players.front().first +
		  " must deal to begin the game.");
      return;
   }
   
   // Grab the player's info
   const player_type* player = checkPlayerStatus(origin.IRCtoLower(), true);
   
   std::ostringstream out;
   
   // Run through the list of players and give them vague information..
   for (players_type::iterator p = players.begin();
	p != players.end(); p++) {
      // Make sure this is not the current player
      if (&(*p) != player) {
	 // Do we need to add a formatting (for between players)
	 if (!out.str().empty()) {
	    out << "; ";
	 }
	 
	 // Add the info
	 out << (*p).first << ": " << (*p).second.getCardCount() << " cards";
	 
	 // If the line is too long, send what we have and clear it
	 if (out.str().length() > 300) {
	    sendMessage(origin, out.str());
	    out.str() = "";
	 }
      }
   }
   
   // If there is anything left to send, send it
   if (!out.str().empty()) {
      sendMessage(origin, out.str());
      out.str() = "";
   }
   
   // If this person is a player, show their current hand as well
   if (player != 0) {
      showHand(*player);
   }
   
   // Tell them a bit more information
   out << "It's " << (*currentPlayer).first;
   if ((*currentPlayer).first[(*currentPlayer).first.length() - 1] == 's') {
      out << "' turn. ";
   } else {
      out << "'s turn. ";
   }
   
   if (stock.empty()) {
      out << "The stock is empty.";
   } else {
      out << "There are " << stock.size() << " cards left in the stock.";
   }

   // Send the last bit of information..
   sendMessage(origin, out.str());
}


/* parseSUIT - Allow a user to select a suit (if they have discarded an 8)
 * Original 31/08/2002 simonb
 */
EXORDI8_FUNC(Exordi8::parseSUIT)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin.IRCtoLower())) == 0) {
      return;
   }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer)) {
      sendMessage(origin, 
		  "It's not your turn, it's " + (*currentPlayer).first +
		  "'s turn");
      return;
   }
   
   // Make sure an 8 has been dropped
   if ((lastDiscardedCard.getIndex() != 8) || (nextSuit != 0)) {
      sendMessage(origin,
		  "You need to discard a card with a face-value of '8' before "
		  "you can select a suit for the next player to play");
      return;
   }
   
   // Okay, try to comprehend what suit they wanted
   if ((nextSuit = Cards::Card::findSuit(line.nextToken())) == 0) {
      sendMessage(origin,
		  "I don't know what suit you meant. It must be either "
		  "Clubs, Spades, Hearts or Diamonds.");
      return;
   }
   
   // Next player!!
   nextPlayer("has selected the next suit after discarding an '8'");
   
   // Tell the next player which suit they need..
   sendMessage((*currentPlayer).first,
	       Kine::String("You need to use a card of the suit ") +
	       Cards::Card::nameSuit(nextSuit) + ", or take a card.");
}
