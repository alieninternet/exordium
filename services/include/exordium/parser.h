/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __PARSER_H_
#define __PARSER_H_

namespace Exordium {
   class Services;
};

#include <kineircd/str.h>

# define PARSER_FUNC(x) \
     x(const Kine::String& origin, Kine::StringTokens& tokens)

namespace Exordium {
   class Parser {
    private:
      Services &services;
      
      struct functionTableStruct {
	 const char* command;
	 const void PARSER_FUNC((Parser::* const function));
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
