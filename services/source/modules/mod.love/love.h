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
 *
 * For contact details for the IRCDome Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _SOURCE_MODULES_LOVE_LOVE_H_
# define _SOURCE_MODULES_LOVE_LOVE_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <aisutil/string.h>


# define LOVE_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& line)


namespace Exordium {
   namespace LoveModule {
      // The module of lurrve
      class Service : public Exordium::Service {
       private:
	 // Our convenient type-definition of our handler functions
	 typedef LOVE_FUNC(handler_type);
	 
	 struct commandTable_type {
	    const char* const command;			// Command name
	    handler_type Service::* const handler;	// The function
	 } static const commandTable[];

	 // Where is services? WHERE??!?!?!?!??!?! :(
	 Exordium::Services& services;
	 
	 // Our wonderful command handlers
	 handler_type handleCOMMANDS;

	 // How to send private-messages (stepping-stone)
	 void sendMessage(Exordium::User& recipient,
			  const AISutil::String& message)
	   { recipient.sendMessage(message, getNickname()); };
	 
       public:
	 // Our constructor
	 Service(const Exordium::Module::ConfigData& config,
		 Exordium::Services& s)
	   : Exordium::Service(config),
	     services(s)
	   {};
	 
	 // Our destructor
	 ~Service(void) 
	   {};
   
	 // Parser for incoming stuff sent on a channel
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const Kine::ChannelName& channel)
	   { /* Nothing! Bwa ha ha ha hahahahaHEHhEHahehaheAEhaHAEhaE!!! */ };
	 
	 // Parser for incoming stuff sent directly to us
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
      }; // class Service
   }; // namespace LoveModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_LOVE_LOVE_H_
