/*
 *  * Exordium Network Services
 *  * Copyright (C) 2002 IRCDome Development Team
 *  *
 *  * This file is a part of Exordium.
 *  *
 *  * $Id$
 *  *
 *  * This program is free software; you can redistribute it and/or modify
 *  * it under the terms of the GNU General Public License as published by
 *  * the Free Software Foundation; either version 2 of the License, or
 *  * (at your option) any later version.
 *  *
 *  * This program is distributed in the hope that it will be useful,
 *  * but WITHOUT ANY WARRANTY; without even the implied
 *  * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  * See the GNU General Public License for more details.
 *  *
 *  * You should have received a copy of the GNU General Public License
 *  * along with this program; if not, write to the Free Software
 *  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,USA
 *  *
 *  * For contact details for the IRCDome Development Team please see the
 *  * enclosed README file.
 *  *
 * */

#include "include/chan.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

struct Chan::functionTableStruct const
  Chan::functionTable[] =
{
     {"access", &Chan::parseACCESS},
     {".access", &Chan::parseACCESS},
     {"ban", &Chan::parseBAN},
     {".ban", &Chan::parseBAN},
     {".kick", &Chan::parseKICK},
     {"kick", &Chan::parseKICK},
     {".op", &Chan::parseOP},
     {"op", &Chan::parseOP},
     {".voice", &Chan::parseVOICE},
     {"voice", &Chan::parseVOICE},
     {"register", &Chan::parseREGISTER},
     {"help", &Chan::parseHELP},
     {"deop", &Chan::parseDEOP},
     {".deop", &Chan::parseDEOP},
     {"devoice", &Chan::parseDEVOICE},
     {".devoice", &Chan::parseDEVOICE},
     {"topic", &Chan::parseTOPIC},
     {".topic", &Chan::parseTOPIC},
     {"adduser", &Chan::parseADDUSER},
     {".adduser", &Chan::parseADDUSER},
     {"info", &Chan::parseINFO},
     {".info", &Chan::parseINFO},
     {"listban", &Chan::parseLISTBAN},
     {".listban", &Chan::parseLISTBAN},
     {".set", &Chan::parseSET},
     {"set", &Chan::parseSET},
     {"seen", &Chan::parseSEEN},
     {".seen", &Chan::parseSEEN},
     {0, 0}
};

void
  Chan::parseLine (String const &line, String const &requestor, String const &chan)
{
   StringTokens st (line);
   String origin = requestor;
   String command = st.nextToken ().toLower ();
   String ch = chan;
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st, ch);
	     return;
	  }
     }

   return;
}

void
  Chan::parseLine (String const &line, String const &requestor)
{
   StringTokens st (line);
   String origin = requestor;
   String command = st.nextToken ().toLower ();
   String ch = "";
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st, ch);
	     return;
	  }
     }
   services.serviceNotice ("\002[\002Unrecognised Command\002]\002", "Chan", requestor);
}
void
  CHAN_FUNC (Chan::parseSEEN)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   
   else
     {
	channel = tokens.nextToken();
     }
   if(channel.empty())
     {
	services.serviceNotice("Usage: seen #channel nickname","Chan",origin);
	return;
     }

}

