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
    void OPER_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];
  const String myName;
  void sendMessage(const String &to, const String &message)
	{
		Exordium::Services::serviceNotice(message,myName,to);
	}
public:
  Oper(const String &mn)
	: myName(mn)
	{
	};

  ~Oper(void)
	{
		std::cout << "Dead Oper" << std::endl;
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   static void OPER_FUNC (parseHELP);
   static void OPER_FUNC (parseQUOTE);
   static void OPER_FUNC (parseJUPE);
};




#endif
