/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _SOURCE_MODULES_GAME_CHANNELGAME_H_
# define _SOURCE_MODULES_GAME_CHANNELGAME_H_ 1

# include <exordium/user.h>

class ChannelGame;

# include "game.h"

# define CHANNEL_GAME_CREATOR_FUNC_NO_EXTERN(x) \
     ChannelGame* x(Exordium::GameModule::Module& module, \
		    const AIS::Util::String& channel, Exordium::User& caller)

# define CHANNEL_GAME_CREATOR_FUNCTION \
     extern "C" CHANNEL_GAME_CREATOR_FUNC_NO_EXTERN(game_init)


class ChannelGame {
 protected:
   // The game service (so we can send messages)
   Exordium::GameModule::Module& module;
   
   // The channel we are playing on
   const AIS::Util::String channel;
   
 public:
   // Our convenient type-definition of a creator function
   typedef CHANNEL_GAME_CREATOR_FUNC_NO_EXTERN(creator_type);
   
   // Our channel game table structure
   struct channelGameTable_type {
      const char* const game;			// The channel game name
      creator_type* const creator;	// The creator function
   } static const channelGameTable[];

   // Constructor
   ChannelGame(Exordium::GameModule::Module& m, const AIS::Util::String& c)
     : module(m),
       channel(c)
       {};
   
   // Our destructor
   virtual ~ChannelGame(void) 
     {};
   
   // Our line parser - where channel commands for us are sent
   virtual bool parseLine(Exordium::User& origin, AIS::Util::String& command,
			  AIS::Util::StringTokens& tokens) = 0;

   // Grab the channel this channel game is on
   const AIS::Util::String& getChannel(void) const
     { return channel; };
   
   // Send a message to the channel
   void sendMessage(const AIS::Util::String& message) const
     {
	module.getServices().servicePrivmsg(message, module.getNickname(), 
					    channel);
     };

   // Send a message to someone (specified)
   void sendMessage(Exordium::User& nick,
		    const AIS::Util::String& message) const
     { nick.sendMessage(message, module.getNickname()); };
};
   
#endif // _SOURCE_MODULES_GAME_CHANNELGAME_H_
