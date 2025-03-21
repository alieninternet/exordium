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

#ifndef _SOURCE_MODULES_MOD_CREDITS_CREDITS_H_
# define _SOURCE_MODULES_MOD_CREDITS_CREDITS_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <exordium/user.h>

# include <aisutil/string.h>

# include <libbank/bank.h>

# define CREDITS_FUNC(x) \
    void x(Exordium::User& origin, AISutil::StringTokens& line, \
           const AISutil::String& channel)

namespace Exordium {
   namespace CreditsModule {
      // The Credits module
      class Service : public Exordium::Service {
       private:
	 // Our convenient type-definition of our handler functions
	 typedef CREDITS_FUNC(handler_type);
	 
	 // Our command table structure
	 struct commandTable_type {
	    const char* const command;                     // The command name
	    handler_type Service::* const handler;    // The function
	 };

	 // Commands which are sent directly to us
	 static const commandTable_type directCommandTable[];
	 
	 // Commands which are sent to a channel we are on
	 static const commandTable_type channelCommandTable[];
	 
	 // Bank 
	 Exordium::Bank bank;

	 // Services
	 Exordium::Services& services;
	 
	 handler_type handleHELP;
	 handler_type handleBALANCE;
   
       public:
	 Service(const Exordium::Module::ConfigData& config,
		Exordium::Services& s)
	   : Exordium::Service(config),
	     bank(s),
	     services(s)
	   {};
	 
	 ~Service(void)
	   {};
	 
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const Kine::ChannelName& channel)
	   {};
      }; // class Service
   }; // namespace CreditsModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_MOD_CREDITS_CREDITS_H_
