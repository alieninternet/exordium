/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __VOTE_H_
#define __VOTE_H_

#include <kineircd/str.h>

# define VOTE_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"
#include "exordium/services.h"

namespace Exordium {

class Vote : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void VOTE_FUNC ((Vote::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
   
public:
   Vote(Exordium::Services& s)
		: Service(), services(s)
	{
		std::cout << "Vote New()" << std::endl;
	};
	~Vote(void)
	{
		std::cout << "Vote Deadeded" << std::endl;
	};
   void parseLine (String const &, String const &);
   void parseLine (String const &, String const &, String const &);
private:
void VOTE_FUNC (parseHELP);

};
};

#endif
