/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __NICK_H_
#define __NICK_H_

#include "exordium/services.h"
#include "exordium/service.h"
#include <kineircd/str.h>

# define NICK_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;



class Nick : public Exordium::Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void NICK_FUNC ((Nick::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
  const String myName;
  void sendMessage(const String &to, const String &message)
	{
		services.serviceNotice(message,myName,to);
	}
public:
   Nick(Exordium::Services& s, const String &mn)
   : services(s), myName(mn)
   {
   };

  ~Nick(void)
   {
   };

  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);

private:

  void NICK_FUNC (parseIDENTIFY);
  void NICK_FUNC (parseHELP);
  void NICK_FUNC (parseKILL);
  void NICK_FUNC (parseREGISTER);
  void NICK_FUNC (parseGHOST);
  void NICK_FUNC (parseACCESS);
  void NICK_FUNC (parseSET);
  void NICK_FUNC (parseINFO);
  void NICK_FUNC (parseAUTH);
};


#endif
