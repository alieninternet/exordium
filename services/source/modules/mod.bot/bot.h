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

#ifndef __BOT_H_
#define __BOT_H_

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <exordium/service.h>
#include <exordium/services.h>
#include <exordium/user.h>

#include <kineircd/str.h>


#define BOT_FUNC(x)           x(Exordium::User& origin, AISutil::StringTokens &tokens, AISutil::String &chan)


class Bot : public Exordium::Service
{
   class Exordium::User;
 private:
   // Module information structure
   static const Exordium::Service::moduleInfo_type moduleInfo;
   
   // Configuration data class
   Exordium::Service::ConfigData configData;
   
  struct functionTableStruct
  {
    char const *command;
     void BOT_FUNC ((Bot::* const function));
  };
  static struct functionTableStruct const functionTable[];

  void sendMessage(const AISutil::String &to, const AISutil::String &message)
	{
		services->serviceNotice(message,getName(),to);
	};
public:
   Bot(void)
     : configData(moduleInfo.fullName, "somewhere.org", "Bot")
       {};

  ~Bot(void)
	{
#ifdef DEBUG
		std::cout << "Dead Bot" << std::endl;
#endif
	};
   
   // Start the module
   void start(Exordium::Services& s);
   
   void parseLine(AISutil::StringTokens& line, Exordium::User& origin);
   
   void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
		  const AISutil::String& channel);
   

   // Grab the information structure of a module
   virtual const moduleInfo_type& getModuleInfo(void) const
     { return moduleInfo; };

   // Return an appropriate instance of a configuration data class
   const Exordium::Service::ConfigData& getConfigData(void) const
     { return configData; };
   Exordium::Service::ConfigData& getConfigData(void)
     { return configData; };
   
private:
  void BOT_FUNC (parseHELP);
  void BOT_FUNC (parseQUOTE);
};




#endif
