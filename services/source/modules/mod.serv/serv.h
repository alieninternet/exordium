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

#ifndef _SOURCE_MODULES_SERV_SERV_H_
# define _SOURCE_MODULES_SERV_SERV_H_ 1

# include <aisutil/string.h>

# define SERV_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& tokens)

# include <exordium/service.h>


namespace Exordium {
   namespace ServModule {
      class Service : public Exordium::Service {
       private:
	 struct functionTableStruct {
	    char const* const command;
	    SERV_FUNC((Service::* const function));
	 } static const functionTable[];

	 Exordium::Services& services;
	 
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
			const Kine::ChannelName& channel)
	   {};
	 
	 // Public functions here please!.
	 void addFreeze(Kine::ChannelName const &,AISutil::String const &,
			int const &,AISutil::String const &);
	 int timesFreezed(Kine::ChannelName const &);
	 bool isFreezed(Kine::ChannelName const &);
	 void delFreeze(Kine::ChannelName const &);

       private:
	 SERV_FUNC(parseCLIST);
	 SERV_FUNC(parseDELNICK);
	 SERV_FUNC(parseELIST);
	 SERV_FUNC(parseNLIST);
	 SERV_FUNC(parseHELPON);
	 SERV_FUNC(parseHELP);
	 SERV_FUNC(parseUSER);
	 SERV_FUNC(parseCHAN);
	 SERV_FUNC(parseDIE);
	 SERV_FUNC(parseRAW);
	 SERV_FUNC(parseNEWS);
	 SERV_FUNC(parseSYNCH);
	 SERV_FUNC(parseSETPASS);
	 SERV_FUNC(parseSTATUS);
	 SERV_FUNC(parseCOMMANDS);
	 SERV_FUNC(parseGETHASH);
	 SERV_FUNC(parseFREEZE);
      }; // class Service
   }; // class ServModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_SERV_SERV_H_
