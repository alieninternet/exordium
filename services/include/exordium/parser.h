/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __PARSER_H_
#define __PARSER_H_


#include <kineircd/str.h>

# define PARSER_FUNC(x)         x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

namespace Exordium {

class Parser
{
private:
	struct functionTableStruct
  	{
    		char const *command;
    		void PARSER_FUNC ((*function));
  	};
	 struct functionTableStruct const functionTable[];

public:
  	 void parseLine (String &);
private:
   void PARSER_FUNC (parsePASS);
   void PARSER_FUNC (parsePRIVMSG);
   void PARSER_FUNC (parsePART);
   void PARSER_FUNC (parseM);
   void PARSER_FUNC (parseS);
   void PARSER_FUNC (parseN);
   void PARSER_FUNC (parsePING);
   void PARSER_FUNC (parseQUIT);
   void PARSER_FUNC (parseSQUIT);
   void PARSER_FUNC (parseSJOIN);
   void PARSER_FUNC (parseAWAY);
};

#endif

};

