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

#ifndef _EVENTS_H_
# define _EVENTS_H_

# include <kineircd/str.h>
# include <map>

namespace Exordium {
   /* List of events modules can 'sign on' to receive
    * channel events should not be here. 
    * 
    * N.B not complete :-)
    */
   enum event_types
     {
	  EVENT_NONE = 0x00000000,
	  EVENT_NICK = 0x00000001
     };
   typedef unsigned char eventType;
   /* Channel events... not complete */
   enum chan_events 
     {
	  EVENT_CHAN_NONE    = 0x00000000,
	  EVENT_CHAN_JOIN    = 0x00000001,
	  EVENT_CHAN_PART    = 0x00000002,
	  EVENT_CHAN_MODE    = 0x00000004,
	  EVENT_CHAN_TOPIC   = 0x00000008,
	  EVENT_CHAN_KICK    = 0x00000016
     };
   typedef unsigned char chanEventType;
}//end namespace

#endif
