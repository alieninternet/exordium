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

#ifndef __SHUFFLER_H__
# define __SHUFFLER_H__

# include <functional>

namespace Cards {
   // The shuffler class
   template < class ContainerType >
   ContainerType shuffler(ContainerType cardList)
   {
      // Shuffle the stock
//      ContainerType operator()(ContainerType cardList)
//      {
         ContainerType newCardList;
         newCardList.reserve(cardList.size());
                
         // Until the main list of cards is empty, dump the cards in the above vector
         while (!cardList.empty()) 
         {
            typename ContainerType::iterator iter;

            int iterationCount = (int)((float)cardList.size() * rand() /
                (RAND_MAX + 1.0));

            // Loop to that location
            for (iter = cardList.begin(); iterationCount > 1; iter++) {
               iterationCount--;
            }

            // Copy the card..
            newCardList.push_back(*iter);
             
            // Delete the card..
            cardList.erase(iter);
         }
          
         return newCardList;
//      };
   };
}; // namespace Cards
   
#endif // __SHUFFLER_H__

