/*
 *  * Exordium Network Services
 *  * Copyright (C) 2002 IRCDome Development Team
 *  *
 *  * This file is a part of Exordium.
 *  *
 *  * $Id$
 *  *
 *  * This program is free software; you can redistribute it and/or modify
 *  * it under the terms of the GNU General Public License as published by
 *  * the Free Software Foundation; either version 2 of the License, or
 *  * (at your option) any later version.
 *  *
 *  * This program is distributed in the hope that it will be useful,
 *  * but WITHOUT ANY WARRANTY; without even the implied
 *  * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  * See the GNU General Public License for more details.
 *  *
 *  * You should have received a copy of the GNU General Public License
 *  * along with this program; if not, write to the Free Software
 *  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,USA
 *  *
 *  * For contact details for the IRCDome Development Team please see the
 *  * enclosed README file.
 *  *
 * */


#ifndef __NOTE_H_
#define __NOTE_H_

#include <kineircd/str.h>
# define NOTE_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"
#include "exordium/services.h"

namespace Exordium {

class Note : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void NOTE_FUNC ((Note::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
   
public:
   Note(Exordium::Services& s)
   : Service(), services(s)
   {
	std::cout << "Hi im an instance. :(" << std::endl;
   };
  ~Note(void) 
   {
	std::cout << "YOU BASTARD, YOU KILLED NOTE" << std::endl;
   };   
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
  void NOTE_FUNC (parseSEND);
  void NOTE_FUNC (parseLIST);
  void NOTE_FUNC (parseREAD);
  void NOTE_FUNC (parseDEL);
};
















};
#endif
