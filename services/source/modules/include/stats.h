/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __STATS_H_
#define __STATS_H_

#include <kineircd/str.h>

# define STATS_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"
#include "exordium/services.h"

namespace Exordium {

class Stats : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void STATS_FUNC ((Stats::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
   
public:
   Stats(Exordium::Services& s)
		: Service(), services(s)
	{
		std::cout << "Stats New()" << std::endl;
	};
	~Stats(void)
	{
		std::cout << "Stats Deadeded" << std::endl;
	};
   void parseLine (String const &, String const &);
   void parseLine (String const &, String const &, String const &);
private:
void STATS_FUNC (parseHELP);

};
};

#endif