void
  CHAN_FUNC (Chan::parseSET)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }
   if(channel.empty())
     {
	services.serviceNotice("Usage: set #channel option value","Chan",origin);
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   if(!services.getChannel().isChanRegistered(channel))
     {
        services.serviceNotice("Error: That channel is not registered","chan",origin);
	return;
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = st.hasMoreTokens();
   while(more==true)
     {

	String currnick  = st.nextToken();
	if((services.getChannel().getChanAccess(channel,currnick)>400))
	  {
	     if(command=="log")
	       {
		  if(value=="true")
		    {
		       services.getChannel().setChanLog(channel,true);
		       services.serviceNotice("Channel logs have been enabled, the owner will receive a nightly email from this channel","Chan",origin);
		       return;
		    }
		  if(value=="false")
		    {
		       services.getChannel().setChanLog(channel,false);
		       services.serviceNotice("Channel logs have been disabled","Chan",origin);
		       return;
		    }
		  services.serviceNotice("Value must be true or false","Chan",origin);
		  return;

	       }
	     services.serviceNotice("Unsupported command","Chan",origin);
	     return;

	  }

	more = st.hasMoreTokens();
     }
   services.serviceNotice("You do not have enough access for that command","Chan",origin);
   return;
}

void
  CHAN_FUNC (Chan::parseLISTBAN)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel.empty())
     {
	services.serviceNotice("\002[\002Incorrect Command Usage\002]\002 Usage: listban #channel","chan",origin);
	return;
     }
   if(!services.getChannel().isChanRegistered(channel))
     {
	services.serviceNotice("\002[\002Fatal Error\002]\002 Channel not registered","chan",origin);
	return;
     }
   int tid = services.getChannel().getChanID(channel);
   services.serviceNotice("\002[\002Ban List\002]\002 for \002"+channel,"chan",origin);
   String tquery = "SELECT * from chanbans where chan='" + String::convert(tid) + "'";
   MysqlRes res = services.getDatabase().query(tquery);
   MysqlRow row;
   int j=0;
   while ((row = res.fetch_row()))
     {
	j++;
	String mask = ((std::string) row[2]);
	String setby = ((std::string) row[3]);
	String seton = ((std::string) row[4]);
	String expireon = ((std::string) row[5]);
	String reason = ((std::string) row[6]);
	String tosend = "\002[\002"+String::convert(j)+"\002]\002 Mask \002"+mask+"\002 SetBy \002"+setby+"\002 Date \002"+seton+"\002 Expires \002"+expireon+"\002 Reason \002"+reason+"\002";
	services.serviceNotice(tosend,"chan",origin);
     }
   services.serviceNotice("\002[\002Ban List\002]\002 Finished","chan",origin);
}
void
  CHAN_FUNC (Chan::parseINFO)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel.empty())
     {
	services.serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: info #channel","Chan",origin);
	return;
     }
   if(!services.getChannel().isChanRegistered(channel))
     {
	services.serviceNotice("\002[\002Fatal Error\002]\002 Channel not registered","Chan",origin);
	return;
     }
   int cid = services.getChannel().getChanID(channel);
   String nowner = services.getChannel().getChanOwner(cid);
   String ntotal = services.getChannel().getChanCount();
   String nuniq = String::convert(services.getChannel().getChanID(channel));
   String nnuniq = String::convert(services.getChannel().getOnlineChanID(channel));
   String toa = "\002[\002Channel Information\002]\002 for \002"+channel;
   String tob = "Owner : "+nowner;
   String toc = "Unique IDS: "+nuniq+"/"+nnuniq;
   services.serviceNotice(toa,"Chan",origin);
   services.serviceNotice(tob,"Chan",origin);
   services.serviceNotice(toc,"Chan",origin);
}
void
  CHAN_FUNC (Chan::parseADDUSER)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   String nickname = tokens.nextToken();
   String level = tokens.nextToken();
   if(channel=="" | nickname=="" | level=="")
     {
	services.serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: adduser #channel nickname level","Chan",origin);
	return;
     }
   if(!services.getNickname().isNickRegistered(nickname))
     {
	services.serviceNotice("\002[\002Fatal Error\002]\002 Nickname not registered","Chan",origin);
	return;
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick  = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	int waccess = level.toInt();
	int taccess = services.getChannel().getChanAccess(channel,nickname);
	if(waccess==access || waccess>access || waccess<1 || waccess>499)
	  {
	     services.serviceNotice("Error: You cannot add someone with higher, or equal access to your own","Chan",origin);
	     return;
	  }
	if(taccess>0)
	  {
	     services.serviceNotice("Error: That person already has access","Chan",origin);
	     return;
	  }
	services.getChannel().chanAddAccess(channel,nickname,level);
	services.log(origin,"Chan","Added "+nickname+" with level "+level,channel);
	services.serviceNotice("Command completed successfully","Chan",origin);
	return;
     }
}
void
  CHAN_FUNC (Chan::parseTOPIC)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   String topic = tokens.rest();
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>150)
	  {
	     services.getChannel().updateTopic(channel,topic);
	     services.getChannel().setTopic(channel,topic);
	     services.log(origin,"Chan","Set topic to "+topic,channel);
	     return;
	  }
	more = st.hasMoreTokens();

     }
   services.serviceNotice("Sorry, you do not have the required access in that channel","Chan",origin);
   return;
}

