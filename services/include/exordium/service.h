/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _INCLUDE_EXORDIUM_SERVICE_H_
# define _INCLUDE_EXORDIUM_SERVICE_H_ 1

# include <string>
# include <aisutil/string.h>
# include <aisutil/config/parser.h>
# include <kineircd/daemon.h>
# include <kineircd/clientname.h>
# include <kineircd/channelname.h>
# include <kineircd/service.h>
# include <exordium/module.h>

namespace Exordium {
   class User;
   class dChan;
   
   class Service : public Kine::Service {
    public:
      // Information about which events we want to receive
      struct Events { // Should be namespace
	 enum type {
	    NONE		    	= 0x00000000,
	    SERVER_NEW	    		= 0x00000001,
	    SERVER_QUIT	    		= 0x00000002,
	    SERVER_BURST	    	= 0x00000004,
	    SERVER_PASS        		= 0x00000008,
	    SERVER_SVINFO      		= 0x00000010,
	    CLIENT_SIGNON      		= 0x00000020,
	    CLIENT_SIGNOFF     		= 0x00000040,
	    CLIENT_MODE        		= 0x00000080,
	    CLIENT_AWAY        		= 0x00000100,
	    CLIENT_NICKCHANGE  		= 0x00000200,
	    CHANNEL_CREATE     		= 0x00000400,
	    CHANNEL_JOIN       		= 0x00000800,
	    CHANNEL_DESTROY    		= 0x00001000,
	    CHANNEL_TOPIC      		= 0x00002000,
	    CHANNEL_KICK       		= 0x00004000,
	    CHANNEL_OP         		= 0x00008000,
	    CHANNEL_DEOP       		= 0x00010000,
	    CHANNEL_VOICE      		= 0x00020000,
	    CHANNEL_DEVOICE    		= 0x00040000,
	    CHANNEL_HALFOP     		= 0x00080000,
	    CHANNEL_HALFDEOP   		= 0x00100000,
	    CHANNEL_BAN    		= 0x00200000,
	    CHANNEL_EXEMPT    		= 0x00400000,
	    CHANNEL_MODE     		= 0x00800000,
	    CHANNEL_PART       		= 0x01000000,
	    SERVER_MODE			= 0x02000000,
	    ALL				= 0xFFFFFFFF /* Psycho :-) */
	 };
	 
	 typedef unsigned int lazy_type;
      };
      
    private:
      const std::string& description;
      const std::string& username;
      
    protected:
      // Constructor
      Service(const Module::ConfigData& config)
	: Kine::Service(config.getName(),
			config.getHostname(),
			Kine::daemon().getTime()),
          description(config.getDescription()),
          username(config.getIdent())
	{};
      
    public:
      // Destructor
      virtual ~Service() 
	{};

      // Parsers
      virtual void parseLine(AISutil::StringTokens& line, User& origin,
			     const bool safe) = 0;
      
      virtual void parseLine(AISutil::StringTokens& line, User& origin,
			     const Kine::ChannelName& channel) = 0;
      
      
      // Return the events mask (the events we want to know about)
      virtual const Events::lazy_type getEventsMask(void) const
	{ return Events::NONE; };
      
      // Event handler things
      virtual void handleAway(User& origin, const AISutil::String& reason) {} ;
      virtual void handleClientSignon(User& origin) {};
      virtual void handleTopic(const AISutil::String& origin, dChan& channel, const AISutil::String& newTopic) {};
      virtual void handleChannelJoin(User& origin, dChan& channel, const int& status) {};
      virtual void handleChannelPart(User& origin, dChan& channel) {};
      virtual void handleChannelMode(dChan& channel, const AISutil::String& modes,
                                     const AISutil::String& target, const AISutil::String& source) {};
      
      
      // Return the username/identity of the service
      const std::string& getUsername(void) const
	{ return username; };
      
      // Return the description of this service module
      const std::string& getDescription(void) const
	{ return description; };
   };
};

#include <exordium/services.h>
#include <exordium/user.h>

#endif // _INCLUDE_EXORDIUM_SERVICE_H_

