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

#ifndef __STOCK_H__
# define __STOCK_H__

# include <vector>
# include <stack>

# include "pack.h"

namespace Cards {
   template < class CardType >
   class Stock {
     private:
       typedef Cards::Pack < CardType > Pack;
       std::vector < Pack > packs;
       
       // The stock of cards
       typedef std::stack < CardType > stock_type;
       stock_type stock;

     public:
       // Empty stock.. needed for discard pile
       Stock(void) { }

       Stock(int numPacks)
         {
           // Obtain our packs, shuffling each deck.
           while (numPacks > 0)
           {
             packs.push_back(Pack());
             packs.back().shuffle();
             numPacks--;
           }

           // Populate stock
           while (!packs.empty())
           {
             while (!packs.back().isEmpty())
             {
               stock.push(packs.back().removeCard());
             }

             // Turf the empty pack
             packs.pop_back();
           }
         }

       void addCard(CardType card) 
         { stock.push(card); }

       unsigned int total(void)
         { return stock.size(); }

       CardType removeCard(void)
         { 
           if(!stock.empty())
           {
             CardType card = stock.top();
             stock.pop();
             return card;
           }
           return 0;
         }
   };
}; // namespace Cards
   
#endif // __PACKS_H__
   
