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
	static struct functionTableStruct const functionTable[];

public:
  	static void parseLine (String &);
private:
  static void PARSER_FUNC (parsePASS);
  static void PARSER_FUNC (parsePRIVMSG);
  static void PARSER_FUNC (parsePART);
  static void PARSER_FUNC (parseM);
  static void PARSER_FUNC (parseS);
  static void PARSER_FUNC (parseN);
  static void PARSER_FUNC (parsePING);
  static void PARSER_FUNC (parseQUIT);
  static void PARSER_FUNC (parseSQUIT);
  static void PARSER_FUNC (parseSJOIN);
  static void PARSER_FUNC (parseAWAY);
};

#endif

};

