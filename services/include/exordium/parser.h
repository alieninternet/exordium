/*
    PARSER.H   - Main entry point for IRCDome Network Services
    Version   - 0.1
    Date      - 8th May 2002
    Revisions -
  
    Copyright 2002 James Ian Wilkins <james@ircdome.org>

    This file is part of IRCDome's Services.

    IRCDome Services is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    IRCDome Services is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


 */

#ifndef __PARSER_H_
#define __PARSER_H_


#include "kineircd/str.h"

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
};

#endif

};

