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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <aisutil/string.h>
#include "console.h"
#include "core_language.h"
#include <assert.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;

const struct Console::functionTableStruct Console::functionTable[] = {
     { "module",		&Console::parseMODULE },
     { "language",		&Console::parseLANGUAGE },
     { "help",			&Console::parseHELP },
     { 0, 0 }
};


void Console::parseLine(const String &line, const String &requestor) 
{
   StringTokens st (line);
   Kine::Name origin = requestor;
   String command = st.nextToken ().toLower ();
   User *ptr = services.findUser(origin);

   for (int i = 0; functionTable[i].command != 0; i++) {
      // Does this match?
      if (command == functionTable[i].command) {
 int required = services.getRequiredAccess(services.getConfigInternal().getConsoleName(),command.toLower());
int access = ptr->getAccess(services.getConfigInternal().getConsoleName());
             if(required>access)
               {
  ptr->sendMessage(GETLANG(core_CONSOLE_NOT_ENOUGH_ACCESS),services.getConfigInternal().getConsoleName());
  String togo = ptr->getNickname()+" tried to use \002"+command+"\002";
  services.logLine(togo, Log::Warning);
  return;
               }

	 // Run the command and leave
	 (this->*(functionTable[i].function))(origin, st);
	 return;
      }
   }
   
   services.serviceNotice("Unrecognized Command", 
			  services.getConfigInternal().getConsoleName(),
			  requestor);
}


void CONSOLE_FUNC(Console::parseMODULE)
{
   String command = tokens.nextToken();
   
   if(command == "list") {
      String foo = services.getConfigInternal().getModules().dumpModules();
      services.serviceNotice("The currently loaded service modules are:",
			     services.getConfigInternal().getConsoleName(),
			     origin);
      services.serviceNotice(foo,
			     services.getConfigInternal().getConsoleName(),
			     origin);
      return;
   }
   
   if (command == "unload") {
      String name = tokens.nextToken();
      services.serviceNotice("Unloading module " + name,
			     services.getConfigInternal().getConsoleName(),
			     origin);
      services.getConfigInternal().getModules().unloadModule(name,
							     origin +
							     " unloaded me :(");
      services.sendGOper(services.getConfigInternal().getConsoleName(),origin+ " \002unloaded\002 module " + name);
      return;
   }
   
   if(command == "load") {
//    String name = tokens.nextToken();
      (void)tokens.nextToken();
      String filename = tokens.nextToken();
      services.serviceNotice("Attempting to load module",
			     services.getConfigInternal().getConsoleName(),
			     origin);
      String errString;
      Service* const service = 
	services.getConfigInternal().getModules().loadModule(filename,
							     errString);
      
      if (service == 0) {
	 services.serviceNotice("Error loading module: " + errString,
				services.getConfigInternal().getConsoleName(),
				origin);
	 return;
      }
	
      // Start the module, now that it has been loaded..
      service->start(services);
      
      services.serviceNotice("Module loaded successfully",
			     services.getConfigInternal().getConsoleName(),
			     origin);
      
      services.sendGOper(services.getConfigInternal().getConsoleName(),origin+" \002loaded\002 module " + filename);
   }
}


void CONSOLE_FUNC(Console::parseLANGUAGE)
{
   String command = tokens.nextToken().toLower();
   User *ptr = services.findUser(origin); // <=- ick. just for GETLANG() ??
   
   if (command == "load") {
      std::string file = tokens.nextToken();
      std::string errString;
      
      if (Kine::langs().loadFile(file, errString)) {
	 services.serviceNotice(GETLANG(core_CONSOLE_LANGUAGE_LOADED,
					file),
				services.getConfigInternal().getConsoleName(),
				origin);
      } else {
	 services.serviceNotice(GETLANG(core_CONSOLE_LANGUAGE_COULDNT_LOAD,
					file, errString),
				services.getConfigInternal().getConsoleName(),
				origin);
      }
   }
}


void CONSOLE_FUNC(Console::parseHELP)
{
   // The thingy that sends the data (the operator() bit does the work)
   struct Caller : public Kine::Languages::callFunction_type {
      // Remember stuff..
      const Kine::Name& origin;
      ServicesInternal& services;
      
      bool operator()(const std::string& data)
	{
	   // Send the line
	   services.serviceNotice(data,
				  services.getConfigInternal().getConsoleName(),
				  origin);
	   
	   // We return true here, because we're willing to accept more data
	   return true;
	};

      Caller(const Kine::Name& o, ServicesInternal& s)
	: origin(o), services(s)
	{};
   };
   
   // Create one of those call functions..
   Caller callout(origin, services);
   
   // This shold probably be passed to us..
   User *ptr = services.findUser(origin); 
   assert(ptr != 0);
   
   /* Send the right tag.. however, in normal use for exordium's help engine,
    * there would be a heavy requirement for some autogen magic to set up
    * the help topics vs. commands and link the tags to the tag map to the
    * commands.. I'll leave this up to you to invent, since it's relatively
    * straight forward. AutoGen should probably be employed, regardless, to
    * maintain the command lists for the console (and indeed the modules).
    * 
    * Currently, mod_irc2user for kine uses this method - command lists have
    * references to segments in the tag map arrays in order for appropriate
    * help information to be obtained for the given command. (This has been in
    * place for quite some time).
    * 
    * This will call the call-out function for each segment of the language
    * data found at the given tag..
    */
   Kine::langs().get(ptr->getLanguage(),
		     Language::tagMap[Language::core_CONSOLE_HELP_HELP].tagID,
		     callout);
}
