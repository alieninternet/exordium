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

#ifndef _SOURCE_MODULES_CHAN_CHAN_H_
# define _SOURCE_MODULES_CHAN_CHAN_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <exordium/user.h>
# include <exordium/dchan.h>
# include <exordium/database/database.h>

# include <aisutil/string.h>

# define CHAN_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& tokens, \
	       AISutil::String &chan)


namespace Exordium {
   namespace ChanModule {
      class Module : public Exordium::Service {
       private:
	 // Module information structure
	 static const Exordium::Service::moduleInfo_type moduleInfo;
	 
	 // Configuration data class
	 Exordium::Service::ConfigData configData;
	 
	 struct functionTableStruct {
	    char const* const command;
	    CHAN_FUNC((Module::* const function));
	 } static const functionTable[];
	 CDatabase* db;
       public:
	 Module(void)
	   : configData(moduleInfo.fullName, "exordium.org", "Chan","chan")
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
			const Kine::Name& channel);
	 
	 void handleAway(Exordium::User& origin, 
			 const AISutil::String& reason);

         void handleTopic(const Kine::Name&, dChan& channel, const AISutil::String &);

         void handleChannelJoin(User& origin, dChan &channel, const int &status);
         void handleChannelPart(User& origin, dChan &channel);

         void handleChannelMode( dChan &channel, const AISutil::String &modes,
                                 const AISutil::String &target, const AISutil::String &source );
	 
	 // Grab the information structure of a module
	 virtual const moduleInfo_type& getModuleInfo(void) const
	   { return moduleInfo; };

	 // Return an appropriate instance of a configuration data class
	 const Exordium::Service::ConfigData& getConfigData(void) const
	   { return configData; };
	 Exordium::Service::ConfigData& getConfigData(void)
	   { return configData; };
	 
	 
	 
       private:
	 CHAN_FUNC(parseHELP);
	 CHAN_FUNC(parseACCESS);
	 CHAN_FUNC(parseKICK);
	 CHAN_FUNC(parseOP);
	 CHAN_FUNC(parseVOICE);
	 CHAN_FUNC(parseREGISTER);
	 CHAN_FUNC(parseBAN);
	 CHAN_FUNC(parseDEOP);
	 CHAN_FUNC(parseTOPIC);
	 CHAN_FUNC(parseDEVOICE);
	 CHAN_FUNC(parseADDUSER);
	 CHAN_FUNC(parseINFO);
	 CHAN_FUNC(parseLISTBAN);
	 CHAN_FUNC(parseSET);
	 CHAN_FUNC(parseSEEN);
	 CHAN_FUNC(parseCOMMANDS);
         CHAN_FUNC(parseDROP);

	 bool isFreezed(Kine::Name const &);

      }; // class Module
   }; // namespace ChanModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_CHAN_CHAN_H_
