/*
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/channel.h"
#include "exordium/nickname.h"
#include <kineircd/str.h>
#include "exordium/services.h"
#include "exordium/sql.h"

using namespace Exordium;
using Kine::String;
using Kine::StringMask;
namespace Exordium
{

/* Removing a ban */
   void
     Channel::RemoveBan(String const &id, String const &chan, String const &mask)
       {
	  String tid = chan;
	  int cid = tid.toInt();
	  String channel = getChanName(cid);
	  String todebug = "\002[\002Unban\002]\002 " + mask + " / " + channel;
	  services.Debug(todebug);
	  services.serverMode(channel,"-b",mask);
	  String query = "DELETE from chanbans where id='"+id+"'";
	  services.getDatabase().query(query);

       }
/* Return total number of channels in database */
   String
     Channel::getChanCount(void)
       {
	  String query = "SELECT count(*) from chans";
	  MysqlRes res = services.getDatabase().query(String(query));
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]).c_str();
	       res.free_result();
	       return foo;
	    }
	  res.free_result();
	  return String("0");
       }

/* Update our tables as showing the given user as 'opped'
   in the given chan */
   void
     Channel::internalOp(String const &nickname, String const &channel)
       {
	  int cid = getOnlineChanID(channel);
	  int nid = services.getNickname().getOnlineNickID(nickname);
	  String query = "INSERT into chanstatus values ('"
	    +String::convert(cid)+"','"+String::convert(nid)+"','2')";
	  services.getDatabase().query(query);
       }

/* Update our tables as showing the given user is 'voiced'
   in the given chan */
   void
     Channel::internalVoice(String const &nickname, String const &channel)
       {
	  int cid = getOnlineChanID(channel);
	  int nid = services.getNickname().getOnlineNickID(nickname);
	  String query = "INSERT into chanstatus values ('"
	    +String::convert(cid)+"','"+String::convert(nid)+"','1')";
	  services.getDatabase().query(query);
       }

/* Update our tables as showing the given user in the channel
   N.B their status is 0 - ala non opped, non voiced */
   void
     Channel::internalAdd(String const &nickname, String const &channel)
       {
	  int cid = getOnlineChanID(channel);
	  int nid = services.getNickname().getOnlineNickID(nickname);
	  String query = "INSERT into chanstatus values ('"
	    +String::convert(cid)+"','"+String::convert(nid)+"','0')";
	  services.getDatabase().query(query);
       }

/* Update our tables as showing the given user is no longer
   in the channel, removing all status flags */
   void
     Channel::internalDel(String const &nickname, String const &channel)
       {
	  int cid = getOnlineChanID(channel);
	  int nid = services.getNickname().getOnlineNickID(nickname);
	  String query = "DELETE from chanstatus where chanid='"
	    +String::convert(cid)+"' AND nickid='"+String::convert(nid)+"'";
	  services.getDatabase().query(query);
       }

/* Update our tables to show the given user as being 'deopped' */
   void
     Channel::internalDeOp(String const &nickname, String const &channel)
       {
	  int chanid = getOnlineChanID(channel);
	  int nickid = services.getNickname().getOnlineNickID(nickname);
	  String query = "DELETE from chanstatus where chanid='"
	    +String::convert(chanid)+"' AND nickid='"
	    + String::convert(nickid) + "' AND status='2'";
	  services.getDatabase().query(query);
       }

/* Update our tables to show the given user as being 'devoiced' */

   void
     Channel::internalDeVoice(String const &nickname, String const &channel)
       {
	  int chanid = getOnlineChanID(channel);
	  int nickid = services.getNickname().getOnlineNickID(nickname);
	  String query = "DELETE from chanstatus where chanid='"
	    +String::convert(chanid)+"' AND nickid='"
	    + String::convert(nickid) + "' AND status='1'";
	  services.getDatabase().query(query);
       }

/* Return the numercial (unique) id of a registered channel */
   int
     Channel::getChanID(String const &name)
       {
	  String query = "SELECT id from chans where name='"+name.toLower()+"'";
	  MysqlRes res = services.getDatabase().query(String(query));
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]).c_str();
	       res.free_result();
	       return foo.toInt();
	    }
	  res.free_result();
	  return 0;
       }

