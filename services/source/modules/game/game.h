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

#ifndef __GAME_H_
# define __GAME_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <kineircd/str.h>
# include <map>


# define GAME_FUNC(x) \
     void x(Exordium::User& origin, LibAIS::StringTokens& line, \
	    const LibAIS::String& channel)


// Forward declarations
class ChannelGame;


// The Game class.. :)
class Game : public Exordium::Service {
 private:
   // Our convenient type-definition of our handler functions
   typedef GAME_FUNC(handler_type);
   
   // Our command table structure
   struct commandTable_type {
      const char* const command;		// The command name
      const handler_type Game::* const handler;	// The function (handler)
   };
   
   // Commands which are sent directly to us
   static const commandTable_type directCommandTable[];
   
   // Commands which are sent to a channel we are on
   static const commandTable_type channelCommandTable[];

   // A list of channel games currently in progress
   typedef std::map < LibAIS::String, ChannelGame* > channelGames_type;
   channelGames_type channelGames;
   
   // Command handlers
   handler_type handleHELP;
   handler_type handleQUOTE;
   handler_type handleSTART;
   handler_type handleLIST;
   
public:
   // Our constructor
   Game(Exordium::Services& s, const LibAIS::String& mn);

   // Our destructor
   ~Game(void)
     {};

   // Return the services thingy
   Exordium::Services& getServices(void)
     { return services; };
   
   // Return our name
   const LibAIS::String& getName(void) const
     { return myName; };
   
   // Parser for incoming stuff
   void parseLine(LibAIS::StringTokens& line, Exordium::User& origin);
   void parseLine(LibAIS::StringTokens& line, Exordium::User& origin,
		  const LibAIS::String& channel);
};

// Complete the forwarded declaration
# include "channelgame.h"

#endif // __GAME_H_
