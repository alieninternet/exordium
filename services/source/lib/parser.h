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

#ifndef _SOURCE_LIB_PARSER_H_
# define _SOURCE_LIB_PARSER_H_ 1

# include <kineircd/str.h>

# define PARSER_FUNC(x) \
     x(AISutil::String& OLDorigin, AISutil::StringTokens& tokens)

namespace Exordium {
   class ServicesInternal;
   class User;
   
   class Parser {
    private:
      ServicesInternal &services;
      
      struct functionTableStruct {
	 const char* command;
	 void PARSER_FUNC((Parser::* const function));
      } static const functionTable[];
      
    public:
      Parser(ServicesInternal& s)
	: services(s)
	{};

      ~Parser(void)
	{};
      
      void parseLine(const AISutil::String&);
      
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
      void PARSER_FUNC(parseKICK);
      void PARSER_FUNC(parseKILL);
      void PARSER_FUNC(parseMOTD);
      void PARSER_FUNC(parseTOPIC);
      void PARSER_FUNC(parseUSERAWAY);
      void PARSER_FUNC(parseGNOTICE);
   };
}; // namespace Exordium

// Complete the forwarded definition
# include "services.h"

#endif // _SOURCE_LIB_PARSER_H_
