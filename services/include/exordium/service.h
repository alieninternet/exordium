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
	 const char* const fullName;			// Full module name
	 const unsigned short versionMajor;		// Version major number
	 const unsigned short versionMinor;		// Version minor number
      };
      
    protected:
      // Where is services?
      Exordium::Services& services;
      
      // Who are we?
      const LibAIS::String myName;
      
    public:
      // Constructor
      Service(Exordium::Services& s, const LibAIS::String& mn)
	: services(s),
          myName(mn)
	{};
      
      // Destructor
      virtual ~Service() 
	{};

      // Parsers
      virtual void parseLine(LibAIS::StringTokens& line, User& origin) = 0;
      
      virtual void parseLine(LibAIS::StringTokens& line, User& origin,
			     const LibAIS::String& channel) = 0;
      
      // Return the nickname of the module
      const LibAIS::String& getName(void) const
	{ return myName; };

      // Grab the information structure of a module
      virtual const moduleInfo_type& getModuleInfo(void) const = 0;
   };
};

#include "exordium/services.h"
#include "exordium/user.h"

#endif

