/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __LOVE_H_
# define __LOVE_H_ 1

# include <kineircd/str.h>

# define LOVE_FUNC(x) \
	void x(const Kine::String &origin, const Kine::StringTokens &tokens)

# include "exordium/service.h"


class Love : public Exordium::Service {
 private:
   struct functionTableStruct {
      const char *command;
      const LOVE_FUNC((*function));
   } static const functionTable[];
   
 public:
   // Our constructor
   Love(void)
     {
	std::cout << 
	  "Why hello there, I'm a loving instance :) :)" << std::endl;
     };

   // Our destructor
   ~Love(void) 
     { 
	std::cout << "Share the love, don't trample on it :(" << std::endl; 
     };
   
   // Parser for incoming stuff sent on a channel
   void parseLine(const Kine::String &line, const Kine::String &origin,
		  const Kine::String &channel)
     { /* Nothing! Bwa ha ha ha hahahahaHEHhEHahehaheAEhaehHAEhaE!!! */ };
   
   // Parser for incoming stuff sent directly to us
   void parseLine(const Kine::String &line, const Kine::String &origin);
};

#endif // __LOVE_H_
