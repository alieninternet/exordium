/* $Id$
 * The ChannelGame class - an abstract class for games played on channels
 * 
 * Copyright (c) 2002 Simon Butcher <simonb@alien.net.au>
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

#ifndef __CHANNELGAME_H__
# define __CHANNELGAME_H__

# include <kineircd/str.h>

class ChannelGame;

# include "game.h"

# define CHANNEL_GAME_CREATOR_FUNC(x) \
     ChannelGame* x(Game& game, const Kine::String& channel, \
		    const Kine::String& caller)

class ChannelGame {
 protected:
   // The game service (so we can send messages)
   Game& game;
   
   // The channel we are playing on
   const Kine::String channel;
   
 public:
   // Our convenient type-definition of a creator function
   typedef CHANNEL_GAME_CREATOR_FUNC(creator_type);
   
   // Our channel game table structure
   struct channelGameTable_type {
      const char* const game;			// The channel game name
      const creator_type* const creator;	// The creator function
   } static const channelGameTable[];

   // Constructor
   ChannelGame(Game& g, const Kine::String& c)
     : game(g), 
       channel(c)
       {};
   
   // Our destructor
   virtual ~ChannelGame(void) 
     {};
   
   // Our line parser - where channel commands for us are sent
   virtual bool parseLine(const Kine::String& origin,
			  Kine::StringTokens& tokens) 
     = 0;
   
   // Send a message to the channel
   void sendMessage(const Kine::String& message) const
     { game.getServices().servicePrivmsg(message, game.getName(), channel); };

   // Send a message to someone (specified)
   void sendMessage(const Kine::String& nick, 
		    const Kine::String& message) const
     { game.getServices().serviceNotice(message, game.getName(), nick); };
};
   
#endif // __CHANNELGAME_H__
   
