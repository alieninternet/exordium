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

#ifndef _SOURCE_MODULES_NOTE_NOTE_H_
# define _SOURCE_MODULES_NOTE_NOTE_H_ 1

# ifdef HAVE_CONFIG_H
#  include "autoconf.h"
# endif

# include <kineircd/str.h>
# define NOTE_FUNC(x) \
     void x(Exordium::User& origin, AISutil::StringTokens& tokens)

# include <exordium/service.h>


namespace Exordium {
   namespace NoteModule {
      // The note module
      class Module : public Exordium::Service {
       private:
	 // Module information structure
	 static const Exordium::Service::moduleInfo_type moduleInfo;
	 
	 // Configuration data class
	 Exordium::Service::ConfigData configData;
	 
	 struct functionTableStruct {
	    char const* const command;
	    NOTE_FUNC((Module::* const function));
	 };
	 static const functionTableStruct functionTable[];
	 
       public:
	 Module(void)
	   : configData(moduleInfo.fullName, "somewhere.org", "Note")
	   {};
	 
	 ~Module(void) 
	   {};
	 
	 // Start the module
	 bool start(Exordium::Services& s);
	 
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const bool safe);
	 void parseLine(AISutil::StringTokens& line, Exordium::User& origin,
			const AISutil::String& channel)
	   { return; };
	 
	 // Grab the information structure of a module
	 virtual const moduleInfo_type& getModuleInfo(void) const
	   { return moduleInfo; };
	 
	 // Return an appropriate instance of a configuration data class
	 const Exordium::Service::ConfigData& getConfigData(void) const
	   { return configData; };
	 Exordium::Service::ConfigData& getConfigData(void)
	   { return configData; };
   
       private:
	 NOTE_FUNC(parseSEND);
	 NOTE_FUNC(parseLIST);
	 NOTE_FUNC(parseREAD);
	 NOTE_FUNC(parseDEL);
      }; // class Module
   }; // namespace NoteModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_NOTE_NOTE_H_
