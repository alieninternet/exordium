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
      class Module : public Exordium::Service {
       private:
	 // Module information structure
	 static const Exordium::Service::moduleInfo_type moduleInfo;
	 
	 // Configuration data class
	 Exordium::Service::ConfigData configData;
	 
	 void sendMessage(const AISutil::String& to,
			  const AISutil::String& message)
	   { services->serviceNotice(message,getNickname(),to); };
	 
       public:
	 Module(void)
	   : configData(moduleInfo.fullName, "peoplechat.org", "Nick", "nick")
	   {};
	 
	 ~Module(void)
	   {};

	 // Start the module
	 bool start(Exordium::Services& s);
	 
	 // Stop the module (called just before a module is unloaded)
	 void stop(const AISutil::String& reason);
	 
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			Kine::Name const &)
	   {};
	 
	 void nickLinkAdd(AISutil::String const &, AISutil::String const &);

	 /* Our Event Handler(s) */
	 void handleClientSignon(Exordium::User& origin);
	 
	 // Grab the information structure of a module
	 virtual const moduleInfo_type& getModuleInfo(void) const
	   { return moduleInfo; };
	 
	 // Return an appropriate instance of a configuration data class
	 const Exordium::Service::ConfigData& getConfigData(void) const
	   { return configData; };
	 Exordium::Service::ConfigData& getConfigData(void)
	   { return configData; };
	 
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
      }; // class Module
   }; // namespace NickModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_NICK_NICK_H_
