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

#ifndef SERVICE_H
# define SERVICE_H

# include <kineircd/str.h>
# include <libais/config/parser.h>

// Definitions for the module init functions..
#define EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN(x) \
   Service* x(Exordium::Services& services, const LibAIS::String& name)

#define EXORDIUM_SERVICE_INIT_FUNCTION \
   extern "C" EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN(service_init)

namespace Exordium {
   class Services;
   class User;
   
   class Service {
    public:
      struct moduleInfo_type {
	 // Version information about the module
	 const char* const fullName;
	 const unsigned short versionMajor;
	 const unsigned short versionMinor;
      };
      
      class ConfigData : public LibAIS::ConfigData {
       public:
	 // Configuration information (this is a default list for this base)
	 static const LibAIS::ConfigParser::defTable_type defaultDefinitions;
	 
       private:
	 LibAIS::String defDescription;			// Our description
	 LibAIS::String defHostname;			// Our hostname
	 LibAIS::String defName;			// Our name
	 
       public:
	 // Constructor
	 ConfigData(const LibAIS::String& d, const LibAIS::String& h,
		    const LibAIS::String& n)
	   : defDescription(d),
	     defHostname(h),
	     defName(n)
	   {};

	 // Destructor
	 virtual ~ConfigData(void)
	   {};

	 // Grab the configuration definition table
	 virtual const LibAIS::ConfigParser::defTable_type& 
	   getDefinitions(void) const
	   { return defaultDefinitions; };
	 
	 // Return variables..
	 const LibAIS::String& getDescription(void) const
	   { return defDescription; };
	 const LibAIS::String& getHostname(void) const
	   { return defHostname; };
	 const LibAIS::String& getName(void) const
	   { return defName; };
      };
      
    protected:
      // Where is services?
      Exordium::Services& services;
      
    public:
      // Constructor
      Service(Exordium::Services& s)
	: services(s)
	{};
      
      // Destructor
      virtual ~Service() 
	{};

      // Start the module
      virtual void start(void) = 0;
      
      // Stop the module (called just before a module is unloaded)
      virtual void stop(void) = 0;
      
      // Parsers
      virtual void parseLine(LibAIS::StringTokens& line, User& origin) = 0;
      
      virtual void parseLine(LibAIS::StringTokens& line, User& origin,
			     const LibAIS::String& channel) = 0;
      
      // Grab the information structure of a module
      virtual const moduleInfo_type& getModuleInfo(void) const = 0;
      
      // Return an appropriate instance of a configuration data class
      virtual const ConfigData& getConfigData(void) const = 0;
      virtual ConfigData& getConfigData(void) = 0;

      // Return the nickname of the module (for lazy people)
      const LibAIS::String& getName(void) const
	{ return getConfigData().getName(); };
   };
};

#include "exordium/services.h"
#include "exordium/user.h"

#endif

