/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __NICK_H_
#define __NICK_H_

#include "kineircd/str.h"

# define NICK_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {


class Nick : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void NICK_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];

public:
  Nick(void)
   : Service()
   {
        std::cout << "Hi im an instance. :(" << std::endl;
   };
  ~Nick(void)
   {
        std::cout << "YOU BASTARD, YOU KILLED NICK" << std::endl;
   };

  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);

private:

  static void NICK_FUNC (parseIDENTIFY);
  static void NICK_FUNC (parseHELP);
  static void NICK_FUNC (parseKILL);
  static void NICK_FUNC (parseREGISTER);
  static void NICK_FUNC (parseGHOST);
  static void NICK_FUNC (parseACCESS);
  static void NICK_FUNC (parseSET);
  static void NICK_FUNC (parseINFO);
  static void NICK_FUNC (parseAUTH);
};

};


#endif
