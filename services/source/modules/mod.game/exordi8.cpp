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

#include "exordi8.h"

using AIS::Util::String;
using AIS::Util::StringTokens;

CHANNEL_GAME_CREATOR_FUNCTION
{
  return new Exordi8(module, channel, caller);
}

/* Exordi8 - Constructor for a new Exordi8 card game being played on a channel
 * Original 29/08/2002 pickle
 */
Exordi8::Exordi8(Exordium::GameModule::Module& module, const Kine::Name& channel,
		 Exordium::User& caller)
  : ChannelGame(module, channel.IRCtoLower()),
playing(false),
forwardDirection(true),
sequentTrick(false),
nextSuit(5) // an invalid suit, on purpose.
{
   // Add the person who fired up the game to the players list
   players.push_back(player_type(&caller, Cards::Hand()));
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
 * Original 29/08/2002 pickle
 */
bool Exordi8::parseLine(Exordium::User& origin, String& command,
			StringTokens& tokens)
{
   // Run through the command list and find a matching command..
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Is this a match?
	if (command == functionTable[i].command)
	  {
	     // Run the function handler, and leave
	     return (this->*(functionTable[i].function))(origin, tokens);
	     break;
	  }
     }

   // For now, we will just consider that the channel game should remain alive
   return true;
}

/* getPlayer [Various Forms] - Find a player
 * Original 29/08/2002 pickle
 * Note: Could be more efficient, but I'm a little too lazy at the moment
 */
const Exordi8::player_type* const
  Exordi8::getPlayer(const Exordium::User& player) const
{
   // Run through the list and find this player.. *sigh*
   for (players_type::const_iterator it = players.begin(); it !=
	players.end();
	it++)
     {
	// Does this match?
	if (*((*it).first) == player)
	  {
	     return &(*it);
	  }
     }

   // We could not find that player obviously
   return 0;
}

Exordi8::player_type* const Exordi8::getPlayer(const Exordium::User& player)
{
   // Run through the list and find this player.. *sigh*
   for (players_type::iterator it = players.begin(); it != players.end();
	it++)
     {
	// Does this match?
	if (*((*it).first) == player)
	  {
	     return &(*it);
	  }
     }

   // We could not find that player obviously
   return 0;
}

/* Check a player's player status (returning their info if possible)
 * Original 30/08/2002 pickle
 */
Exordi8::player_type* const
  Exordi8::checkPlayerStatus(Exordium::User& player, bool quiet)
{
   // Find the player's info
   player_type* const playerInfo = getPlayer(player);

   // Make sure we found something
   if (playerInfo == 0)
     {
	if (!quiet)
	  {
	     sendMessage(player, "You're not playing Exordi8!");
	  }
	return 0;
     }

   // Make sure we are playing too
   if (!playing)
     {
	if (!quiet)
	  {
	     sendMessage(player,
			 "The game isn't being played - " +
			 players.front().first->getNickname() +
			 " must deal the pack out first.");
	  }
	return 0;
     }

   return playerInfo;
}

/* showHand - Show a player's current hand to them
 * Original 30/08/2002 pickle
 */
void Exordi8::showHand(const player_type& player) const
{
   std::ostringstream out;

   out << "You have " << player.second.getCardCount() <<
     " cards in your hand: " << player.second.getHandStr();

   sendMessage(*player.first, out.str());
}

/* nextPlayer - Jump to the next player, telling the channel why parse
 * Original 30/08/2002 pickle
 */
