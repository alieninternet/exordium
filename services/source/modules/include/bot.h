// This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                             
// $Author$        
// $Date$                      
// $Id$                                                                        


#ifndef __BOT_H_
#define __BOT_H_

#include "exordium/service.h"
#include "exordium/services.h"
#include "kineircd/str.h"

# define BOT_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;


class Bot : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void BOT_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];
  const String myName;
  void sendMessage(const String &to, const String &message)
	{
		Exordium::Services::serviceNotice(message,myName,to);
	}
public:
  Bot(const String &mn)
	: myName(mn)
	{
	};

  ~Bot(void)
	{
		std::cout << "Dead Bot" << std::endl;
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   static void BOT_FUNC (parseHELP);
   static void BOT_FUNC (parseQUOTE);
};




#endif
