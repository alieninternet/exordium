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

#ifndef __NICK_H_
#define __NICK_H_

#include "exordium/services.h"
#include "exordium/service.h"
#include <kineircd/str.h>

#define NICK_FUNC(x)           x(Exordium::User& origin, AISutil::StringTokens &tokens)


class Nick : public Exordium::Service
{
private:
   // Module information structure
   static const Exordium::Service::moduleInfo_type moduleInfo;
   
   // Configuration data class
   Exordium::Service::ConfigData configData;

  struct functionTableStruct
  {
    char const *command;
     void NICK_FUNC ((Nick::* const function));
  };
  static struct functionTableStruct const functionTable[];

  void sendMessage(const AISutil::String &to, const AISutil::String &message)
	{
		services->serviceNotice(message,getName(),to);
	}
public:
   Nick(void)
     : configData(moduleInfo.fullName, "somewhere.org", "Nick")
   {
   };

  ~Nick(void)
   {
   };

   // Start the module
   void start(Exordium::Services& s);
   
   void parseLine (AISutil::StringTokens& line, Exordium::User& origin);
   void parseLine (AISutil::StringTokens& line, Exordium::User& origin,AISutil::String const &);

   /* Our Event Handler(s) */
   void handleClientSignon(Exordium::User& origin);
   // Grab the information structure of a module
   virtual const moduleInfo_type& getModuleInfo(void) const
     { return moduleInfo; };

   // Return an appropriate instance of a configuration data class
   const Exordium::Service::ConfigData& getConfigData(void) const
     { return configData; };
   Exordium::Service::ConfigData& getConfigData(void)
     { return configData; };
   
private:

  void NICK_FUNC (parseIDENTIFY);
  void NICK_FUNC (parseHELP);
  void NICK_FUNC (parseKILL);
  void NICK_FUNC (parseREGISTER);
  void NICK_FUNC (parseGHOST);
  void NICK_FUNC (parseACCESS);
  void NICK_FUNC (parseSET);
  void NICK_FUNC (parseINFO);
  void NICK_FUNC (parseAUTH);
};


#endif