void Exordi8::nextPlayer(const String& why, bool withMatchNotify)
{
   std::ostringstream out;
   out << (*(*currentPlayer).first).getNickname() << ' ' << why << ". It is ";

   // Work out who is to play next..
   if (forwardDirection)
     {
	// Check if this player is at the end of the players list
	if (currentPlayer == --players.end())
	  {
	     // Jump to the first player in the list
	     currentPlayer = players.begin();
	  }
	else
	  {
	     // Move on to the next player in the list
	     currentPlayer++;
	  }
     }
   else
     {
	// Check if this player is at the start of the players list
	if (currentPlayer == players.begin())
	  {
	     // Jump to the last player in the list
	     currentPlayer = --players.end();
	  }
	else
	  {
	     // Move to the previous player in the list
	     currentPlayer--;
	  }
     }

   // Add the current player's name to the string
   out << (*(*currentPlayer).first).getNickname();

   // ... get our grammar right.. :)
   if ((*currentPlayer).first->getNickname()
       [(*currentPlayer).first->getNickname().length() - 1] == 's')
     {
	out << "' turn.";
     }
   else
     {
	out << "'s turn.";
     }

   // Tell the channel what is happening
   sendMessage(out.str());

   /* Show the new player their hand, since it's more than likely that they
    * need refreshing - especially in games with large numbers of players.
    */
   showHand(*currentPlayer);

   if (withMatchNotify)
     {
	// Tell the player what they need to get..
	if ((lastDiscardedCard.getIndex() == 8) && (nextSuit != 0))
	  {
	     sendMessage(*((*currentPlayer).first),
			 String("To discard, you need to put down a card of "
				"the ") + Cards::Card::nameSuit(nextSuit) +
			 " suit");
	  }
	else if ((lastDiscardedCard.getSuit() == Cards::Card::Suit::Spades) &&
		 (lastDiscardedCard.getIndex() == Cards::Card::Rank::Queen))
	  {
	     sendMessage(*((*currentPlayer).first),
			 "You can put down any card you like, since the last "
			 "card discarded was the Queen of Spades");
	  }
	else if (lastDiscardedCard.getIndex() == Cards::Card::Rank::Jack)
	  {
	     sendMessage(*((*currentPlayer).first),
			 String("To discard, you need to match the colour "
				"of the last card discarded (") +
			 lastDiscardedCard.getColourName() + ")");
	  }
	else
	  {
	     sendMessage(*((*currentPlayer).first),
			 String("To discard, you need to match the ") +
			 lastDiscardedCard.getName());
	  }
     }
}

