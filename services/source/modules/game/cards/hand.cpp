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

#include <sstream>

#include "hand.h"

using LibAIS::String;


/* getHandStr - Grab the name of the cards in this hand
 * Original 30/08/2002 simonb
 */
String Cards::Hand::getHandStr(void) const
{
   // If the hand is empty, say so
   if (cards.empty()) {
      return "No cards.";
   }
   
   std::ostringstream out;
   bool first = true;
   
   // Iterate through the cards list
   for (cards_type::const_iterator it = cards.begin(); it != cards.end();
	it++) {
      // Determine list formatting
      if (!first) {
	 out << ", " << (*it).getName();
      } else {
	 first = false;
	 out << (*it).getName();
      }
   }
 
   // Return the string..
   return out.str();
}
