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

#ifndef __VOTE_H_
#define __VOTE_H_

#include <kineircd/str.h>

# define VOTE_FUNC(x)           x(LibAIS::String &origin, LibAIS::StringTokens &tokens)

#include "exordium/service.h"
#include "exordium/services.h"

namespace Exordium {

class Vote : public Service
{
private:
   // Module information structure
   static const Exordium::Service::moduleInfo_type moduleInfo;
   
  struct functionTableStruct
  {
    char const *command;
     void VOTE_FUNC ((Vote::* const function));
  };
  static struct functionTableStruct const functionTable[];
   
public:
   Vote(Exordium::Services& s, const LibAIS::String& mn)
     : Service(s, mn)
	{
		std::cout << "Vote New()" << std::endl;
	};
	~Vote(void)
	{
		std::cout << "Vote Deadeded" << std::endl;
	};
   // Start the module
   void start(void);
   
   // Stop the module (called just before a module is unloaded)
   void stop(void) {};
      
   void parseLine (LibAIS::String const &, LibAIS::String const &);
   void parseLine (LibAIS::String const &, LibAIS::String const &, LibAIS::String const &);
   
   // Grab the information structure of a module
   virtual const moduleInfo_type& getModuleInfo(void) const
     { return moduleInfo; };
   
private:
void VOTE_FUNC (parseHELP);

};
};

#endif