/* parseDEAL - Parse the DEAL command, which starts the game by dealing cards
 * Original 29/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parseDEAL)
{
   // Only the dealer can deal (the first player in the players list)
   //   if (origin != *(players.front().first)) {
   //      sendMessage(origin,
   //		  "Only " + players.front().first->getNickname() +
   //		  " can deal, since " +  players.front().first->getNickname()
   //
   //		  " is the dealer!!");
   //      return true; // Keep the game alive
   //   }
   //
   // Make sure we aren't already playing
   if (playing)
     {
	sendMessage(origin,
		    "The game is already in play, there is no point in dealing "
		    "again!");
	return true; // Keep the game alive
     }

   // Check there are enough players..
   if (players.size() < 2)
     {
	sendMessage(origin,
		    "There aren't enough players - you need at least two "
		    "players to play Exordi8.");
	return true; // Keep the game alive
     }

   // Okay!! Let's start playing!
   sendMessage(players.front().first->getNickname() + " is now dealing...");

   // Work out how many packs we need..
   players_type::size_type numPlayers = players.size();
   unsigned int numPacks = 1;
   while (numPlayers >= 6)
     {
	numPacks++;
	numPlayers -= 6;
     };

   // Obtain our packs
   stock = Cards::Stock< Cards::Card >(numPacks);
   stock.shuffle();

   // Deal five cards to each person, doing it like a real dealer (hehehe)
   for (int c = 0; c != 5; c++)
     {
	for (players_type::iterator p = players.begin();
	     p != players.end(); p++)
	  {
	     // Add a card to this player's hand direct from the pack
	     (*p).second.addCard(stock.removeCard());
	  }
     }

   // Show the players their hands
   for (players_type::const_iterator it = players.begin(); it !=
	players.end();
	it++)
     {
	showHand((*it));
     }

   // Set the first player and note that we have begun play
   currentPlayer = players.begin();
   playing = true;

   // Start assembling a string to tell the channel what is happening
   std::ostringstream out;
   out << "There are " << stock.total() << " cards left in the stock. It is "
     <<
     (*(*currentPlayer).first).getNickname();

   // ... get our grammar right.. :)
   if ((*currentPlayer).first->getNickname()
       [(*currentPlayer).first->getNickname().length()] == 's')
     {
	out << "' turn.";
     }
   else
     {
	out << "'s turn.";
     }

   // Send the message..
   sendMessage(out.str());

   return true;
}

/* parseDISCARD - Discard a card..
 * Original 30/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parseDISCARD)
{
   Cards::Card cardToDiscard;

   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin)) == 0)
     {
	return true; // Keep the game alive
     }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer))
     {
	sendMessage(origin,
		    "It's not your turn, it's " +
		    (*currentPlayer).first->getNickname() + "'s turn");
	return true; // Keep the game alive
     }

   /* Make sure this player has not already put down a card (an 8 to be
    * specific) since we would now be waiting for them to tell us what
    * suit to use..
    */
   if ((lastDiscardedCard.getIndex() == 8) && (nextSuit == 0))
     {
	sendMessage(origin,
		    "You cannot drop another card - you must now select a "
		    "suit for the next player to play.");
	return true; // Keep the game alive
     }

   // Okay, try to make a card which matches their description or position num
   String identifier = line.nextToken();

   if(line.countTokens() > 1 && (line.nextToken() == "of"))
   {
      cardToDiscard = Cards::Card(identifier + " of " + line.rest());
   }
   else 
   {
      unsigned int idx = identifier.toInt();
      if(idx <= 0)
      {
	 sendMessage(origin, "You must select a number higher than 0");
	 return true; // Keep the game alive
      }
      
      // Reduce by one since we count from zero not one
      idx--;
      cardToDiscard = (*currentPlayer).second.getCardAtIndex(idx);
   }

   // Make sure that worked
   if (!cardToDiscard.isValid())
     {
	sendMessage(origin, "I don't know what card you mean");
	return true; // Keep the game alive
     }

   // Okay, make sure the player actually HAS this card first..
   if (!(*currentPlayer).second.hasCard(cardToDiscard))
     {
	sendMessage(origin,
		    String("You don't have a ") +
		    cardToDiscard.getName() + " to discard!");
	return true; // Keep the game alive
     }

   /* Make sure they are able to drop this card following the rules. Any
    * card can be placed down on the first move, so we can skip it if that
    * is the case.
    */
   if (lastDiscardedCard.isValid())
     {
      /* If the queen of spades was the last discarded, skip the check since
       * anything is allowed to be put on the queen of spades
       */
	if (!((lastDiscardedCard.getSuit() == Cards::Card::Suit::Spades) &&
	      (lastDiscardedCard.getIndex() == Cards::Card::Rank::Queen)))
	  {
	 /* If the last card was an '8' and there is a suit selected, we must
	  * use it..
	  */
	     if ((lastDiscardedCard.getIndex() == 8) && (nextSuit != 0))
	       {
	    /* Check the suits match. Note, if the stock is empty, they can
	     * drop any card they like.
	     */
		  if ((nextSuit != cardToDiscard.getSuit()) && stock.total())
		    {
		       sendMessage(origin,
				   String("You must discard a card that matches "
					  "the suit selected by the last player "
					  "(") +
				   Cards::Card::nameSuit(nextSuit) +
				   ") or take a card.");
		       return true; // Keep the game alive
		    }
	       }
	     else
	       {
	    /* Check the suits OR the ranks match. However, if the card is
	     * a jack (of any suit) then we really only need to check the
	     * colours..
	     */
		  if (!((cardToDiscard.getSuit() == lastDiscardedCard.getSuit()) ||
			(cardToDiscard.getIndex() == lastDiscardedCard.getIndex()))
		      &&
		      !((lastDiscardedCard.getIndex() == Cards::Card::Rank::Jack) &&
			(cardToDiscard.getColour() ==
			 lastDiscardedCard.getColour())))
		    {
		       sendMessage(origin,
				   String("You must match either the suit or "
					  "the rank of the last discarded "
					  "card (") +
				   lastDiscardedCard.getName() +
				   (!stock.total() ?
				    ") or drop a card" : ") or take a card."));
		       return true; // Keep the game alive
		    }
	       }
	  }
     }
   else
     {
	// However, if this IS the first play, the queen of hearts cannot beused
	if ((cardToDiscard.getSuit() == Cards::Card::Suit::Hearts) &&
	    (cardToDiscard.getIndex() == Cards::Card::Rank::Queen))
	  {
	     sendMessage(origin,
			 "You cannot discard the Queen of Hearts on the game's "
			 "first move, sorry.");
	     return true; // Keep the game alive
	  }
     }

   // Is trumping okay?
   bool trumpingOkay = true;

   // If the queen of spades was on the top of the discard pile, devalue   trumps
     if ((cardToDiscard.getSuit() == Cards::Card::Suit::Spades) &&
	 (cardToDiscard.getIndex() == Cards::Card::Rank::Queen))
       {
	  trumpingOkay = false;
       }

   if (trumpingOkay)
     {
	if (sequentTrick)
	  {
	     // Make sure the last card is ONE more than this one
	     if (lastDiscardedCard.getIndex() != (cardToDiscard.getIndex() - 1))
	       {
		  sequentTrick = false;
		  trumpingOkay = false;
		  sendMessage(origin.getNickname() + " has broken the sequent!");

		  // Make them pick up two cards
                  (*currentPlayer).second.addCard(stock.removeCard());
                  (*currentPlayer).second.addCard(stock.removeCard());
		  showHand(*currentPlayer);
	       }
	  }
	else
	  {
	     // If the last card was a 2, and this card is a 3 - trigger a sequent
	     if ((lastDiscardedCard.getIndex() == 2) &&
		 (cardToDiscard.getIndex() == 3))
	       {
		  sequentTrick = true;
		  trumpingOkay = false;
		  sendMessage(origin.getNickname() + " has triggered a sequent!");
	       }
	  }
     }

   // Discard the card..
   lastDiscardedCard = ((*currentPlayer).second.removeCard(cardToDiscard));
   discard.addCard(lastDiscardedCard);

   // Hold on a second.. check if this player is the winner..
   if ((*currentPlayer).second.isEmpty())
     {
	sendMessage(origin.getNickname() +
		    " has no more cards left, and is the winner!!");
	playing = false;
	return false; // End the game!
     }

   // If trump cards still exist, do their thing..
   if (trumpingOkay)
     {
	// If the card is an '8', reset the suit and ask them to specify one.
	if (cardToDiscard.getIndex() == 8)
	  {
	     nextSuit = 0;

	     // Tell everyone what happened.. (notice - no next player)
	     sendMessage(origin.getNickname() + " has discarded the " +
			 cardToDiscard.getName() +
			 ", and now needs to select a suit.");

	     // Tell the player what happens next
	     sendMessage(origin,
			 "You have discarded an 8, you must specify which suit "
			 "the next player must discard.");

	     return true; // Keep the game alive
	  }

	// If the card is an ace, reverse the play.. if it matters..
	if ((players.size() > 2) &&
	    (cardToDiscard.getIndex() == Cards::Card::Rank::Ace))
	  {
	     forwardDirection = !forwardDirection;
	     nextPlayer(String("has discarded the ") +
			cardToDiscard.getName() +
			" - the direction of play has been reversed");
	     return true; // Keep the game alive
	  }

	// If the card is the queen of hearts, the player gets another go.
	if ((cardToDiscard.getSuit() == Cards::Card::Suit::Hearts) &&
	    (cardToDiscard.getIndex() == Cards::Card::Rank::Queen))
	  {
	     sendMessage(origin.getNickname() + " has discarded the " +
			 cardToDiscard.getName() + " and gets another turn.");
	     return true; // Keep the game alive
	  }

	// If we are in a sequent and this card is the '8', reward the player
	if (sequentTrick && (cardToDiscard.getIndex() == 8))
	  {
	     sequentTrick = false;
	     sendMessage(origin.getNickname() +
			 " has completed the sequent and gets another turn");
	     return true; // Keep the game alive
	  }
     }

   // If we got here, do a standard message..
   nextPlayer(String("has discarded the ") + cardToDiscard.getName());

   return true;
}

