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

#ifndef __PARSER_H_
#define __PARSER_H_

namespace Exordium {
   class Services;
   class User;
};

#include <kineircd/str.h>

# define PARSER_FUNC(x) \
     x(Kine::String& OLDorigin, Kine::StringTokens& tokens)

namespace Exordium {
   class Parser {
    private:
      Services &services;
      
      struct functionTableStruct {
	 const char* command;
	 void PARSER_FUNC((Parser::* const function));
      } static const functionTable[];
      
    public:
      Parser(Services &s)
	: services(s)
	{};
      
      void parseLine(const Kine::String &);
      
    private:
      void PARSER_FUNC(parsePASS);
      void PARSER_FUNC(parsePRIVMSG);
      void PARSER_FUNC(parsePART);
      void PARSER_FUNC(parseM);
      void PARSER_FUNC(parseS);
      void PARSER_FUNC(parseN);
      void PARSER_FUNC(parsePING);
      void PARSER_FUNC(parseQUIT);
      void PARSER_FUNC(parseSQUIT);
      void PARSER_FUNC(parseSJOIN);
      void PARSER_FUNC(parseAWAY);
   };
}; // namespace Exordium

// Complete the forwarded definition
#include "exordium/services.h"

#endif