void
  CHAN_FUNC (Chan::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services.doHelp(origin,"chan",word,parm);
}

void
  CHAN_FUNC (Chan::parseBAN)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   String who = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" | who=="")
     {
	services.serviceNotice("Usage:  ban #channel nickname your optional reason goes here","Chan",origin);
	return;
     }
   if(reason=="")
     {
	reason = "You are banned";
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>150)
	  {
	     StringTokens st (who);
	     String temp1 = st.nextToken('@');
	     String temp2 = st.rest();
	     if(temp2.length()<2)
	       {
		  String tban = services.getNickname().getHost(temp1);
		  String toban = "*!*@"+tban;
		  who = toban;
	       }
	     int cid = services.getChannel().getChanID(channel);
	     services.serverMode(channel,"+b",who);
	     long newt = services.currentTime + 120;
	     services.getChannel().addChanBan(cid,who,origin,newt,reason);
	     String rs = "("+origin+"/"+currnick+") "+reason;
	     services.getChannel().banChan(channel,who,rs);
	     return;
	  }
     }
   services.serviceNotice("\002[\002No Access\002]\002","Chan",origin);

}

void
  CHAN_FUNC (Chan::parseREGISTER)
{
   String channel = tokens.nextToken();
   if (channel=="")
     {
	services.serviceNotice("Usage: register #channel","Chan",origin);
	return;
     }
   if ( channel[0] != '#' )
     {
	services.serviceNotice("Error: Channel names must begin with the '#' symbol","Chan",origin);
	return;
     }
   if(!services.getNickname().isNickRegistered(origin))
     {
	services.serviceNotice("Error: Your nickname is not registered","Chan",origin);
	return;
     }
   if(!services.getNickname().isIdentified(origin,origin))
     {
	services.serviceNotice("Error: You must be identified, and using the nickname you wish to own the channel","Chan",origin);
	return;
     }
   int owned = services.getChannel().ownedChannels(origin);
   if(owned>0)
     {
	services.serviceNotice("Error: You are only permitted own one channel per nickname on IRCDome","Chan",origin);
	return;
     }
   if(services.getChannel().isChanRegistered(channel))
     {
	services.serviceNotice("Error: That channel is already registered","Chan",origin);
	return;
     }
   services.getChannel().registerChannel(channel,origin);
   services.serviceNotice("Registration Successful","Chan",origin);
   services.log(origin,"Chan","Registered the channel",channel);
   return;
}