/* parsePASS - Skip/Pass a move (only when the stock is depleted)
 * Original 30/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parsePASS)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin)) == 0)
     {
	return true; // Keep the game alive
     }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer))
     {
	sendMessage(origin,
		    "It's not your turn, it's " +
		    (*currentPlayer).first->getNickname() + "'s turn");
	return true; // Keep the game alive
     }

   // Make sure the stock isn't empty
   if (!stock.total())
     {
	sendMessage(origin,
		    "You cannot pass a play while the stock is not empty. If "
		    "you cannot discard a card, you must take a card from the "
		    "stock pile.");
	return true; // Keep the game alive
     }

   // Move to the next player (pass the move)
   nextPlayer("passes");

   return true;
}

/* parsePICKUP - Pick-up a card
 * Original 30/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parsePICKUP)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin)) == 0)
     {
	return true; // Keep the game alive
     }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer))
     {
	sendMessage(origin,
		    "It's not your turn, it's " +
		    (*currentPlayer).first->getNickname() + "'s turn");
	return true; // Keep the game alive
     }

   // Make sure the stock isn't empty
   if (!stock.total())
     {
	sendMessage(origin,
		    "The stock is empty, you can't pick up a card. If you "
		    "cannot discard a card, you may pass");
	return true; // Keep the game alive
     }

   // Okay, it must be the current player.. Pick up a card for them
   Cards::Card card = stock.removeCard();
   sendMessage(origin, String("You picked up ") + card.getName());
   (*currentPlayer).second.addCard(card);

   // Move to the next player
   if (!stock.total())
     {
	nextPlayer("picks up a card. The stock is now empty");
     }
   else
     {
	nextPlayer("picks up a card");
     }

   return true;
}

/* parsePLAY - Parse the PLAY command, which registers players
 * Original 29/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parsePLAY)
{
   // If we are already playing, tell the user they cannot join mid-game
   if (playing)
     {
	sendMessage(origin,
		    "You cannot join the game in the middle of play, sorry");
	return true; // Keep the game alive
     }

   // Make sure there aren't TOO MANY players!
   if (players.size() >= maxPlayers)
     {
	sendMessage(origin,
		    "Sorry, there are too many players in this game!");
	return true; // Keep the game alive
     }

   // Check if the user is already playing the game
   if (isPlaying(origin))
     {
	sendMessage(origin, "You are already registered to play the game!");
	return true; // Keep the game alive
     }

   // Add them..
   players.push_back(player_type(&origin, Cards::Hand()));
   sendMessage(origin,
	       "You are now registered to play, " +
	       players.front().first->getNickname() +
	       " must deal before the game begins.");
   sendMessage(*(players.front().first),
	       origin.getNickname() + " has registered to play.");

   // If we are at the maximum number of players, tell the dealer
   if (players.size() == maxPlayers)
     {
	sendMessage(*(players.front().first),
		    "You have reached the maximum number of players for "
		    "Exordi8 - No more players can join the game. You must "
		    "deal the cards now to begin the game.");
     }

   return true;
}

/* parseSHOWHAND - Show a player their hand
 * Original 30/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parseSHOWHAND)
{
   if (!playing)
     {
	sendMessage(origin,
		    players.front().first->getNickname() +
		    " needs to deal before you have a hand to look at!");
	return true; // Keep the game alive
     }

   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin)) == 0)
     {
	return true; // Keep the game alive
     }

   // OKAY... now show the player their cards
   showHand(*player);

   return true;
}

/* parseSTATUS - Show a player information about the current game
 * Original 30/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parseSTATUS)
{
   // If we are not playing, we cannot be very handy
   if (!playing)
     {
	sendMessage(origin,
		    "The game has not begun yet - " +
		    players.front().first->getNickname() +
		    " must deal to begin the game.");
	return true; // Keep the game alive
     }

   // Grab the player's info
   const player_type* player = checkPlayerStatus(origin, true);

   String out;

   // Run through the list of players and give them vague information..
   for (players_type::iterator p = players.begin();
	p != players.end(); p++)
     {
	// Make sure this is not the current player
	if (&(*p) != player)
	  {
	     // Do we need to add a formatting (for between players)
	     if (!out.empty())
	       {
		  out += "; ";
	       }

	     // Add the info
	     out += (*p).first->getNickname() + ": " +
	       String::convert((*p).second.getCardCount()) + " cards";

	     // If the line is too long, send what we have and clear it
	     if (out.length() > 300)
	       {
		  sendMessage(origin, out);
		  out.clear();
	       }
	  }
     }

   // If there is anything left to send, send it
   if (!out.empty())
     {
	sendMessage(origin, out);
     }

   // If this person is a player, show their current hand as well
   if (player != 0)
     {
	showHand(*player);
     }

   // Tell them a bit more information
   out = "It's " + (*currentPlayer).first->getNickname();
   if ((*currentPlayer).first->getNickname()
       [(*currentPlayer).first->getNickname().length() - 1] == 's')
     {
	out += "' turn. ";
     }
   else
     {
	out += "'s turn. ";
     }

   if (!stock.total())
     {
	out += "The stock is empty.";
     }
   else
     {
	out += "There are " + String::convert(stock.total()) +
	  " cards left in the stock.";
     }

   // Send the last bit of information..
   sendMessage(origin, out);

   return true;
}

/* parseSUIT - Allow a user to select a suit (if they have discarded an 8)
 * Original 31/08/2002 pickle
 */
