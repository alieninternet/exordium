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

#ifndef _SOURCE_MODULES_GAME_DICE_DIE_H_
# define _SOURCE_MODULES_GAME_DICE_DIE_H_ 1

# ifdef HAVE_CONFIG_H
#  include "autoconf.h"
# endif

# include <cstdlib>
# ifdef DEBUG
#  include <cassert>
# endif

namespace Dice {
   /* The die class. Each die takes up two bytes of memory, and can be from
    * a coin to a D126 ;) This class should happily accomodate common RPG dice,
    * such as D10, and D20, but defaulting to the bog-standard D6 everyone
    * knows. For now, this class doesn't actually do any alphabetical die
    * handling, but the stuff is there to do so at a later date. This class
    * also cannot handle dice with mixed face types (i.e. alphanumerical, or
    * symbolic), or die with a zero.
    */
   class Die {
    public:
      // The value of the dice when no roll has been done, yet.
      static const unsigned char NO_ROLL_YET = 127;
	
    private:
      union {
	 struct {
	    const bool alphabetical:1;		// Alphabetical die?
	    const unsigned char faces:7;	// The number of faces
	    unsigned char value:7;		// The value of the die
	 };
	 
	 unsigned short conflux;
      };
      
    public:
      // Constructor - make a new die with the given number of faces
      Die(unsigned char f = 6, bool a = false)
	: faces(f),
          alphabetical(a),
          value(NO_ROLL_YET)
	{
# ifdef DEBUG
	   assert((f >= 2) && (f < 127));
# endif
	};
      
      // Copy constructor
      Die(const Die& d)
	: conflux(d.conflux)
	{};
      
      // Destructor
      ~Die(void)
	{};
      
      // Get the value of the die
      const unsigned char getValue(void) const
	{ return value; };
      
      // Get the number of faces of the die
      const unsigned char getFaces(void) const
	{ return faces; };

      // Roll the die (returns the value after the roll)
      unsigned char roll(void)
	{ 
	   return 
	     (value = (1 + ((unsigned char)
			    ((float)faces * rand() / (RAND_MAX + 1.0)))));
	};
   }; // class Die
}; // namespace Dice
   
#endif // _SOURCE_MODULES_GAME_DICE_DIE_H_
   
