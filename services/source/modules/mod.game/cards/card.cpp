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

#include <aisutil/string.h>

#include "card.h"

using AISutil::String;
using AISutil::StringTokens;


/* This relates closely to the enumeration list in the card class. For
 * efficiency it was decided to use redundant data rather than constantly
 * regenerate this data (i.e. <name> + of + <suit>). The time wasted in
 * creating and concatenating a few strings is negligable, however this way
 * is much faster in the long run.
 * 
 * Note that the indexes of this list are - 1 of their real values
 * 
 * All of this should be autogen'd!! ;)
 */
const char* Cards::Card::namesList[4][13] = {
     {	
	"Ace of Clubs",
	"2 of Clubs",
	"3 of Clubs",
	"4 of Clubs",
	"5 of Clubs",
	"6 of Clubs",
	"7 of Clubs",
	"8 of Clubs",
	"9 of Clubs",
	"10 of Clubs",
	"Jack of Clubs",
	"Queen of Clubs",
	"King of Clubs"
     },
     {	
	"Ace of Spades",
	"2 of Spades",
	"3 of Spades",
	"4 of Spades",
	"5 of Spades",
	"6 of Spades",
	"7 of Spades",
	"8 of Spades",
	"9 of Spades",
	"10 of Spades",
	"Jack of Spades",
	"Queen of Spades",
	"King of Spades"
     },
     {	
	"Ace of Hearts",
	"2 of Hearts",
	"3 of Hearts",
	"4 of Hearts",
	"5 of Hearts",
	"6 of Hearts",
	"7 of Hearts",
	"8 of Hearts",
	"9 of Hearts",
	"10 of Hearts",
	"Jack of Hearts",
	"Queen of Hearts",
	"King of Hearts"
     },
     {	
	"Ace of Diamonds",
	"2 of Diamonds",
	"3 of Diamonds",
	"4 of Diamonds",
	"5 of Diamonds",
	"6 of Diamonds",
	"7 of Diamonds",
	"8 of Diamonds",
	"9 of Diamonds",
	"10 of Diamonds",
	"Jack of Diamonds",
	"Queen of Diamonds",
	"King of Diamonds"
     }
};

// List of suits for easy translation.. Note this is suit-1
const Cards::Card::suitsList_type Cards::Card::suitsList[4] = {
   { "CLUBS",		"\226\153\163" },
   { "SPADES",		"\226\153\160" },
   { "HEARTS",		"\226\153\165" },
   { "DIAMONDS",	"\226\153\166" }
};

// List of ranks for easy translation.. Note this is index-1..
const char* Cards::Card::ranksList[13] = {
   "ACE",
   "2",
   "3",
   "4",
   "5",
   "6",
   "7",
   "8",
   "9",
   "10",
   "JACK",
   "QUEEN",
   "KING"
};


/* Constructor - Converts a name (such as "Queen of hearts") into a valid card
 * Original 31/08/2002 pickle
 */
Cards::Card::Card(const String& name)
  : suit(0),
    index(0)
{
   // Tokenise the string so we can jump through it in a nice lazy fashion
   StringTokens st(name.toUpper());
   
   // Grab the first token..
   String token = st.nextToken();
   
   // Is this supposed to be a joker?
   if (token == "JOKER") {
      // A joker it shall be then,
      suit = Suit::Joker;
      index = Rank::Joker;
      return;
   }
   
   /* The first word must always be the index/rank of the card.. Let's try
    * and look its index value up
    */
   for (unsigned int i = 0; i != 13; i++) {
      if (token == ranksList[i]) {
	 // Ahh a match! Be happy then..
	 index = ++i;
	 break;
      }
   }
   
   // Did we work out the index?
   if (index == 0) {
      // Be upset..
      return;
   }
   
   // Okay, grab the next word
   token = st.nextToken();
   
   // If the next word is 'of', skip it.
   if (token == "OF") {
      token = st.nextToken();
   }
   
   // Try and find the suit..
   for (unsigned int i = 0; i != 4; i++) {
      if (token == suitsList[i].name) {
	 // Ahh a match! Return joyously..
	 suit = ++i;
	 return;
      }
   }
   
   // If we got here, the card will remain invalid
}


/* Less than operator - Check if a card ranks higher than another card
 * Original 30/08/2002 pickle
 */
bool Cards::Card::operator<(const Card& rhs) const
{
   // See if the rank can be assumed by suit alone
   if (suit < rhs.suit) {
      return true;
   } else if (suit > rhs.suit) {
      return false;
   }
   
   // The suits are equal, check the index - note the ranks work DOWNWARDS
   return (index > rhs.index);
}


/* findSuit - Help people find a suit
 * Original 31/08/2002 pickle
 */
unsigned char Cards::Card::findSuit(const String& suitName)
{
   String name = suitName.toUpper();
   
   // Try and find the suit..
   for (unsigned int i = 0; i != 4; i++) {
      // If it matched, return it
      if (name == suitsList[i].name) {
	 return ++i;
      }
   }
   
   return 0;
}


/* nameSuit - Help people name a suit
 * Original 31/08/2002 pickle
 */
const char* Cards::Card::nameSuit(const unsigned char suit)
{
   if ((suit == 0) || (suit > 4)) {
      return "Unknown";
   }
   
   return suitsList[suit - 1].name;
}

/* populate - Populate the pack
 * Original 07/10/2002 josullivan
 */
void Cards::Card::populate(cards_type& cards)
{
   // Run through and populate the pack (yes, rank is sorted DOWNWARDS)
   for (register unsigned int suit = 1; suit != 5; suit++) {
      for (register unsigned int index = 14; --index;) {
#ifdef DEBUG
         std::cerr << "Pushing (suit, index) ( " << suit << ", " << index << ")\n";
#endif
         cards.push_back(Cards::Card(suit, index));
      }
   }
}
