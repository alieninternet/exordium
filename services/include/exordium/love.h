/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __LOVE_H_
#define __LOVE_H_

#include "kineircd/str.h"
# define LOVE_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {


class Love : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void LOVE_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];

public:
  Love(void)
   : Service()
   {
	std::cout << "Hi im an instance. :(" << std::endl;
   };
  ~Love(void) 
   {
	std::cout << "YOU BASTARD, YOU KILLED LOVE" << std::endl;
   };   
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
};






};










#endif