void
  CHAN_FUNC (Chan::parseOP)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	services.serviceNotice("Usage: op #channel optional list of nicknames to op","Chan",origin);
	return;
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>100)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(!services.isOp(origin,channel))
		    {
		       services.mode("Chan",channel,"+o",origin);
		       services.getChannel().internalOp(origin,channel);
		       services.log(origin,"Chan","Opped themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(!services.isOp(foo,channel))
	       {
		  services.mode("Chan",channel,"+o",foo);
		  services.getChannel().internalOp(foo,channel);
		  services.log(origin,"Chan","Opped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!services.isOp(foo,channel))
		    {
		       services.mode("Chan",channel,"+o",foo);
		       services.getChannel().internalOp(foo,channel);
		       services.log(origin,"Chan","Opped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   services.serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
   return;
}

void
  CHAN_FUNC (Chan::parseDEOP)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	services.serviceNotice("Usage: deop #channel optional list of nicknames to op","Chan",origin);
	return;
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>100)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(services.isOp(origin,channel))
		    {
		       services.mode("Chan",channel,"-o",origin);
		       services.getChannel().internalDeOp(origin,channel);
		       services.log(origin,"Chan","Deopped themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(services.isOp(foo,channel))
	       {
		  services.mode("Chan",channel,"-o",foo);
		  services.getChannel().internalDeOp(foo,channel);
		  services.log(origin,"Chan","Deopped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(services.isOp(foo,channel))
		    {
		       services.mode("Chan",channel,"-o",foo);
		       services.getChannel().internalDeOp(foo,channel);
		       services.log(origin,"Chan","Deopped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   services.serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
   return;
}

void
  CHAN_FUNC (Chan::parseVOICE)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	services.serviceNotice("Usage: voice #channel optional list of nicknames to op","Chan",origin);
	return;
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>50)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(!services.isVoice(origin,channel))
		    {
		       services.mode("Chan",channel,"+v",origin);
		       services.getChannel().internalVoice(origin,channel);
		       services.log(origin,"Chan","Voiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(!services.isVoice(foo,channel))
	       {
		  services.mode("Chan",channel,"+v",foo);
		  services.getChannel().internalVoice(foo,channel);
		  services.log(origin,"Chan","Voiced "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!services.isVoice(foo,channel))
		    {
		       services.mode("Chan",channel,"+v",foo);
		       services.getChannel().internalVoice(foo,channel);
		       services.log(origin,"Chan","Voiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   services.serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
   return;
}

void
  CHAN_FUNC (Chan::parseDEVOICE)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	services.serviceNotice("Usage: devoice #channel optional list of nicknames to op","Chan",origin);
	return;
     }
   String la = services.getNickname().getIDList(origin);
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>50)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(services.isVoice(origin,channel))
		    {
		       services.mode("Chan",channel,"-v",origin);
		       services.getChannel().internalDeVoice(origin,channel);
		       services.log(origin,"Chan","DeVoiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(services.isVoice(foo,channel))
	       {
		  services.mode("Chan",channel,"-v",foo);
		  services.getChannel().internalDeVoice(foo,channel);
		  services.log(origin,"Chan","DeVoiced "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(services.isVoice(foo,channel))
		    {
		       services.mode("Chan",channel,"-v",foo);
		       services.getChannel().internalDeVoice(foo,channel);
		       services.log(origin,"Chan","DeVoiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   services.serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
   return;
}

void
  CHAN_FUNC (Chan::parseKICK)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   String who = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" | who=="" | reason=="")
     {
	services.serviceNotice("Usage: kick #channel nickname Your reason here","Chan",origin);
	return;
     }
   String thelist = services.getNickname().getIDList(origin);
   StringTokens st (thelist);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services.getChannel().getChanAccess(channel,currnick);
	if(access>100)
	  {
	     if(who.toLower()=="chan")
	       {
		  String rs = "And why would you want to kick me? :-(";
		  services.serviceKick(channel,origin,rs);
		  return;
	       }
	     String rs = "("+origin+"/"+currnick+") "+reason;
	     services.serviceKick(channel,who,rs);
	     services.getChannel().internalDel(who,channel);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   services.serviceNotice("You do not have the required access to perform that command","Chan",origin);
   return;
}
void
  CHAN_FUNC (Chan::parseACCESS)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	services.serviceNotice("Usage: /msg Chan access #channel","Chan",origin);
	return;
     }
   bool foo = false;
   foo = services.getChannel().isChanRegistered(channel);
   if(foo==true)
     {
     }
   if(foo==false)
     {
	services.serviceNotice("That channel is not registered","Chan",origin);
	return;
     }
   services.serviceNotice("Channel access list for "+channel,"Chan",origin);
   int chanid = services.getChannel().getChanID(channel);
   String query = "SELECT nickid,access from chanaccess where chanid=" + String::convert(chanid);
   MysqlRes res = services.getDatabase().query(query);
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	String tnickid = ((std::string) row[0]).c_str();
	String taccess = ((std::string) row[1]).c_str();
	String tosend = "Nickname \002"+services.getNickname().getNick(tnickid.toInt())+"\002 Access \002"+taccess+"\002";
	services.serviceNotice(tosend,"Chan",origin);
     }
   services.log(origin,"Chan","Did a channel access",channel);
   res.free_result();
}

EXORDIUM_SERVICE_INIT_FUNCTION
{
   services.registerService(name,name,"ircdome.org","+dz",
			    "Channel Registration Services");
   services.serviceJoin(name,"#Debug");
   services.getChannel().synchChannels();
   return new Module("chan", new Chan(services, name));
}

