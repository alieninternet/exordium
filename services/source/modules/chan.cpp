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

#include "include/chan.h"
#include "exordium/channel.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

using LibAIS::String;
using LibAIS::StringTokens;
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
  Chan::parseLine (StringTokens& line, User& origin
		   , const String& chan)
{
   StringTokens& st = line;
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
  Chan::parseLine (StringTokens& line, User& origin)
{
   StringTokens& st = line;
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
   origin.sendMessage("Unrecognized Command",myName);
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
	origin.sendMessage("Usage: seen #channel nickname",myName);
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
	origin.sendMessage("Usage: set #channel option value",myName);
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   if(!services.getChannel().isChanRegistered(channel))
     {
        origin.sendMessage("Error: That channel is not registered",myName);
	return;
     }
   String la = origin.getIDList();
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
		       origin.sendMessage("Channel logs have been enabled, the owner will receive a nightly email from this channel",myName);
		       return;
		    }
		  if(value=="false")
		    {
		       services.getChannel().setChanLog(channel,false);
		       origin.sendMessage("Channel logs have been disabled",myName);
		       return;
		    }
		  origin.sendMessage("Value must be true or false",myName);
		  return;

	       }
	     origin.sendMessage("Unsupported command",myName);
	     return;

	  }

	more = st.hasMoreTokens();
     }
   origin.sendMessage("You do not have enough access for that command",myName);
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
	origin.sendMessage("\002[\002Incorrect Command Usage\002]\002 Usage: listban #channel",myName);
	return;
     }
   if(!services.getChannel().isChanRegistered(channel))
     {
	origin.sendMessage("\002[\002Fatal Error\002]\002 Channel not registered",myName);
	return;
     }
   int tid = services.getChannel().getChanID(channel);
   origin.sendMessage("\002[\002Ban List\002]\002 for \002"+channel,myName);
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
	origin.sendMessage(tosend,myName);
     }
   origin.sendMessage("\002[\002Ban List\002]\002 Finished",myName);
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
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: info #channel",myName);
	return;
     }
   if(!services.getChannel().isChanRegistered(channel))
     {
        origin.sendMessage("\002[\002Fatal Error\002]\002 Channel not registered",myName);
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
   origin.sendMessage(toa,myName);
   origin.sendMessage(tob,myName);
   origin.sendMessage(toc,myName);
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
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: adduser #channel nickname level",myName);
	return;
     }
   if(!services.isNickRegistered(nickname))
     {
	origin.sendMessage("\002[\002Fatal Error\002]\002 Nickname not registered",myName);
	return;
     }
   String la = origin.getIDList();
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
	     origin.sendMessage("Error: You cannot add someone with higher, or equal access to your own",myName);
	     return;
	  }
	if(taccess>0)
	  {
	     origin.sendMessage("Error: That person already has access",myName);
	     return;
	  }
	services.getChannel().chanAddAccess(channel,nickname,level);
	services.log(origin.getNickname(),"Chan","Added "+nickname+" with level "+level,channel);
	origin.sendMessage("Command completed successfully",myName);
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
   String la = origin.getIDList();
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
	     services.log(origin.getNickname(),"Chan","Set topic to "+topic,channel);
	     return;
	  }
	more = st.hasMoreTokens();

     }
   origin.sendMessage("Sorry, you do not have the required access in that channel",myName);
   return;
}

void
  CHAN_FUNC (Chan::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services.doHelp(origin.getNickname(),myName,word,parm);
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
	origin.sendMessage("Usage:  ban #channel nickname your optional reason goes here",myName);
	return;
     }
   if(reason=="")
     {
	reason = "You are banned";
     }
   String la = origin.getIDList();
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
		  User *ptr = services.findUser(temp1);
		  String tban = ptr->getHost();
		  String toban = "*!*@"+tban;
		  who = toban;
	       }
	     int cid = services.getChannel().getChanID(channel);
	     services.serverMode(channel,"+b",who);
	     long newt = services.currentTime + 120;
	     services.getChannel().addChanBan(cid,who,origin.getNickname(),newt,reason);
	     String rs = "("+origin.getNickname()+"/"+currnick+") "+reason;
	     services.getChannel().banChan(channel,who,rs);
	     return;
	  }
     }
   origin.sendMessage("\002[\002No Access\002]\002",myName);

}

