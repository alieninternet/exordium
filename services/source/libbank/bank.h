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

#ifndef _SOURCE_LIBBANK_BANK_H_
# define _SOURCE_LIBBANK_BANK_H_ 1

# include <set>
# include <kineircd/str.h>

# include <exordium/services.h>
# include <exordium/user.h>

namespace Exordium {
  // The credits class
  class Bank {
    private:
      Exordium::Services& services;

    public:
      Bank(Exordium::Services& s) : services(s)
        { };

      ~Bank(void)
        { };

      const int getBalance(const Exordium::User& origin);
      bool alterBalance(const Exordium::User& origin, const int amount);
  };
};
   
#endif // _SOURCE_LIBBANK_BANK_H_
