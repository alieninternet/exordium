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

#ifndef _SOURCE_LIB_CONSOLE_H_
# define _SOURCE_LIB_CONSOLE_H_ 1


/* This should be in source/lib/. - nothing outside of the core uses it.. */

# include <aisutil/string.h>
# include <kineircd/name.h>

# define CONSOLE_FUNC(x) \
     x(Kine::Name &origin, AISutil::StringTokens &tokens)

namespace Exordium {
   class ServicesInternal;
   
   class Console {
    private:
      ServicesInternal& services;
      
      struct functionTableStruct {
	 const char* command;
	 void CONSOLE_FUNC((Console::* const function));
      } static const functionTable[];
      
      void CONSOLE_FUNC(parseMODULE);
      void CONSOLE_FUNC(parseLANGUAGE);
      void CONSOLE_FUNC(parseHELP);

    public:
      Console(ServicesInternal& s)
	: services(s)
	{};

      ~Console(void)
	{};
      
      void parseLine(const AISutil::String &, const AISutil::String &);
   };
};

// Complete the forwarded definition
# include "services.h"

#endif // _SOURCE_LIB_CONSOLE_H_
