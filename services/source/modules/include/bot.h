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

#ifndef __BOT_H_
#define __BOT_H_

#include "exordium/service.h"
#include "exordium/services.h"
#include "exordium/user.h"

#include <kineircd/str.h>


#define BOT_FUNC(x)           x(Exordium::User& origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;


class Bot : public Exordium::Service
{
   class Exordium::User;
 private:
  struct functionTableStruct
  {
    char const *command;
     void BOT_FUNC ((Bot::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
  const String myName;
  void sendMessage(const String &to, const String &message)
	{
		services.serviceNotice(message,myName,to);
	};
public:
  Bot(Exordium::Services& s, const String &mn)
	: services(s), myName(mn)
	{
	};

  ~Bot(void)
	{
		std::cout << "Dead Bot" << std::endl;
	};
   void parseLine(Kine::StringTokens& line, Exordium::User& origin);
   
   void parseLine(Kine::StringTokens& line, Exordium::User& origin,
		  const Kine::String& channel);
private:
  void BOT_FUNC (parseHELP);
  void BOT_FUNC (parseQUOTE);
};




#endif
