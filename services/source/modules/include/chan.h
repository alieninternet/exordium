// This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
// $Author$                                                                                                        
// $Date$                                                                                                          
// $Id$

#ifndef __CHAN_H_
#define __CHAN_H_

#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>

# define CHAN_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;


class Chan : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void CHAN_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];
  const String myName;
  void sendMessage(const String &message, const String &to)
	{
		Exordium::Services::serviceNotice(message,myName,to);
	}
public:
  Chan(const String &mn)
	: myName(mn)
	{
	};
  ~Chan(void)
	{
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   static void CHAN_FUNC (parseHELP);
   static void CHAN_FUNC (parseACCESS);
   static void CHAN_FUNC (parseKICK);
   static void CHAN_FUNC (parseOP);
   static void CHAN_FUNC (parseVOICE);
   static void CHAN_FUNC (parseREGISTER);
   static void CHAN_FUNC (parseBAN);
   static void CHAN_FUNC (parseDEOP);
   static void CHAN_FUNC (parseTOPIC);
   static void CHAN_FUNC (parseDEVOICE);
   static void CHAN_FUNC (parseADDUSER);
   static void CHAN_FUNC (parseINFO);
};




#endif
