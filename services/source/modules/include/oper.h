// This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                             
// $Author$        
// $Date$                      
// $Id$                                                                        


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
