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

/* this file contains functions that are 'static' in nature, and aren't oo in design.
 * Eventually all of these functions will be depreciated, however for the time being
 * I thought it better to have them all in one place, as opposed to polluting the other
 * modules with them.
 *
 * James.
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "exordium/static.h"
#include <aisutil/string.h>

using AISutil::String;
using namespace Exordium;

namespace Exordium
{

   int Static::getServiceAccess(String const &service, String const &nickname)
     {
	if (services.getDatabase().dbSelect("access","access"+String::convert(getRegisteredNickID(nickname))+"' AND service='"+service+"'") < 1 )
	  return 0;
	else
	  return services.getDatabase().dbGetValue().toInt();
     }

   int Static::getRegisteredNickID(String const &nick)
     {
	if (services.getDatabase().dbSelect("id","nicks","nickname='"+nick+"'") < 1)
	  return 0;
	else
	  return services.getDatabase().dbGetValue().toInt();
     }

}