void
  CHAN_FUNC (Chan::parseREGISTER)
{
   String channel = tokens.nextToken();
   if (channel=="")
     {
        origin.sendMessage("Usage: register #channel",myName);
	return;
     }
   if ( channel[0] != '#' )
     {
	origin.sendMessage("Error: Channel names must begin with the '#' symbol",myName);
	return;
     }
   if(!services.isNickRegistered(origin.getNickname()))
     {
	origin.sendMessage("Error: Your nickname is not registered",myName);
	return;
     }
   if(!origin.isIdentified(origin.getNickname()))
     {
	origin.sendMessage("Error: You must be identified, and using the nickname you wish to own the channel",myName);
	return;
     }
   int owned = services.getChannel().ownedChannels(origin.getNickname());
   if(owned>0)
     {
	origin.sendMessage("Error: You are only permitted own one channel per nickname on IRCDome",myName);
	return;
     }
   if(services.getChannel().isChanRegistered(channel))
     {
	origin.sendMessage("Error: That channel is already registered",myName);
	return;
     }
   services.getChannel().registerChannel(channel,origin.getNickname());
   origin.sendMessage("Registration Successful",myName);
   services.log(origin.getNickname(),"Chan","Registered the channel",channel);
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
	origin.sendMessage("Usage: op #channel optional list of nicknames to op",myName);
	return;
     }
   String la = origin.getIDList();
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
		  if(!services.isOp(origin.getNickname(),channel))
		    {
		       services.mode("Chan",channel,"+o",origin.getNickname());
		       services.getChannel().internalOp(origin.getNickname(),channel);
		       services.log(origin.getNickname(),"Chan","Opped themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(!services.isOp(foo,channel))
	       {
		  services.mode("Chan",channel,"+o",foo);
		  services.getChannel().internalOp(foo,channel);
		  services.log(origin.getNickname(),"Chan","Opped "+foo,channel);
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
		       services.log(origin.getNickname(),"Chan","Opped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",myName);
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
	origin.sendMessage("Usage: deop #channel optional list of nicknames to op",myName);
	return;
     }
   String la = origin.getIDList();
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
		  if(services.isOp(origin.getNickname(),channel))
		    {
		       services.mode("Chan",channel,"-o",origin.getNickname());
		       services.getChannel().internalDeOp(origin.getNickname(),channel);
		       services.log(origin.getNickname(),"Chan","Deopped themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(services.isOp(foo,channel))
	       {
		  services.mode("Chan",channel,"-o",foo);
		  services.getChannel().internalDeOp(foo,channel);
		  services.log(origin.getNickname(),"Chan","Deopped "+foo,channel);
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
		       services.log(origin.getNickname(),"Chan","Deopped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",myName);
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
	origin.sendMessage("Usage: voice #channel optional list of nicknames to op",myName);
	return;
     }
   String la = origin.getIDList();
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
		  if(!services.isVoice(origin.getNickname(),channel))
		    {
		       services.mode("Chan",channel,"+v",origin.getNickname());
		       services.getChannel().internalVoice(origin.getNickname(),channel);
		       services.log(origin.getNickname(),"Chan","Voiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(!services.isVoice(foo,channel))
	       {
		  services.mode("Chan",channel,"+v",foo);
		  services.getChannel().internalVoice(foo,channel);
		  services.log(origin.getNickname(),"Chan","Voiced "+foo,channel);
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
		       services.log(origin.getNickname(),"Chan","Voiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",myName);
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
	origin.sendMessage("Usage: devoice #channel optional list of nicknames to op",myName);
	return;
     }
   String la = origin.getIDList();
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
		  if(services.isVoice(origin.getNickname(),channel))
		    {
		       services.mode("Chan",channel,"-v",origin.getNickname());
		       services.getChannel().internalDeVoice(origin.getNickname(),channel);
		       services.log(origin.getNickname(),"Chan","DeVoiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(services.isVoice(foo,channel))
	       {
		  services.mode("Chan",channel,"-v",foo);
		  services.getChannel().internalDeVoice(foo,channel);
		  services.log(origin.getNickname(),"Chan","DeVoiced "+foo,channel);
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
		       services.log(origin.getNickname(),"Chan","DeVoiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",myName);
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
	origin.sendMessage("Usage: kick #channel nickname Your reason here",myName);
	return;
     }
   String thelist = origin.getIDList();
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
		  services.serviceKick(channel,origin.getNickname(),rs);
		  return;
	       }
	     String rs = "("+origin.getNickname()+"/"+currnick+") "+reason;
	     services.serviceKick(channel,who,rs);
	     services.getChannel().internalDel(who,channel);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage("You do not have the required access to perform that command",myName);
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
	origin.sendMessage("Usage: /msg Chan access #channel",myName);
	return;
     }
   bool foo = false;
   foo = services.getChannel().isChanRegistered(channel);
   if(foo==true)
     {
     }
   if(foo==false)
     {
	origin.sendMessage("That channel is not registered",myName);
	return;
     }
   origin.sendMessage("Channel access list for "+channel,myName);
   int chanid = services.getChannel().getChanID(channel);
   String query = "SELECT nickid,access from chanaccess where chanid=" + String::convert(chanid);
   MysqlRes res = services.getDatabase().query(query);
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	String tnickid = ((std::string) row[0]).c_str();
	String taccess = ((std::string) row[1]).c_str();
	String tosend = "Nickname \002"+services.getNick(tnickid.toInt())+"\002 Access \002"+taccess+"\002";
	origin.sendMessage(tosend,myName);
     }
   services.log(origin.getNickname(),"Chan","Did a channel access",channel);
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

