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

#ifndef __LOVE_H_
# define __LOVE_H_ 1

# include <exordium/service.h>
# include <exordium/services.h>
# include <kineircd/str.h>


# define LOVE_FUNC(x) \
     void x(Exordium::User& origin, LibAIS::StringTokens& line)


// The class of lurrve
class Love : public Exordium::Service {
 private:
   // Our convenient type-definition of our handler functions
   typedef LOVE_FUNC(handler_type);
   
   // Module information structure
   static const Exordium::Service::moduleInfo_type moduleInfo;
   
   // Configuration data class
   Exordium::Service::ConfigData configData;
   
   /* Each command gets a little descriptor with the following parameters.
    * Note that in the future the padding field will be used, but for now
    * it is there to make the structure even (4x4 bytes) as processors like
    * to chew on nice neat evenly spaces arrays using some magical multiple
    * of its natural word size (like 4).. Aren't we nice to succumb to its
    * wishes? Well okay, I thought we were at least.. Stop looking at me like
    * that.. Stop it! Stop it..... okay, OKAY - fine.. I'll go then. :(
    */
   struct commandTable_type {
      const char* command;			// Command name
      const unsigned short minParams;		// The min. number of params
      const unsigned short maxParams;		// Maximum parameters
      const static unsigned short MAX_PARAMS_UNLIMITED = 65535; // (any for ^^)
      const handler_type Love::* const handler;	// The function (handler)
      const int padding;			// For future use..
   } static const commandTable[];

   // Our wonderful command handlers
   handler_type handleCOMMANDS;

   // How to send private-messages (stepping-stone)
   void sendMessage(Exordium::User& recipient, const LibAIS::String& message)
     { recipient.sendMessage(message, getName()); };
   
 public:
   // Our constructor
   Love(Exordium::Services& s)
     : Exordium::Service(s),
       configData(moduleInfo.fullName, "somewhere.org", "Love")
     {};

   // Our destructor
   ~Love(void) 
     {};
   
   // Start the module
   void start(void);
   
   // Stop the module (called just before a module is unloaded)
   void stop(void) 
     {};
      
   // Parser for incoming stuff sent on a channel
   void parseLine(LibAIS::StringTokens& line, Exordium::User& origin,
		  const LibAIS::String& channel)
     { /* Nothing! Bwa ha ha ha hahahahaHEHhEHahehaheAEhaehHAEhaE!!! */ };
   
   // Parser for incoming stuff sent directly to us
   void parseLine(LibAIS::StringTokens& line, Exordium::User& origin);

   // Grab the information structure of a module
   virtual const moduleInfo_type& getModuleInfo(void) const
     { return moduleInfo; };

   // Return an appropriate instance of a configuration data class
   const Exordium::Service::ConfigData& getConfigData(void) const
     { return configData; };
   Exordium::Service::ConfigData& getConfigData(void)
     { return configData; };
};

#endif // __LOVE_H_
