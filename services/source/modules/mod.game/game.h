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

#ifndef _SOURCE_MODULES_GAME_GAME_H_
# define _SOURCE_MODULES_GAME_GAME_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <map>

# include "config.h"

# define GAME_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& line, \
	    const AISutil::String& channel)


// Forward declarations
class ChannelGame;


namespace Exordium {
   namespace GameModule {
      // The Game module
      class Module : public Exordium::Service {
       private:
	 // Our convenient type-definition of our handler functions
	 typedef GAME_FUNC(handler_type);
	 
	 // Module information structure
	 static const Exordium::Service::moduleInfo_type moduleInfo;
	 
	 // Configuration data
	 Game::ConfigData configData;
	 
	 // Our command table structure
	 struct commandTable_type {
	    const char* const command;			// The command name
	    handler_type Module::* const handler;	// The function
	 };
	 
	 // Commands which are sent directly to us
	 static const commandTable_type directCommandTable[];
	 
	 // Commands which are sent to a channel we are on
	 static const commandTable_type channelCommandTable[];
	 
	 // A list of channel games currently in progress
	 typedef std::map < AISutil::String, ChannelGame* > channelGames_type;
	 channelGames_type channelGames;
	 
	 // Command handlers
	 handler_type handleHELP;
	 handler_type handleQUOTE;
	 handler_type handleSTART;
	 handler_type handleLIST;
	 
       public:
	 // Our constructor
	 Module(void)
	   : configData(moduleInfo.fullName)
	   {};
      
	 // Our destructor
	 ~Module(void)
	   {};
	 
	 // Return the services thingy
	 Exordium::Services& getServices(void)
	   { return *services; };
      
	 // Start the module
	 bool start(Exordium::Services& s);
	 
	 // Stop the module (called just before a module is unloaded)
	 void stop(const AISutil::String* const reason = 0);
	 
	 // Parser for incoming stuff
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const Kine::ChannelName& channel);
	 
	 // Grab the information structure of a module
	 virtual const moduleInfo_type& getModuleInfo(void) const
	   { return moduleInfo; };
	 
	 // Return an appropriate instance of a configuration data class
	 const Exordium::Service::ConfigData& getConfigData(void) const
	   { return configData; };
	 Exordium::Service::ConfigData& getConfigData(void)
	   { return configData; };
      }; // class Module
   }; // namespace GameModule
}; // namespace Exordium
   
// Complete the forwarded declaration
# include "channelgame.h"

#endif // _SOURCE_MODULES_GAME_GAME_H_