EXORDI8_FUNC(Exordi8::parseSUIT)
{
   // Check the player's info
   const player_type* player;
   if ((player = checkPlayerStatus(origin)) == 0)
     {
	return true; // Keep the game alive
     }

   // Make sure this player is the current player..
   if (player != &(*currentPlayer))
     {
	sendMessage(origin,
		    "It's not your turn, it's " +
		    (*currentPlayer).first->getNickname() + "'s turn");
	return true; // Keep the game alive
     }

   // Make sure an 8 has been dropped
   if ((lastDiscardedCard.getIndex() != 8) || (nextSuit != 0))
     {
	sendMessage(origin,
		    "You need to discard a card with a face-value of '8' before "
		    "you can select a suit for the next player to play");
	return true; // Keep the game alive
     }

   // Okay, try to comprehend what suit they wanted
   if ((nextSuit = Cards::Card::findSuit(line.nextToken())) == 0)
     {
	sendMessage(origin,
		    "I don't know what suit you meant. It must be either "
		    "Clubs, Spades, Hearts or Diamonds.");
	return true; // Keep the game alive
     }

   // Next player!!
   nextPlayer("has selected the next suit after discarding an '8'", false);

   // Tell the next player which suit they need..
   sendMessage(*((*currentPlayer).first),
	       String("You need to use a card of the suit ") +
	       Cards::Card::nameSuit(nextSuit) + ", or take a card.");

   return true;
}

