// This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                             
// $Author$        
// $Date$                      
// $Id$                                                                        


#ifndef __BOT_H_
#define __BOT_H_

#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>

# define BOT_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;


class Bot : public Exordium::Service
{
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
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
  void BOT_FUNC (parseHELP);
  void BOT_FUNC (parseQUOTE);
};




#endif
