/* $Id$
 * The ChannelGame class - an abstract class for games played on channels
 * 
 * Copyright (c) 2002 Simon Butcher <simonb@alien.net.au>
 *
 * This file is a part of Exordium.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CHANNELGAME_H__
# define __CHANNELGAME_H__

# include <kineircd/str.h>

class ChannelGame;

# include "game.h"

class ChannelGame {
 protected:
   // The game service (so we can send messages)
   Game& game;
   
   // The channel we are playing on
   const Kine::String channel;
   
 public:
   // Constructor
   ChannelGame(Game& g, const Kine::String& c)
     : game(g), 
       channel(c)
       {};
   
   // Our destructor
   virtual ~ChannelGame(void) 
     {};
   
   // Our line parser - where channel commands for us are sent
   virtual bool parseLine(Kine::String& origin, Kine::StringTokens& tokens) 
     = 0;
   
   // Send a message to the channel
   void sendMessage(const Kine::String& message) const
     { game.services.servicePrivmsg(message, game.myName, channel); };
//   void sendMessage(const Kine::String& message) const
//     { game.services.serviceNotice(message, game.myName, channel); };

   // Send a message to someone (specified)
   void sendMessage(const Kine::String& nick, 
		    const Kine::String& message) const
     { game.services.serviceNotice(message, game.myName, nick); };
};
   
#endif // __CHANNELGAME_H__
   
