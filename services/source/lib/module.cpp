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

#include <kineircd/registry.h>

#include "exordium/module.h"

using namespace Exordium;

// Default configuration definition table (for uninherited ConfigData classes)
const AISutil::ConfigParser::defTable_type
  Module::ConfigData::defaultDefinitions = {
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
	  "IDENT", 5,
	    (void *)&ConfigData::defIdent, &varHandleStringOneWord,
	    0, 0
       },
     
       { 0, 0, 0, 0, 0, 0 }
  };


/* ~Module - Destroy the module
 * Original 30/05/2003 pickle
 */
Module::~Module(void)
{
   // If a service was created, destroy it on the module's behalf
   if (service != 0) {
      delete service;
   }
}

  
/* start - Start the module
 * Original 30/05/2003 pickle
 */
const bool Module::start(Exordium::Services& s)
{
   // Try to start the module, and make sure a service was created
   if ((service = realStart(s)) != 0) {
      // Okay, the module started - register the service to kine here
      return (Kine::registry().addService(*service) != Kine::Error::NO_ERROR);
   }
   
   // If we got here, the module did not start.. ouch..
   return false;
}


/* stop - Stop the module
 * Original 30/05/2003 pickle
 */
void Module::stop(const AISutil::String* const reason)
{
   // Stop the module
   realStop(reason);
   
   // Deregister the service from kine, if it existed
   if (service != 0) {
      (void)Kine::registry().removeService(*service);
   }
}



