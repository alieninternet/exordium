#/* $Id$
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

#include "exordium/services.h"
#include "exordium/server.h"
#include "exordium/utils.h"
#include <aisutil/string.h>
#include <kineircd/config.h>

using AIS::Util::String;
using AIS::Util::StringTokens;
using namespace Exordium;


bool Server::isAuthorised(void)
{
if (services.getDatabase().dbSelect("id","serverlist","name='"+name+"'") < 1)
     return false;
   else
     if (services.getDatabase().dbGetValue().toInt() > 0)
       return true;
   else
     return false;
}


void
  Server::addOnlineServer(void)
{
   services.getDatabase().dbInsert("onlineservers","'','"+name+"','"+String::convert(hops)
				   +"','"+desc+"'");
}

void
  Server::delOnlineServer(void)
{
   services.getDatabase().dbDelete("onlineservers", "servername='" + name + "'");
}