/* Return the (non-persistant) id of any channel */
   int
     Channel::getOnlineChanID(String const &name)
       {
	  String query = "SELECT id from onlinechan where name='"+name.toLower()+"'";
	  MysqlRes res = services.getDatabase().query(String(query));
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]).c_str();
	       res.free_result();
	       return foo.toInt();
	    }
	  res.free_result();
	  return 0;
       }

/* Checks to see if a channel is registered */
   bool
     Channel::isChanRegistered(String const &name)
       {
	  String query = "select id from chans where name='" + name + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo  = ((std::string) row[0]).c_str();
	       res.free_result();
	       if(foo.toInt() > 0)
		 {
		    return true;
		 }
	    }
	  return false;
       }
/* Return the access (if any) the given nickname has in the given channel */
   int
     Channel::getChanAccess(String const &name, String const &nick)
       {
	  int nickid = services.getNickname().getRegisteredNickID(nick);
	  int chanid = getChanID(name);
	  String query = "SELECT access from chanaccess where chanid='"
	    + String::convert(chanid)+"' AND nickid='"
	    + String::convert(nickid) + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]).c_str();
	       res.free_result();
	       return foo.toInt();
	    }
	  res.free_result();
	  return 0;
       }

/* Check to see if the channel is 'made' - i.e someone is on it on the network */
   bool
     Channel::ifChanExists (String const &name)
       {
	  String query = "SELECT id from onlinechan where name='"+name+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       if((int) row[0]>0)
		 {
		    res.free_result();
		    return true;
		 }
	    }
	  res.free_result();
	  return false;
       }

/* Return the number of registered channels */
   int
     Channel::maxChannels(void)
       {
	  String query = "SELECT count(*) from chans";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String max = ((std::string)row[0]).c_str();
	       res.free_result();
	       return max.toInt();
	    }
	  res.free_result();
	  return 0;
       }

/* Return the number of access entries across the whole database */
   int
     Channel::maxChannelsAccess(void)
       {
	  String query = "SELECT count(*) from chanaccess";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String max = ((std::string)row[0]).c_str();
	       res.free_result();
	       return max.toInt();
	    }
	  res.free_result();
	  return 0;
       }

/* Return the number of access entries for the given nickname */
   int
     Channel::maxChannelsUser(String const &nick)
       {
	  int id = services.getNickname().getRegisteredNickID(nick);
	  String query = "SELECT count(*) from chanaccess where nickid='"
	    + String::convert(id)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String max = ((std::string)row[0]).c_str();
	       res.free_result();
	       return max.toInt();
	    }
	  res.free_result();
	  return 0;
       }

/* Return the given name for the id given */
   String
     Channel::getChanName(int const &number)
       {
	  String query = "SELECT name from chans where id="
	    + String::convert(number);
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String name = ((std::string) row[0]).c_str();
	       res.free_result();
	       return name;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Channel::getChanIDName(int const &number)
       {
	  String query = "SELECT name from onlinechan where id="
	    + String::convert(number);
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String name = ((std::string) row[0]).c_str();
	       res.free_result();
	       return name;
	    }
	  res.free_result();
	  return String("");
       }

/* Set the topic in the given channel, to the given text */

   void
     Channel::setTopic(String const &name, String const &topic)
       {
	  time_t currentTime;
	  currentTime = time(NULL);
	  String temp = String(":Chan TOPIC ")+name+" Chan " +String::convert(currentTime)+" :"+topic;
	  services.queueAdd(String(temp));
       }

/* Update *our* records of the channels topic to whats given */
   void
     Channel::updateTopic(String const &name, String const &topic)
       {
	  int id = getChanID(name);
	  String query = "UPDATE chans set topic='"+topic
	    +"' where id='"+String::convert(id)+"'";
	  services.getDatabase().query(query);
       }

