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

#ifndef _SOURCE_MODULES_GAME_CARDS_CARD_H_
# define _SOURCE_MODULES_GAME_CARDS_CARD_H_ 1

# include <aisutil/string.h>
# include <vector>

namespace Cards {
   /* The card class. This is based on INTERNATIONAL/FRENCH class of cards,
    * and therefore does not fit in with Swiss or German cards. This should be
    * acceptable for the majority of modern card games, or at the very least
    * all popular modern card games. Each instance of this class will
    * consume byte of memory.
    */
      
   class Card;
   typedef std::vector<Card> cards_type;

   class Card {
    public:
      struct Suit { // <=- should be namespace?
	 enum {
	    Joker = 0,	// Rarely used within games, though
	    Clubs = 1,
	    Spades = 2,
	    Hearts = 3,
	    Diamonds = 4,
	 };
      };
      
      // Some handy names which can be used whilst checking the index
      struct Rank { // <=- Should be namespace too?
	 enum {
	    Ace = 1,
	    Jack = 11,
	    Queen = 12,
	    King = 13,
	    Joker = 14
	 };
      };
      
      // Handy colour thingies (for getColour)
      struct Colour { // <=- Should be namespace also?
	 enum {
	    Black = 0,
	    Red = 1
	 };
      };
      
    private:
      // A look-up list of names for the getName() function
      static const char* namesList[4][13];
      
      // More names, used for converting card names into proper ranking/suit
      struct suitsList_type {
	 const char* const name;		// Name of the suit
	 const char* const glyph;		// Glyph for the suit (UTF-8)
      } static const suitsList[4];
      static const char* ranksList[13];

      // A union, to allow for speedy copies of the data while saving memory
      union {
	 struct {
	    // What kind of suit we are
	    unsigned char suit:3;
   
	    /* Our index number
	     * Note that 1 = ACE, 11 = JACK, 12 = QUEEN and 13 = KING.
	     * Special number 14 = JOKER
	     */
	    unsigned char index:4;
	 };
	 
	 unsigned char conflux;
      };

    public:
      // Constructor (makes an invalid card)
      Card(void)
	: conflux(0)
	{};
      
      // Constructor, using raw information about the suit and index numbers
      Card(const unsigned char s, const unsigned char i = Rank::Joker)
	: suit(s), index(i)
	{};

      // Copy constructor
      Card(const Card& c)
	: conflux(c.conflux)
	{};
      
      // Constructor, but using a string (such as "Queen of hearts")
      Card(const AISutil::String& name);
      
      // Return the suit this card belongs to
      const unsigned char getSuit(void) const
	{ return suit; };
      
      // Return the index of this card (it's value in most cases)
      const unsigned char getIndex(void) const
	{ return index; };

      // Return the colour of this card
      const unsigned char getColour(void) const
	{
	   return (((suit == Suit::Hearts) || (suit == Suit::Diamonds)) ?
		   Colour::Red : Colour::Black);
	};
      
      // Is this card an ace?
      const bool isAce(void) const
	{ return (index == 1); };

      // Is this card a spot card?
      const bool isSpotCard(void) const
	{ return ((index > 1) && (index < 11)); };
      
      // Is this card a face card?
      const bool isFaceCard(void) const
	{ return (index > 10); };
      
      // Is this card a joker?
      const bool isJoker(void) const
	{ return ((suit == Suit::Joker) && (index == Rank::Joker)); };

      // Is this card valid?
      const bool isValid(void) const
	{ 
	   return (isJoker() ||
		   ((suit > 0) && (suit <= 5) &&
		    (index > 0) && (index < 14)));
	};
      
      // Return the name of this card
      const char* getName(void) const
	{ return (isJoker() ? "Joker" : namesList[suit - 1][index - 1]); };
      
      // Return the colour name of this card
      const char* getColourName(void) const
	{ return (getColour() == Colour::Red ? "Red" : "Black"); };
      
      // Check if a card ranks higher than another card (for sorting mainly)
      bool operator<(const Card& rhs) const;
      
      // Help people find a suit..
      static unsigned char findSuit(const AISutil::String& suitName);
      
      // Help people name a suit..
      static const char* nameSuit(const unsigned char suit);

      // Populate the the pack with the appropriate cards
      static void populate(cards_type& cards);
   };
  
}; // namespace Cards
   
#endif // _SOURCE_MODULES_GAME_CARDS_CARD_H_
   
