/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
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

#ifndef _DCHAN_H_
# define _DCHAN_H_

# include <kineircd/str.h>
# include <map>
# include <exordium/services.h>

namespace Exordium {
   class Services;
/* temp name ... Chan conflicts with the module 'Chan' .. suggestions for name? :-) */
   class dChan {
    private:
      AISutil::String name;
      int onlineID;
      Services& services;
      
    public:
     const AISutil::String& getName(void) const
	{ return name; };
      
      /* getOnlineID()
       * 
       * Return our unique sql identifier
       * 
       */
      const int getOnlineID(void) const
	{ return onlineID; };
      
      /* getOnlineIDString()
       * 
       * Return our unique sql identifier 
       * (as a String)
       * 
       * This is redundant imho. - simon
       */
      const AISutil::String getOnlineIDString(void) const
	{ return AISutil::String::convert(onlineID); };

      dChan(AISutil::String const &n, int const &oid, Services &s)
	: name(n), //their current nickname.. duh
          onlineID(oid), //as dictated by the sql server..
          services(s)// Gives us access to the full suite of services..
	{};

      ~dChan(void)
	{};
      
      // is equal to operator
      bool operator==(const dChan &c) const
	{ return (onlineID == c.onlineID); };
   };
}; // class Exordium

#endif
