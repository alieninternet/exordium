/* $Id$
 * A pack class (or deck, if you're American)
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

#include <cstdlib>

#include "pack.h"


/* Pack - Pack constructor; populates a fresh pack of cards in 'mint' order
 * Original 29/08/2002 simonb
 */
Cards::Pack::Pack(bool withJoker)
{
   // Run through and populate the pack (yes, rank is sorted DOWNWARDS)
   for (register unsigned int suit = 1; suit != 5; suit++) {
      for (register unsigned int index = 14; --index;) {
	 cards.push_back(Cards::Card(suit, index));
      }
   }
   
   // If the joker is also required, add it last
   if (withJoker) {
      cards.push_back(Cards::Card(Card::Suit::Joker));
   }
};


/* shuffle - Shuffle the pack of cards
 * Original 30/08/2002 simonb
 */
void Cards::Pack::shuffle(void)
{
   cards_type newCardList;
   newCardList.reserve(cards.size());
   
   // Until the main list of cards is empty, dump the cards in the above vector
   while (!cards.empty()) {
      cards_type::iterator cardLocation;
      int iterationCount = (int)((float)cards.size() * rand() /
				 (RAND_MAX + 1.0));

      // Loop to that location
      for (cardLocation = cards.begin(); iterationCount > 1; cardLocation++) {
	 iterationCount--;
      }
      
      // Copy the card..
      newCardList.push_back(*cardLocation);
      
      // Delete the card..
      cards.erase(cardLocation);
   }
   
   // Swap the two lists..
   cards.swap(newCardList);
}
