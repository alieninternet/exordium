/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 *
 * This file is a part of Exordium.
 * 
 * Exordium is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Exordium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Exordium; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __LOVE_H_
# define __LOVE_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <kineircd/str.h>


# define LOVE_FUNC(x) \
     void x(Exordium::User& origin, Kine::StringTokens& line)


// The class of lurrve
class Love : public Exordium::Service {
 private:
   // Our convenient type-definition of our handler functions
   typedef LOVE_FUNC(handler_type);
   
   /* Each command gets a little descriptor with the following parameters.
    * Note that in the future the padding field will be used, but for now
    * it is there to make the structure even (4x4 bytes) as processors like
    * to chew on nice neat evenly spaces arrays using some magical multiple
    * of its natural word size (like 4).. Aren't we nice to succumb to its
    * wishes? Well okay, I thought we were at least.. Stop looking at me like
    * that.. Stop it! Stop it..... okay, OKAY - fine.. I'll go then. :(
    */
   struct commandTable_type {
      const char* command;			// Command name
      const unsigned short minParams;		// The min. number of params
      const unsigned short maxParams;		// Maximum parameters
      const static unsigned short MAX_PARAMS_UNLIMITED = 65535; // (any for ^^)
      const handler_type Love::* const handler;	// The function (handler)
      const int padding;			// For future use..
   } static const commandTable[];

   // Where is services?
   Exordium::Services& services;
   
   // Who are we?!
   const String myName;
   
   // Our wonderful command handlers
   handler_type handleCOMMANDS;

   // How to send private-messages (stepping-stone)
   void sendMessage(Exordium::User& recipient, const String& message)
     { recipient.sendMessage(message, myName); };
   
 public:
   // Our constructor
   Love(Exordium::Services& s, const String& mn);

   // Our destructor
   ~Love(void) 
     {};
   
   // Parser for incoming stuff sent on a channel
   void parseLine(Kine::StringTokens& line, Exordium::User& origin,
		  const Kine::String& channel)
     { /* Nothing! Bwa ha ha ha hahahahaHEHhEHahehaheAEhaehHAEhaE!!! */ };
   
   // Parser for incoming stuff sent directly to us
   void parseLine(Kine::StringTokens& line, Exordium::User& origin);
};

#endif // __LOVE_H_
