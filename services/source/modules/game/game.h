/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __GAME_H_
#define __GAME_H_

#include <kineircd/str.h>
#include "exordium/services.h"

# define GAME_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {


class Game : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void GAME_FUNC ((Game::* const function));
  };
  static struct functionTableStruct const functionTable[];

   Exordium::Services& services;
   
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
};




};

#endif
