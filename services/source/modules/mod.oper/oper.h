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

#ifndef _SOURCE_MODULES_OPER_OPER_H_
# define _SOURCE_MODULES_OPER_OPER_H_ 1

#include <exordium/service.h>
#include <exordium/services.h>
#include <aisutil/string.h>

#define OPER_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& tokens)


namespace Exordium {
   namespace OperModule {
      class Service : public Exordium::Service {
       private:
	 struct functionTableStruct {
	    char const* const command;
	    OPER_FUNC((Service::* const function));
	 } static const functionTable[];

	 Exordium::Services& services;
	 
       public:
	 Service(const Exordium::Module::ConfigData& config,
		 Exordium::Services& s)
	   : Exordium::Service(config),
	     services(s)
	   {};
	 
	 ~Service(void)
	   {};
	 
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const Kine::ChannelName& channel)
	   {};
	 
       private:
	 OPER_FUNC(parseHELP);
	 OPER_FUNC(parseQUOTE);
         OPER_FUNC(parseJOIN);
         OPER_FUNC(parsePART);
         OPER_FUNC(parseCOMMANDS);
	 OPER_FUNC(parseZLINE);
	 OPER_FUNC(parseQLINE);
	 OPER_FUNC(parseMDEOP);
	 OPER_FUNC(parseGLOBAL);
	 OPER_FUNC(parseKILL);
	 OPER_FUNC(parseJUPE);
	 OPER_FUNC(parseWHOIS);
      }; // class Service
   }; // namespace OperModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_OPER_OPER_H_
