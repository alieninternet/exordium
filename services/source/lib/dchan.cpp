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

#include "exordium/services.h"
#include "exordium/dchan.h"
#include <kineircd/str.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;


void const dChan::addUser(User &ptr, const int &status)
{
services.getDatabase().dbDelete("chanstatus","chanid=" + getOnlineIDString() + " AND nickid="+String::convert(ptr.getOnlineID()));
services.getDatabase().dbInsert("chanstatus","'"+getOnlineIDString()+"','"+ptr.getOnlineIDString()+"','"+String::convert(status)+"'");
}

void const dChan::delUser(User &ptr)
{
services.getDatabase().dbDelete("chanstatus","chanid=" + getOnlineIDString() + " AND nickid="+ptr.getOnlineIDString());

}

int const dChan::getChanCount(void)
{
int nbRes = services.getDatabase().dbSelect("nickid","chanstatus","chanid='"+getOnlineIDString()+"'");
return nbRes;
}

bool const dChan::isRegistered(void)
{
if (services.getDatabase().dbSelect("id","chans","name='"+name.toLower()+"'") < 1)
     return false;
if (services.getDatabase().dbGetValue().toInt() > 0)
     return true;
   else
     return false;
}

int const dChan::getAccess(String const &who)
{
     if(services.getDatabase().dbSelect("access","chanaccess",
				      "chanid='"+String::convert(onlineID)+"' AND nickid='"
				      +String::convert(services.getRegisteredNickID(who))+"'") < 1)
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

void const dChan::setChanLog(bool const &value)
{
if(value)
     {
	services.getDatabase().dbDelete("chanopts", "'"+name.toLower()+"' AND clog=1");
	services.getDatabase().dbInsert("chanopts", "'','" + name.toLower()+"',1");
	return;
     }
   else
     {
	services.getDatabase().dbDelete("chanopts", "'"+name.toLower()+"' AND clog=1");
	return;
     }
return;
}

int const dChan::getRegisteredID(void)
{
if (services.getDatabase().dbSelect("id","chans","name='"+name.toLower()+"'") < 1)
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

void const dChan::sendBans(String &dest, String const &from)
{
  User *ptr = services.findUser(dest);
  if(ptr==0)
     return;
   ptr->sendMessage("Ban List for "+name,from);
   int nbRes = services.getDatabase().dbSelect("*","chanbans","chan='"+String::convert(getRegisteredID())+"'");
   for (int i=0; i<nbRes; i++)
     {
	ptr->sendMessage("["+String::convert(i)+"] Mask "+services.getDatabase().dbGetValue(2)+
			 "SetBy "+services.getDatabase().dbGetValue(3)+
			 "Date "+services.getDatabase().dbGetValue(4)+
			 "Expires "+services.getDatabase().dbGetValue(5),from);
	ptr->sendMessage("Reason "+services.getDatabase().dbGetValue(6),from);
	services.getDatabase().dbGetRow();
     }
}

String const dChan::getOwner(void)
{
   if(services.getDatabase().dbSelect("owner","chans","id='"+String::convert(getRegisteredID())+"'") < 1)
     return "No Owner";
   else
     return services.getDatabase().dbGetValue();
}

int const dChan::getCount(void)
{
   return services.getDatabase().dbCount("chans");   
   
}
