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

#ifndef __IRCDOME_H_
#define __IRCDOME_H_

#include <kineircd/str.h>

# define IRCDOME_FUNC(x) \
     x(LibAIS::String &origin, LibAIS::StringTokens &tokens)

namespace Exordium {
   class Services;
   
   class IRCDome {
    private:
      Services& services;
      
      struct functionTableStruct {
	 const char* command;
	 void IRCDOME_FUNC((IRCDome::* const function));
      } static const functionTable[];
      
      void IRCDOME_FUNC(parseMODULE);
      
    public:
      IRCDome(Services& s)
	: services(s)
	{};
      
      void parseLine(const LibAIS::String &, const LibAIS::String &);
   };
};

// Complete the forwarded definition
# include "exordium/services.h"

#endif
