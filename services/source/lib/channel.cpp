/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "exordium/channel.h"
#include <aisutil/string.h>
#include <kineircd/config.h>
#include "exordium/services.h"

using namespace Exordium;
using AISutil::String;
using AISutil::StringMask;
namespace Exordium
{

/* Removing a ban */
   void
     Channel::RemoveBan(String const &id, String const &chan, String const &mask)
       {
	  String tid = chan;
	  int cid = tid.toInt();
	  String channel = getChanName(cid);
#ifdef DEBUG
	  services.logLine("\002[\002Unban\002]\002 " + mask + " / " + channel,
			   Log::Debug);
#endif
	  services.serverMode(channel,"-b",mask);
	  services.getDatabase().dbDelete("chanbans", "id='" + id + "'");
	  return;
       }

   /* Return total number of channels in database */
   String
     Channel::getChanCount(void)
       {
          return String::convert(services.getDatabase().dbCount("chans"));
       }

bool
  Channel::ChanLog(String const &channel)
{
   if( services.getDatabase().dbSelect("clog", "chanopts", "'" + channel.toLower() + "'") < 1 )
      return false;

   if(services.getDatabase().dbGetValue() == "1")
      return true;
   else
      return false;
}

void
  Channel::setChanLog(String const &channel, bool const &value)
{
   if(value)
     {
        services.getDatabase().dbDelete("chanopts", "'"+channel.toLower()+"' AND clog=1");
        services.getDatabase().dbInsert("chanopts", "'', '" + channel.toLower()+"',1");
	return;
     }
   else
     {
        services.getDatabase().dbDelete("chanopts", "'"+channel.toLower()+"' AND clog=1");
	return;
     }
}

/* Return the numercial (unique) id of a registered channel */
int
  Channel::getChanID(String const &name)
{
   if( services.getDatabase().dbSelect("id", "chans", "name='"+name.toLower()+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

/* Return the (non-persistant) id of any channel */
int
  Channel::getOnlineChanID(String const &name)
{
   if ( services.getDatabase().dbSelect("id", "onlinechan", "name='"+name.toLower()+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

/* Checks to see if a channel is registered */
bool
  Channel::isChanRegistered(String const &name)
{
   if( services.getDatabase().dbSelect("id", "chans", "name='"+name.toLower()+"'") < 1 )
     return false;
   
   if( services.getDatabase().dbGetValue().toInt() > 0 )
     return true;
   else
     return false;
}
/* Return the access (if any) the given nickname has in the given channel */
int
  Channel::getChanAccess(String const &name, String const &nick)
{
   int nickid = services.getStatic().getRegisteredNickID(nick);
   int chanid = getChanID(name);

   if( services.getDatabase().dbSelect("access", "chanaccess", 
              "chanid='"+String::convert(chanid)+"' AND nickid='"+String::convert(nickid)+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

/* Check to see if the channel is 'made' - i.e someone is on it on the network */
bool
  Channel::ifChanExists (String const &name)
{
   if( services.getDatabase().dbSelect("id", "onlinechan", "name='"+name+"'") < 1 )
      return false;
   else
   {
     if( services.getDatabase().dbGetValue().toInt() > 0 )
       return true;
     else
       return false;
   }
}

/* Return the number of registered channels */
int
  Channel::maxChannels(void)
{
   return services.getDatabase().dbCount("chans");
}

/* Return the number of access entries across the whole database */
int
  Channel::maxChannelsAccess(void)
{
  return services.getDatabase().dbCount("chanaccess");
}

/* Return the number of access entries for the given nickname */
int
  Channel::maxChannelsUser(String const &nick)
{
  return services.getDatabase().dbCount("chanaccess", "nickid='"+String::convert(nick)+"'");
}

/* Return the given name for the id given */
Kine::ChannelName
  Channel::getChanName(int const &number)
{
   if( services.getDatabase().dbSelect("name", "chans", "id="+String::convert(number)) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

String
  Channel::getChanIDName(int const &number)
{
   if( services.getDatabase().dbSelect("name", "onlinechan", "id="+String::convert(number)) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

/* Set the topic in the given channel, to the given text */

void
  Channel::setTopic(String const &name, String const &topic)
{
//   String temp = String(":Chan TOPIC ")+name+" Chan " +String::convert(currentTime)+" :"+topic;
//   services.queueAdd(String(temp));
}

/* Update *our* records of the channels topic to whats given */
void
  Channel::updateTopic(String const &name, String const &topic)
{
   int id = getChanID(name);

   services.getDatabase().dbUpdate("chans", "topic='"+topic+"'", "id='"+String::convert(id)+"'");
}

/* Get the channel owner for the given numerical registered chan id */
String
  Channel::getChanOwner(int const &number)
{
   if( services.getDatabase().dbSelect("owner", "chans", "id='"+String::convert(number)+"'") < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

/* Add a user to the given channel */
void
  Channel::chanAddAccess(String const &name, String const &nick, String const &level)
{
   int nickid = services.getStatic().getRegisteredNickID(nick);
   int chanid = getChanID(name);

   services.getDatabase().dbInsert("chanaccess", 

"'"+String::convert(chanid)+"','"+String::convert(nickid)+"','"+String::convert(level)+"'");
}
/* Delete access from the given channel */
void
  Channel::chanDelAccess(String const &name, String const &nick)
{
   int nickid = services.getStatic().getRegisteredNickID(nick);
   int chanid = getChanID(name);
   services.getDatabase().dbDelete("chanaccess", 
          "chanid='"+String::convert(chanid)+"' AND nickid='"+String::convert(nickid)+"'");
}

/* Deregister a channel, and remove all pertaining channel access entries */
void
  Channel::deregisterChannel(const Kine::ChannelName& name)
{
   int foo = getChanID(name);

   services.getDatabase().dbDelete("chans", "name='"+name.IRCtoLower()+"'");
   services.getDatabase().dbDelete("chanaccess", "chanid='"+String::convert(foo)+"'");
   services.getDatabase().dbDelete("chanopts","name='"+name.IRCtoLower()+"'");
   services.getDatabase().dbDelete("chanbans", "chan='"+String::convert(foo)+"'");
//   services.queueAdd(":" + Kine::config().getOptionsServerName() + " MODE " +
//        name + " -r");
}

/* Synch the network to what we think it should be like */
void
  Channel::synchChannels(void)
{
   int nbRes = services.getDatabase().dbSelect("name,topic,modes", "chans");
   for(int i=0; i<nbRes; i++)
   {
     synchChannel(services.getDatabase().dbGetValue(0), services.getDatabase().dbGetValue(1), 
                  services.getDatabase().dbGetValue(2));
     services.getDatabase().dbGetRow();
   }
}

/* Synch a single channel :) */
void
  Channel::synchChannel(String const &name, String const &topic, String const &modes)
{
   //services.serviceJoin("Chan",name);
//   services.queueAdd(":" + Kine::config().getOptionsServerName() + " MODE " +
//		     name + " +r");
//   services.queueAdd(":" + Kine::config().getOptionsServerName() + " MODE " +
//		     name + " " + modes);
   setTopic(name,topic);
}

/* Register a channel with the given owner */
void
  Channel::registerChannel(const Kine::ChannelName& name, String const &owner)
{
   services.getDatabase().dbInsert("chans", 
       "'','"+name.IRCtoLower()+"','"+owner+
       "',NOW(),'This is a new channel','+nt','A new channel','www.peoplechat.org', '','', 0"); 
   chanAddAccess(name,owner,"500");
//   services.queueAdd(":" + Kine::config().getOptionsServerName() + " MODE " +
//                     name + " +r");

   setTopic(name, String("This channel has just been registered by ")+owner);
   services.getDatabase().dbInsert("chanopts",
				   "'','"+name.IRCtoLower()+"',1,1,0,0,0,0");
}

/* Return how many channels a nickname owns */
int
  Channel::ownedChannels(String const &nickname)
{
   return services.getDatabase().dbCount("chans", "owner='"+nickname+"'");
}
/* Add a ban to a channel db ban list */
void
  Channel::addChanBan(int const &chan, String const &mask, String const &setby, int const &expireon, String const &reason)
{
   services.getDatabase().dbInsert("chanbans", 
           "'','"+String::convert(chan)+"','"+mask+"','"+setby+"',NOW(),'"+String::convert(expireon)+"','"+reason+"'");
}

// TODO
/* Get channel's mode */
String
  Channel::getModes(void)
{
/*
   if( services.getDatabase().dbSelect("modes", "chans", "id="+String::convert(getChanID()) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
*/
   return "";
}

int
  Channel::getNbUsers(String const &chan)
{
  return services.getDatabase().dbCount("chanstatus", "chanid="+ String::convert(getOnlineChanID(chan)));
}




/* Ban the given mask from the channel */
void
  Channel::banChan(String const &chan, String const &mask, String const &reason)
{
   int chanid = getOnlineChanID(chan);

   int nbRes = services.getDatabase().dbSelect("nickid", "chanstatus", "chanid='"+String::convert(chanid)+"'");

 
   for(int i=0; i<nbRes; i++)
     {
	String nid = services.getDatabase().dbGetValue();
	int foo = nid.toInt();
	Kine::ClientName tnick = services.getOnlineNick(foo);
	User *ptr = services.findUser(tnick);

	String ident = ptr->getIdent();
	String host = ptr->getHost();
	String full = String(tnick)+"!"+ident+"@"+host;
	StringMask bar(mask);
	bool hi = false;
	hi = bar.matches(full);
	if(hi==false)
	  {
	     //Matched (despite being false?) :-)
	     services.serviceKick(chan,tnick,reason);
	     String togo = "\002[\002BanKick\002]\002 "+chan+" / "+tnick;
#ifdef DEBUG
	     services.logLine(togo, Log::Debug);
#endif
	  }
	if(hi==true)
	  {
	  }

        services.getDatabase().dbGetRow();
     }
}

}

