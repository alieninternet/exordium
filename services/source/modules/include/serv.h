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

#ifndef __SERV_H_
# define __SERV_H_

# include <kineircd/str.h>

# define SERV_FUNC(x)           x(Exordium::User& origin, LibAIS::StringTokens &tokens)

# include "exordium/service.h"
# include "exordium/services.h"

class Serv : public Exordium::Service
{
 private:
   // Module information structure
   static const Exordium::Service::moduleInfo_type moduleInfo;

   // Configuration data class
   Exordium::Service::ConfigData configData;

   struct functionTableStruct
     {
	char const *command;
	void SERV_FUNC ((Serv::* const function));
     };
   static struct functionTableStruct const functionTable[];

 public:
   Serv(void)
     : configData(moduleInfo.fullName, "somewhere.org", "Serv")
       {
       };
   ~Serv(void)
     {
     };
   // Start the module
   void start(Exordium::Services& s);

   // Stop the module (called just before a module is unloaded)
   void stop(void) {};

   void parseLine (LibAIS::StringTokens& line, Exordium::User& origin);
   void parseLine (LibAIS::StringTokens& line, Exordium::User& origin, LibAIS::String const &);

   // Grab the information structure of a module
   virtual const moduleInfo_type& getModuleInfo(void) const
     { return moduleInfo; };

   const Exordium::Service::ConfigData& getConfigData(void) const
     { return configData; };
   Exordium::Service::ConfigData& getConfigData(void)
     { return configData; };

 private:
   void SERV_FUNC (parseCLIST);
   void SERV_FUNC (parseDELNICK);
   void SERV_FUNC (parseELIST);
   void SERV_FUNC (parseNLIST);
   void SERV_FUNC (parseHELPON);
   void SERV_FUNC (parseHELP);
   void SERV_FUNC (parseUSER);
   void SERV_FUNC (parseCHAN);
   void SERV_FUNC (parseDIE);
   void SERV_FUNC (parseRAW);
   void SERV_FUNC (parseNEWS);
   void SERV_FUNC (parseSYNCH);
   void SERV_FUNC (parseSETPASS);
};

#endif
