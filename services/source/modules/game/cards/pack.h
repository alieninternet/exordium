/* $Id$
 * A pack class (or deck, if you're American)
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

#ifndef __PACK_H__
# define __PACK_H__

# include <vector>

# include "card.h"

namespace Cards {
   // The pack class
   class Pack {
    private:
      typedef std::vector <Card> cards_type;
      
      // Our cards
      cards_type cards;
      
    public:
      // Constructor (populates the pack, all cards are in 'mint' order)
      Pack(bool withJoker = false);
      
      // Return the number of cards left in the pack
      cards_type::size_type getCardCount(void) const
	{ return cards.size(); };
      
      // Is the pack empty?
      bool isEmpty(void) const
	{ return cards.empty(); };
      
      // Shuffle the pack
      void shuffle(void);
      
      // Remove a card from the pack
      Card removeCard(void)
	{
	   Card card = cards.back();
	   cards.pop_back();
	   return card;
	};
   };
}; // namespace Cards
   
#endif // __PACK_H__
   
