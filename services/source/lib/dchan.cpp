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

#include "exordium/services.h"
#include "exordium/dchan.h"
#include <kineircd/str.h>

using AISutil::String;
using AISutil::StringTokens;
using AISutil::StringMask;
using namespace Exordium;


namespace Exordium
{

/* Removing a ban */
   void
     dChan::RemoveBan(String const &id, String const &chan, String const &mask)
       {
	  String tid = chan;
	  int cid = tid.toInt();
	  String channel = getChanName(cid);
	  services.Debug("\002[\002Unban\002]\002 " + mask + " / " + channel);
	  services.serverMode(channel,"-b",mask);
	  services.getDatabase().dbDelete("chanbans", "id='" + id + "'");
	  return;
       }

   /* Return total number of channels in database */
   String
     dChan::getChanCount(void)
       {
          return services.chans.size();
       }

/* Update our tables as showing the given user as 'opped'
   in the given chan */
void
  dChan::internalOp(String const &nickname, String const &channel)
{
   int cid = getOnlineChanID(channel);
   int nid = services.locateID(nickname);
   services.getDatabase().dbDelete("chanstatus", "chanid=" + String::convert(cid) + " AND nickid="+String::convert(nid)); 
   services.getDatabase().dbInsert("chanstatus", "'" + String::convert(cid)+"','"+String::convert(nid)+"','2'");
   services.Debug("Have marked "+nickname+" as being opped in "+channel);
   return;
}

/* Update our tables as showing the given user is 'voiced'
   in the given chan */
void
  dChan::internalVoice(String const &nickname, String const &channel)
{
   int cid = getOnlineChanID(channel);
   int nid = services.locateID(nickname);
   services.getDatabase().dbDelete("chanstatus", "chanid=" + String::convert(cid) + " AND nickid="+String::convert(nid));
   services.getDatabase().dbInsert("chanstatus", "'" + String::convert(cid)+"','"+String::convert(nid)+"','1'");
   services.Debug("Have marked "+nickname+" as being voiced in "+channel);
   return;
}

/* Update our tables as showing the given user in the channel
   N.B their status is 0 - ala non opped, non voiced */
void
  dChan::internalAdd(String const &nickname, String const &channel)
{
   int cid = getOnlineChanID(channel);
   int nid = services.locateID(nickname);
   services.getDatabase().dbDelete("chanstatus", "chanid=" + String::convert(cid) + " AND nickid="+String::convert(nid));
   services.getDatabase().dbInsert("chanstatus", "'" + String::convert(cid)+"','"+String::convert(nid)+"','0'");
   services.Debug("Have marked "+nickname+" as being NORMAL in "+channel);
   return;
}
bool
  dChan::ChanLog(String const &channel)
{
   if( services.getDatabase().dbSelect("clog", "chanopts", "'" + channel.toLower() + "'") < 1 )
      return false;

   if(services.getDatabase().dbGetValue() == "1")
      return true;
   else
      return false;
}

void
  dChan::setChanLog(String const &channel, bool const &value)
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

/* Update our tables as showing the given user is no longer
   in the channel, removing all status flags */
void
  dChan::internalDel(String const &nickname, String const &channel)
{
   services.getDatabase().dbDelete("chanstatus", "chanid='" + String::convert(getOnlineChanID(channel))+"' AND nickid='"+String::convert(services.locateID(nickname))+"'");
   return;
}

/* Update our tables to show the given user as being 'deopped' */
void
  dChan::internalDeOp(String const &nickname, String const &channel)
{
   int chanid = getOnlineChanID(channel);
   int nickid = services.locateID(nickname);
   services.getDatabase().dbDelete("chanstatus", "chanid="+String::convert(chanid)+" AND nickid="+String::convert(nickid));
   services.getDatabase().dbInsert("chanstatus", "'','"+String::convert(chanid)+"','"+String::convert(nickid)+"'");
}

/* Update our tables to show the given user as being 'devoiced' */

void
  dChan::internalDeVoice(String const &nickname, String const &channel)
{
   int chanid = getOnlineChanID(channel);
   int nickid = services.locateID(nickname);
   services.getDatabase().dbDelete("chanstatus", 
       "chanid='"+String::convert(chanid)+"' AND nickid='"+String::convert(nickid)+ "' AND status='1'");
}

/* Return the numercial (unique) id of a registered channel */
int
  dChan::getChanID(String const &name)
{
   if( services.getDatabase().dbSelect("id", "chans", "name='"+name.toLower()+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

/* Return the (non-persistant) id of any channel */
int
  dChan::getOnlineChanID(String const &name)
{
   if ( services.getDatabase().dbSelect("id", "onlinechan", "name='"+name.toLower()+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

/* Checks to see if a channel is registered */
bool
  dChan::isChanRegistered(String const &name)
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
  dChan::getChanAccess(String const &name, String const &nick)
{
   int nickid = services.getRegisteredNickID(nick);
   int chanid = getChanID(name);

   if( services.getDatabase().dbSelect("access", "chanaccess", 
              "chanid='"+String::convert(chanid)+"' AND nickid='"+String::convert(nickid)+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}

/* Check to see if the channel is 'made' - i.e someone is on it on the network */
bool
  dChan::ifChanExists (String const &name)
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
  dChan::maxChannels(void)
{
   return services.getDatabase().dbCount("chans");
}

/* Return the number of access entries across the whole database */
int
  dChan::maxChannelsAccess(void)
{
  return services.getDatabase().dbCount("chanaccess");
}

/* Return the number of access entries for the given nickname */
int
  dChan::maxChannelsUser(String const &nick)
{
  return services.getDatabase().dbCount("chanaccess", "nickid='"+String::convert(nick)+"'");
}

/* Return the given name for the id given */
String
  dChan::getChanName(int const &number)
{
   if( services.getDatabase().dbSelect("name", "chans", "id="+String::convert(number)) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

String
  dChan::getChanIDName(int const &number)
{
   if( services.getDatabase().dbSelect("name", "onlinechan", "id="+String::convert(number)) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

/* Set the topic in the given channel, to the given text */

void
  dChan::setTopic(String const &name, String const &topic)
{
   time_t currentTime;
   currentTime = time(NULL);
   String temp = String(":Chan TOPIC ")+name+" Chan " +String::convert(currentTime)+" :"+topic;
   services.queueAdd(String(temp));
}

/* Update *our* records of the channels topic to whats given */
void
  dChan::updateTopic(String const &name, String const &topic)
{
   int id = getChanID(name);

   services.getDatabase().dbUpdate("chans", "topic='"+topic+"'", "id='"+String::convert(id)+"'");
}

/* Get the channel owner for the given numerical registered chan id */
String
  dChan::getChanOwner(int const &number)
{
   if( services.getDatabase().dbSelect("owner", "chans", "id='"+String::convert(number)+"'") < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

/* Add a user to the given channel */
void
  dChan::chanAddAccess(String const &name, String const &nick, String const &level)
{
   int nickid = services.getRegisteredNickID(nick);
   int chanid = getChanID(name);

   services.getDatabase().dbInsert("chanaccess", 
         "'"+String::convert(chanid)+"',',"+String::convert(nickid)+"','"+String::convert(level)+"'");
}
/* Delete access from the given channel */
void
  dChan::chanDelAccess(String const &name, String const &nick)
{
   int nickid = services.getRegisteredNickID(nick);
   int chanid = getChanID(name);
   services.getDatabase().dbDelete("chanaccess", 
          "chanid='"+String::convert(chanid)+"' AND nickid='"+String::convert(nickid)+"'");
}

/* Deregister a channel, and remove all pertaining channel access entries */
void
  dChan::deregisterChannel(String const &name, String const &reason)
{
   int foo = getChanID(name);

   services.getDatabase().dbDelete("chans", "name='"+name+"'");
   services.getDatabase().dbDelete("chanaccess", "chanid='"+String::convert(foo)+"'");

   String togo = "This channel has been deregistered \002"+reason;
   services.serviceNotice(String(togo),"Chan",name);
   services.servicePart("Chan",name);
}

/* Synch the network to what we think it should be like */
void
  dChan::synchChannels(void)
{
   int nbRes = services.getDatabase().dbSelect("name,topic,modes", "chans");

   std::cout << "nbRes:" << String::convert(nbRes) << std::endl;

   for(int i=0; i<nbRes; i++)
   {
     std::cout << "inside FOR!!!!!!" << std::endl;
     synchChannel(services.getDatabase().dbGetValue(0), services.getDatabase().dbGetValue(1), 
                  services.getDatabase().dbGetValue(2));
     services.getDatabase().dbGetRow();
   }
}

/* Synch a single channel :) */
void
  dChan::synchChannel(String const &name, String const &topic, String const &modes)
{
   services.serviceJoin("Chan",name);
   services.serverMode(name,"+o","Chan");
   String togo = ":services.ircdome.org MODE "+name+" +r";
   services.queueAdd(String(togo));
   togo = ":services.ircdome.org MODE "+name+" "+modes;
   services.queueAdd(String(togo));
   setTopic(name,topic);
}

/* Register a channel with the given owner */
void
  dChan::registerChannel(String const &name, String const &owner)
{
   services.getDatabase().dbInsert("chans", 
       "'','"+name+"','"+owner+"','This is a new channel','+nt','A new channel','www.ircdome.irg', 0"); 
   chanAddAccess(name,owner,"500");
   services.serviceJoin("Chan",name);
   services.serverMode(name,"+o","Chan");
   setTopic(name, String("This channel has just been registered by ")+owner);
}

/* Return how many channels a nickname owns */
int
  dChan::ownedChannels(String const &nickname)
{
   return services.getDatabase().dbCount("chans", "owner='"+nickname+"'");
}
/* Add a ban to a channel db ban list */
void
  dChan::addChanBan(int const &chan, String const &mask, String const &setby, int const &expireon, String const &reason)
{
   services.getDatabase().dbInsert("chanbans", 
           "'','"+String::convert(chan)+"','"+mask+"','"+setby+"',NOW(),'"+String::convert(expireon)+"','"+reason+"'");
}

// TODO
/* Get channel's mode */
String
  dChan::getModes(void)
{
/*
   if( services.getDatabase().dbSelect("modes", "chans", "id="+String::convert(getChanID()) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
*/
}

int
  dChan::getNbUsers(String const &chan)
{
  return services.getDatabase().dbCount("chanstatus", "chanid="+ String::convert(getOnlineChanID(chan)));
}




/* Ban the given mask from the channel */
void
  dChan::banChan(String const &chan, String const &mask, String const &reason)
{
   int chanid = getOnlineChanID(chan);

   int nbRes = services.getDatabase().dbSelect("nickid", "chanstatus", "chanid='"+String::convert(chanid)+"'");

 
   for(int i=0; i<nbRes; i++)
     {
	String nid = services.getDatabase().dbGetValue();
	int foo = nid.toInt();
	String tnick = services.getOnlineNick(foo);
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
	     services.Debug(togo);
	  }
	if(hi==true)
	  {
	  }

        services.getDatabase().dbGetRow();
     }
}

}

