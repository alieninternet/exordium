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

#ifndef __NICK_H_
#define __NICK_H_

#include "exordium/services.h"
#include "exordium/service.h"
#include <kineircd/str.h>

#define NICK_FUNC(x)           x(Exordium::User& origin, Kine::StringTokens &tokens)

using Kine::String;



class Nick : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void NICK_FUNC ((Nick::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
  const String myName;
  void sendMessage(const String &to, const String &message)
	{
		services.serviceNotice(message,myName,to);
	}
public:
   Nick(Exordium::Services& s, const String &mn)
   : services(s), myName(mn)
   {
   };

  ~Nick(void)
   {
   };

   void parseLine (Kine::StringTokens& line, Exordium::User& origin);
   void parseLine (Kine::StringTokens& line, Exordium::User& origin,String const &);

private:

  void NICK_FUNC (parseIDENTIFY);
  void NICK_FUNC (parseHELP);
  void NICK_FUNC (parseKILL);
  void NICK_FUNC (parseREGISTER);
  void NICK_FUNC (parseGHOST);
  void NICK_FUNC (parseACCESS);
  void NICK_FUNC (parseSET);
  void NICK_FUNC (parseINFO);
  void NICK_FUNC (parseAUTH);
};


#endif
