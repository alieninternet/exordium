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

#ifndef __OPER_H_
#define __OPER_H_

#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>

# define OPER_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;


class Oper : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void OPER_FUNC ((Oper::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
  const String myName;
  void sendMessage(const String &to, const String &message)
	{
		services.serviceNotice(message,myName,to);
	}
public:
   Oper(Exordium::Services& s, const String &mn)
	: services(s), myName(mn)
	{
	};

  ~Oper(void)
	{
		std::cout << "Dead Oper" << std::endl;
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   void OPER_FUNC (parseHELP);
   void OPER_FUNC (parseQUOTE);
   void OPER_FUNC (parseJUPE);
};




#endif
