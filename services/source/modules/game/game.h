/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

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

   Exordium::Services& services;

   // Channel games in progress..
   typedef std::map < Kine::String, ChannelGame* > channelGames_type;
   channelGames_type channelGames;
   
public:
   Game(Exordium::Services& s)
	: Service(), services(s)
	{
		std::cout << "Game" << std::endl;
	};
  ~Game(void)
	{
		services.queueAdd(":Game QUIT :Module unloaded");
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
  static String me;
private:
   void GAME_FUNC (parseHELP);
   void GAME_FUNC (parseQUOTE);
   void GAME_FUNC (parseSTART);
};


#include "channelgame.h"

#endif
