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


#ifndef __GAME_H_
#define __GAME_H_

#include <kineircd/str.h>
#include <exordium/services.h>
#include <map>

class ChannelGame;

# define GAME_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;

#include <exordium/service.h>

class Game : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void GAME_FUNC ((Game::* const function));
  };
  static struct functionTableStruct const functionTable[];

   // Channel games in progress..
   typedef std::map < Kine::String, ChannelGame* > channelGames_type;
   channelGames_type channelGames;
   
 public: // temporary, I'm in a rush.
   Exordium::Services& services;
   const String myName;

public:
   Game(Exordium::Services& s, const String& mn)
	: Service(), services(s), myName(mn)
	{
		std::cout << "Game" << std::endl;
	};
  ~Game(void)
	{
		services.queueAdd(":Game QUIT :Module unloaded");
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   void GAME_FUNC (parseHELP);
   void GAME_FUNC (parseQUOTE);
   void GAME_FUNC (parseSTART);
};


#include "channelgame.h"

#endif
