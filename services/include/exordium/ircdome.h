/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __IRCDOME_H_
#define __IRCDOME_H_

#include <kineircd/str.h>

# define IRCDOME_FUNC(x) \
     x(Kine::String &origin, Kine::StringTokens &tokens)

namespace Exordium {
   class Services;
   
   class IRCDome {
    private:
      Services& services;
      
      struct functionTableStruct {
	 const char* command;
	 void IRCDOME_FUNC((IRCDome::* const function));
      } static const functionTable[];
      
      void IRCDOME_FUNC(parseMODULE);
      
    public:
      IRCDome(Services& s)
	: services(s)
	{};
      
      void parseLine(const String &, const String &);
   };
};

// Complete the forwarded definition
# include "exordium/services.h"

#endif
