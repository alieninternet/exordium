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
				      "chanid='"+String::convert(getRegisteredID())+"' AND nickid='"
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
   CResult *myRes = services.getDatabase().dbGetResultSet();
   for (int i=0; i<nbRes; i++)
     {
	ptr->sendMessage("["+String::convert(i)+"] Mask "+myRes->getValue(i,2)+
			 " SetBy "+myRes->getValue(i,3)+
			 " Date "+myRes->getValue(i,4)+
			 " Expires "+myRes->getValue(i,5),from);
	ptr->sendMessage("Reason "+myRes->getValue(i,6),from);
     }
   delete(myRes);
   
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

void const dChan::addAccess(String &nick, String const &level)
{
   User *ptr = services.findUser(nick);
  
   services.getDatabase().dbInsert("chanaccess","'"+String::convert(getRegisteredID())+
				   "','"+String::convert(ptr->getRegisteredNickID())+"','"+level+"'");
}

void const dChan::log(User& origin, String const &service,String const &text, String const &cname)
{
services.getDatabase().dbInsert("log","'','"+origin.getIDList()+"','"+origin.getIdent()+
				"','"+origin.getHost()+"','"+service+"',NOW(),'"+text+
				"','"+cname+"'");
}

void const dChan::updateTopic(String const &topic)
{
   services.getDatabase().dbUpdate("chans","topic='"+topic+"'", "id='"+String::convert(getRegisteredID())+"'");   
}

void const dChan::setTopic(String const &service, String const &topic)
{
   time_t currentTime;
   currentTime = time(NULL);
   services.queueAdd(":"+service+" TOPIC "+name+" Chan "+String::convert(currentTime)+" :"+topic);   
}

bool const dChan::isOp(String const &nick)
{
   if(services.getDatabase().dbSelect("status","chanstatus","chanid='"+String::convert(onlineID)+
				      "' AND nickid='"+String::convert(services.locateID(nick))+
				      "'") < 1)
     return false;
   if(services.getDatabase().dbGetValue().toInt() == 2)
     return true;
   else
     return false;
}

bool const dChan::isVoice(String const &nick)
{
   if(services.getDatabase().dbSelect("status","chanstatus","chanid='"+String::convert(onlineID)+
				      "' AND nickid='"+String::convert(services.locateID(nick))+
				                                            "'") < 1)
          return false;
      if(services.getDatabase().dbGetValue().toInt() == 1)
          return true;
      else
          return false;
 }

void const dChan::mode(String const &service,String const &mode,String const &target)
{
   services.queueAdd(":"+service+" MODE "+name+" "+mode+" "+target);
}

void const dChan::kick(String const &service,String const &nick,String const &reason)
{
   services.queueAdd(":"+service+" KICK "+name+" "+nick+" :"+reason);
   String tempnick = String(nick);
   User *ptr = services.findUser(tempnick);
   if(ptr==0)
     return;
   delUser(*ptr);
   
}

void const dChan::ban(User &user, String const &service, String const &reason, String const &setby)
{
String host = "*!*@"+user.getHost();
int expire = services.currentTime + 3600;
services.queueAdd(":"+service+" MODE "+name+" +b "+host);
String query = "'','"+String::convert(getRegisteredID())+
               "','"+host+"','"+setby+"',NOW(),'"+
               String::convert(expire)+"','"+reason+"'";
services.getDatabase().dbInsert("chanbans",query);
services.serviceKick(name,user.getNickname(),reason);

   
}
