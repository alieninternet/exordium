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

#ifndef __PACK_H__
# define __PACK_H__

# include <vector>

# include "shuffler.h"

namespace Cards {
   // The populator functor
   template < class T > class _populator {
    public:
      typedef std::vector <T> cards_type;
      void operator()(cards_type& cards) 
        { 
          T::populate(cards); 
        };
   };

   // The pack class
   template < class CardType, class Allocator = _populator<CardType> >
   class Pack {
    private:
      typedef std::vector <CardType> cards_type;
      
      // Our cards
      cards_type cards;

      // Our shuffler functor
      //Shuffler<cards_type> shuffler;

    public:
      // Constructor (populates the pack, all cards are in 'mint' order)
      Pack(bool withJoker = false) 
        { Allocator allocator; allocator(cards); }

      // Return the number of cards left in the pack
      typename cards_type::size_type getCardCount(void) const
	{ return cards.size(); };
      
      // Is the pack empty?
      bool isEmpty(void) const
	{ return cards.empty(); };
      
      // Shuffle the pack
      void shuffle(void)
        { 
          cards_type newCardList = shuffler<cards_type>(cards);
          cards.swap(newCardList);
        }
      
      // Remove a card from the pack
      CardType removeCard(void)
	{
	   CardType card = cards.back();
	   cards.pop_back();
	   return card;
	};
   };
}; // namespace Cards
   
#endif // __PACK_H__
   
