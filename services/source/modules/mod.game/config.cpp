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

#include "config.h"


// Game module configuration table
const AISutil::ConfigParser::defTable_type Game::ConfigData::defTable = {
     {
	"DESCRIPTION", 4,
	  (void *)&ConfigData::defDescription, &varHandleString,
	  0, 0
     },
     {
	"DISTRIBUTION", 4,
	  (void *)&ConfigData::defDistribution, &varHandleStringOneWord,
	  0, 0
     },
     {
	"HOSTNAME", 4,
	  (void *)&ConfigData::defHostname, &varHandleHostName,
	  0, 0
     },
     {
	"NAME", 4,
	  (void *)&ConfigData::defName, &varHandleStringOneWord,
	  0, 0
     },
     {
	"MODULE", 3,
	  (void *)&ConfigData::defModule, &varHandleModule,
	  0, 0
     },
     { 0, 0, 0, 0, 0, 0 }
};

LIBAISUTIL_CONFIG_VARIABLE_HANDLER(Game::ConfigData::varHandleModule)
{
   // Check if the first value is empty (the filename field)
   if (values.front().empty()) {
      // Get cranky
      errString = "No module filename supplied!";
      return false;
   }
   
   std::cerr << "---------> " << values.front() << std::endl;
   /*
   void* const handle = dlopen(values.front().c_str(), RTLD_NOW);
   if(handle == 0)
   {
      errString = "Could not load " + values.front() + ": " + dlerror();
      return 0;
   }

   Factory::Instance().registerType(values.front(),);
   */
   
   return true;
}