/* Get the channel owner for the given numerical registered chan id */
   String
     Channel::getChanOwner(int const &number)
       {
	  String query = "SELECT owner from chans where id='"
	    + String::convert(number)+ "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String name = ((std::string) row[0]).c_str();
	       res.free_result();
	       return name;
	    }
	  res.free_result();
	  return "";
       }

/* Add a user to the given channel */
   void
     Channel::chanAddAccess(String const &name, String const &nick, String const &level)
       {
	  int nickid = services.getNickname().getRegisteredNickID(nick);
	  int chanid = getChanID(name);
	  String query = "INSERT into chanaccess values ('"
	    + String::convert(chanid)+"','"+String::convert(nickid)
	      +"','"+String::convert(level)+"')";
	  services.getDatabase().query(query);
       }
/* Delete access from the given channel */
   void
     Channel::chanDelAccess(String const &name, String const &nick)
       {
	  int nickid = services.getNickname().getRegisteredNickID(nick);
	  int chanid = getChanID(name);
	  String query = "DELETE from chanaccess where chanid='"
	    + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)
	      + "'";
	  services.getDatabase().query(query);
       }

/* Deregister a channel, and remove all pertaining channel access entries */
   void
     Channel::deregisterChannel(String const &name, String const &reason)
       {
	  int foo = getChanID(name);
	  String query = "DELETE from chans where name='"+name+"'";
	  services.getDatabase().query(query);
	  query = "DELETE from chanaccess where chanid='"
	    + String::convert(foo)+ "'";
	  services.getDatabase().query(query);
	  String togo = "This channel has been deregistered \002"+reason;
	  services.serviceNotice(String(togo),"Chan",name);
	  services.servicePart("Chan",name);
       }

/* Synch the network to what we think it should be like */
   void
     Channel::synchChannels(void)
       {
	  String query = "SELECT name,topic,modes from chans";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String name = ((std::string) row[0]).c_str();
	       String topic = ((std::string) row[1]).c_str();
	       String modes = ((std::string) row[2]).c_str();
	       synchChannel(name,topic,modes);
	    }
	  res.free_result();
       }

/* Synch a single channel :) */
   void
     Channel::synchChannel(String const &name, String const &topic, String const &modes)
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
     Channel::registerChannel(String const &name, String const &owner)
       {
	  String query = "INSERT into chans values ('','" + name + "','" + owner + "','This is a new channel','+nt','A new channel', 'www.ircdome.org',0)";
	  services.getDatabase().query(query);
	  chanAddAccess(name,owner,"500");
	  services.serviceJoin("Chan",name);
	  services.serverMode(name,"+o","Chan");
	  setTopic(name, String("This channel has just been registered by ")+owner);
       }

/* Return how many channels a nickname owns */
   int
     Channel::ownedChannels(String const &nickname)
       {
	  String query = "SELECT count(*) from chans where owner='"
	    + nickname + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String number = ((std::string) row[0]).c_str();
	       res.free_result();
	       return number.toInt();
	    }
	  res.free_result();
	  return 0;
       }
/* Add a ban to a channel db ban list */
   void
     Channel::addChanBan(int const &chan, String const &mask, String const &setby, int const &expireon, String const &reason)
       {
	  String query = "INSERT into chanbans values ('','"+String::convert(chan)+"','"+mask+"','"+setby+"',NOW(),'"+String::convert(expireon)+"','"+reason+"')";
	  services.getDatabase().query(query);
       }

/* Ban the given mask from the channel */
   void
     Channel::banChan(String const &chan, String const &mask, String const &reason)
       {
	  int chanid = getOnlineChanID(chan);
	  String query = "SELECT nickid from chanstatus where chanid='"
	    + String::convert(chanid) + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String nid = ((std::string) row[0]).c_str();
	       int foo = nid.toInt();
	       String tnick = services.getNickname().getOnlineNick(foo);
	       String ident = services.getNickname().getIdent(tnick);
	       String host = services.getNickname().getHost(tnick);
	       String full = String(tnick)+"!"+ident+"@"+host;
	       res.free_result();
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
	    }
       }

}

