/* $Id$
 * A hand class
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

#ifndef __HAND_H__
# define __HAND_H__

# include <set>
# include <kineircd/str.h>

# include "card.h"

namespace Cards {
   // The hand class
   class Hand {
    private:
      typedef std::multiset <Card> cards_type;
      
      // Our cards
      cards_type cards;
      
    public:
      // Return the number of cards left in the hand
      cards_type::size_type getCardCount(void) const
	{ return cards.size(); };
      
      // Is this hand empty?
      bool isEmpty(void) const
	{ return cards.empty(); };
      
      // Grab the name of the cards in this hand
      Kine::String getHandStr(void) const;
	
      // Add a card to this hand
      void addCard(const Card& card)
	{ (void)cards.insert(card); };

      // Remove a card from this hand
      Cards::Card removeCard(const Cards::Card& card)
	{
	   cards_type::iterator cardLocation = cards.find(card);
	   Card c = *cardLocation;
	   cards.erase(cardLocation);
	   return c;
	};
      
      // Does this hand contain a specific card (or maybe more than one)
      bool hasCard(const Card& card) const
	{ return (cards.find(card) != cards.end()); };
   };
}; // namespace Cards
   
#endif // __HAND_H__
   
