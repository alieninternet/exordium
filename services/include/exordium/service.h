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
# include <kineircd/service.h>

extern "C" {
# include <sys/time.h>
};

// Definitions for the module init functions..
#define EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN(x) \
   Exordium::Service* x(void)

#define EXORDIUM_SERVICE_INIT_FUNCTION \
   extern "C" EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN(service_init)

namespace Exordium {
   class Services;
   class User;
   class dChan;
   
   class Service : public Kine::Service {
    public:
      struct moduleInfo_type {
	 // Version information about the module
	 const char* const fullName;
	 const unsigned short versionMajor;
	 const unsigned short versionMinor;

	 // Information about which events we want to receive
	 struct Events { // Should be namespace, but g++ is dumb
	    enum {
	         NONE		    = 0x00000000,
	         SERVER_NEW	    = 0x00000001,
	         SERVER_QUIT	    = 0x00000002,
	         SERVER_BURST	    = 0x00000004,
	         SERVER_PASS        = 0x00000008,
	         SERVER_SVINFO      = 0x00000010,
		 CLIENT_SIGNON      = 0x00000020,
		 CLIENT_SIGNOFF     = 0x00000040,
		 CLIENT_MODE        = 0x00000080,
		 CLIENT_AWAY        = 0x00000100,
		 CLIENT_NICKCHANGE  = 0x00000200,
		 CHANNEL_CREATE     = 0x00000400,
		 CHANNEL_JOIN       = 0x00000800,
		 CHANNEL_DESTROY    = 0x00001000,
		 CHANNEL_TOPIC      = 0x00002000,
		 CHANNEL_KICK       = 0x00004000,
		 CHANNEL_OP         = 0x00008000,
		 CHANNEL_DEOP       = 0x00010000,
		 CHANNEL_VOICE      = 0x00020000,
		 CHANNEL_DEVOICE    = 0x00040000,
		 CHANNEL_HALFOP     = 0x00080000,
		 CHANNEL_HALFDEOP   = 0x00100000,
		 CHANNEL_BAN        = 0x00200000,
		 CHANNEL_EXEMPT     = 0x00400000,
		 CHANNEL_MODE       = 0x00800000,
                 CHANNEL_PART       = 0x01000000,
		 SERVER_MODE        = 0x02000000,
	         ALL	            = 0xFFFFFFFF /* Psycho :-) */
	    };
	 };
	 const unsigned int eventsMask; // values OR'd from above
      };
      unsigned int getEventsMask(void);

      
      class ConfigData : public AISutil::ConfigData {
       private:
	 // Configuration information (this is a default list for this base)
	 static const AISutil::ConfigParser::defTable_type defaultDefinitions;
	 
       protected:
	 AISutil::String defDescription;		// Our description
	 AISutil::String defDistribution;		// Our scope mask
	 AISutil::String defHostname;			// Our hostname
	 Kine::Name defName;				// Our name
	 AISutil::String defIdent;			// Out ident
	 
       public:
	 // Constructor
	 ConfigData(const AISutil::String& d, const AISutil::String& h,
		    const AISutil::String& n, const AISutil::String& u)
	   : defDescription(d),
	     defDistribution("*"), // <=- network-wide distribution scope
	     defHostname(h),
	     defName(n),
	     defIdent(u)
	   {};

	 // Destructor
	 virtual ~ConfigData(void)
	   {};

	 // Grab the configuration definition table
	 virtual const AISutil::ConfigParser::defTable_type& 
	   getDefinitions(void) const
	   { return defaultDefinitions; };
	 
	 // Return variables..
	 const AISutil::String& getDescription(void) const
	   { return defDescription; };
	 const AISutil::String& getDistribution(void) const
	   { return defDistribution; };
	 const AISutil::String& getHostname(void) const
	   { return defHostname; };
	 const Kine::Name& getName(void) const
	   { return defName; };
	 const AISutil::String& getIdent(void) const
           { return defIdent; };
      };
      
    protected:
      /* Where is services? This is a pointer because we will not know
       * where services is upon initialisation of the class..
       */
      Exordium::Services* services;
      
      // The sign-on time for this module
      timeval signonTime;
      
    public:
      // Constructor
      Service(void)
	{
	   // Urgh :( Set the sign-on time to "now"
	   (void)gettimeofday(&signonTime, NULL);
	};
      
      // Destructor
      virtual ~Service() 
	{};

      // Start the module (return false if the module is unable to start)
      virtual bool start(Exordium::Services& s) = 0;
      
      // Stop the module (called just before a module is unloaded)
      virtual void stop(const AISutil::String& reason) {};

      // Parsers
      virtual void parseLine(AISutil::StringTokens& line, User& origin,
			     const bool safe) = 0;
      
      virtual void parseLine(AISutil::StringTokens& line, User& origin,
			     const Kine::Name& channel) = 0;
      
      virtual void handleAway(User& origin, const AISutil::String& reason) {} ;
      virtual void handleClientSignon(User& origin) {};
      virtual void handleTopic(const AISutil::String& origin, dChan& channel, const AISutil::String& newTopic) {};
      virtual void handleChannelJoin(User& origin, dChan& channel, const int& status) {};
      virtual void handleChannelPart(User& origin, dChan& channel) {};
      virtual void handleChannelMode(dChan& channel, const AISutil::String& modes,
                                     const AISutil::String& target, const AISutil::String& source) {};
      
      
      // Grab the information structure of a module
      virtual const moduleInfo_type& getModuleInfo(void) const = 0;
      
      // Return an appropriate instance of a configuration data class
      virtual const ConfigData& getConfigData(void) const = 0;
      virtual ConfigData& getConfigData(void) = 0;

      /*
       * 
       * The follow is defined as virtuals from Kine::Service and need to be
       * here upon inheriting from Kine..
       *						- pickle
       * 
       */

      // Return the nickname of the module
      const Kine::Name& getNickname(void) const
	{ return getConfigData().getName(); };
      
      // Return the username/identity of the service
      const std::string& getUsername(void) const
	{ return getConfigData().getIdent(); };

      // Return the hostname of this service module
      const std::string& getHostname(void) const
	{ return getConfigData().getHostname(); };
      
      // Return the description of this service module
      const std::string& getDescription(void) const
	{ return getConfigData().getDescription(); };
      
      // Return the time this module was created/signed on to the network
      const timeval& getSignonTime(void) const
	{ return signonTime; };
   };
};

#include <exordium/services.h>
#include <exordium/user.h>

#endif // _INCLUDE_EXORDIUM_SERVICE_H_

