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

#ifndef _SOURCE_MODULES_NICK_NICK_H_
# define _SOURCE_MODULES_NICK_NICK_H_ 1

# include <exordium/services.h>
# include <exordium/service.h>
# include <aisutil/string.h>

# define NICK_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& tokens, \
	    const bool safe)


namespace Exordium {
   namespace NickModule {
      class Service : public Exordium::Service {
       private:
	 Exordium::Services& services;
	 
	 void sendMessage(const AISutil::String& to,
			  const AISutil::String& message)
	   { services.serviceNotice(message,getNickname(),to); };
	 
       public:
	 Service(const Exordium::Module::ConfigData& config,
		 Exordium::Services& s)
	   : Exordium::Service(config),
	     services(s)
	   {};
	 
	 ~Service(void)
	   {};

	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			Kine::ChannelName const &)
	   {};
	 
	 void nickLinkAdd(AISutil::String const &, AISutil::String const &);
	 void nickLinkDel(AISutil::String const &, AISutil::String const &);
	 
	 /* Our Event Handler(s) */
	 void handleClientSignon(Exordium::User& origin);

	 // Return our events mask (events we want to receive)
	 const Events::lazy_type getEventsMask(void) const 
	   {
	      return 
		Events::CLIENT_SIGNON |
		Events::CLIENT_NICKCHANGE;
	   };

	 
	 NICK_FUNC(parseIDENTIFY);
	 NICK_FUNC(parseHELP);
	 NICK_FUNC(parseKILL);
	 NICK_FUNC(parseREGISTER);
	 NICK_FUNC(parseGHOST);
	 NICK_FUNC(parseACCESS);
	 NICK_FUNC(parseSET);
	 NICK_FUNC(parseINFO);
	 NICK_FUNC(parseAUTH);
	 NICK_FUNC(parseNICK);
	 NICK_FUNC(parseCOMMANDS);
      }; // class Service
   }; // namespace NickModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_NICK_NICK_H_
