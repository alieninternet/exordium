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

#include "chan.h"
#include "tables.h"
#include <exordium/channel.h>
#include <kineircd/str.h>


using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::ChanModule;

struct Module::functionTableStruct const
  Module::functionTable[] =
{
     {"access", &Module::parseACCESS},
     {".access", &Module::parseACCESS},
     {"ban", &Module::parseBAN},
     {".ban", &Module::parseBAN},
     {".kick", &Module::parseKICK},
     {"kick", &Module::parseKICK},
     {".op", &Module::parseOP},
     {"op", &Module::parseOP},
     {".voice", &Module::parseVOICE},
     {"voice", &Module::parseVOICE},
     {"register", &Module::parseREGISTER},
     {"help", &Module::parseHELP},
     {"deop", &Module::parseDEOP},
     {".deop", &Module::parseDEOP},
     {"devoice", &Module::parseDEVOICE},
     {".devoice", &Module::parseDEVOICE},
     {"topic", &Module::parseTOPIC},
     {".topic", &Module::parseTOPIC},
     {"adduser", &Module::parseADDUSER},
     {".adduser", &Module::parseADDUSER},
     {"info", &Module::parseINFO},
     {".info", &Module::parseINFO},
     {"listban", &Module::parseLISTBAN},
     {".listban", &Module::parseLISTBAN},
     {".set", &Module::parseSET},
     {"set", &Module::parseSET},
     {"seen", &Module::parseSEEN},
     {".seen", &Module::parseSEEN},
     {0, 0}
};

void
  Module::parseLine (StringTokens& line, User& origin
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
  Module::parseLine (StringTokens& line, User& origin)
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
   origin.sendMessage("Unrecognized Command",getName());
}

  CHAN_FUNC (Module::parseSEEN)
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
	origin.sendMessage("Usage: seen #channel nickname",getName());
	return;
     }

}


  CHAN_FUNC (Module::parseSET)
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
	origin.sendMessage("Usage: set #channel option value",getName());
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   if(!services->getChannel().isChanRegistered(channel))
     {
        origin.sendMessage("Error: That channel is not registered",getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = st.hasMoreTokens();
   while(more==true)
     {

	String currnick  = st.nextToken();
	if((services->getChannel().getChanAccess(channel,currnick)>400))
	  {
	     if(command=="log")
	       {
		  if(value=="true")
		    {
		       services->getChannel().setChanLog(channel,true);
		       origin.sendMessage("Channel logs have been enabled, the owner will receive a nightly email from this channel",getName());
		       return;
		    }
		  if(value=="false")
		    {
		       services->getChannel().setChanLog(channel,false);
		       origin.sendMessage("Channel logs have been disabled",getName());
		       return;
		    }
		  origin.sendMessage("Value must be true or false",getName());
		  return;

	       }
	     origin.sendMessage("Unsupported command",getName());
	     return;

	  }

	more = st.hasMoreTokens();
     }
   origin.sendMessage("You do not have enough access for that command",getName());
   return;
}


  CHAN_FUNC (Module::parseLISTBAN)
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
	origin.sendMessage("\002[\002Incorrect Command Usage\002]\002 Usage: listban #channel",getName());
	return;
     }
   if(!services->getChannel().isChanRegistered(channel))
     {
	origin.sendMessage("\002[\002Fatal Error\002]\002 Channel not registered",getName());
	return;
     }
   int tid = services->getChannel().getChanID(channel);
   origin.sendMessage("\002[\002Ban List\002]\002 for \002"+channel,getName());

   int nbRes = services->getDatabase().dbSelect("*", "chanbans", "chan='"+String::convert(tid)+"'");

   int j=0;
   for (int i=0; i<nbRes; i++)
     {
	j++;
	String mask = services->getDatabase().dbGetValue(2);
	String setby = services->getDatabase().dbGetValue(3);
	String seton = services->getDatabase().dbGetValue(4);
	String expireon = services->getDatabase().dbGetValue(5);
	String reason = services->getDatabase().dbGetValue(6);
	String tosend = "\002[\002"+String::convert(j)+"\002]\002 Mask \002"+mask+"\002 SetBy \002"+setby+"\002 Date \002"+seton+"\002 Expires \002"+expireon+"\002 Reason \002"+reason+"\002";
	origin.sendMessage(tosend,getName());
        services->getDatabase().dbGetRow();
     }

   origin.sendMessage("\002[\002Ban List\002]\002 Finished",getName());
}

  CHAN_FUNC (Module::parseINFO)
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
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: info #channel",getName());
	return;
     }
   if(!services->getChannel().isChanRegistered(channel))
     {
        origin.sendMessage("\002[\002Fatal Error\002]\002 Channel not registered",getName());
	return;
     }
   int cid = services->getChannel().getChanID(channel);
   String nowner = services->getChannel().getChanOwner(cid);
   String ntotal = services->getChannel().getChanCount();
   String nuniq = String::convert(services->getChannel().getChanID(channel));
   String nnuniq = String::convert(services->getChannel().getOnlineChanID(channel));
   String toa = "\002[\002Channel Information\002]\002 for \002"+channel;
   String tob = "Owner : "+nowner;
   String toc = "Unique IDS: "+nuniq+"/"+nnuniq;
   origin.sendMessage(toa,getName());
   origin.sendMessage(tob,getName());
   origin.sendMessage(toc,getName());
}

  CHAN_FUNC (Module::parseADDUSER)
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
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: adduser #channel nickname level",getName());
	return;
     }
   if(!services->isNickRegistered(nickname))
     {
	origin.sendMessage("\002[\002Fatal Error\002]\002 Nickname not registered",getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick  = st.nextToken();
	int access = services->getChannel().getChanAccess(channel,currnick);
	int waccess = level.toInt();
	int taccess = services->getChannel().getChanAccess(channel,nickname);
	if(waccess==access || waccess>access || waccess<1 || waccess>499)
	  {
	     origin.sendMessage("Error: You cannot add someone with higher, or equal access to your own",getName());
	     return;
	  }
	if(taccess>0)
	  {
	     origin.sendMessage("Error: That person already has access",getName());
	     return;
	  }
	services->getChannel().chanAddAccess(channel,nickname,level);
	services->log(origin,"Chan","Added "+nickname+" with level "+level,channel);
	origin.sendMessage("Command completed successfully",getName());
	return;
     }
}

  CHAN_FUNC (Module::parseTOPIC)
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
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>150)
	  {
	     services->getChannel().updateTopic(channel,topic);
	     services->getChannel().setTopic(channel,topic);
	     services->log(origin,"Chan","Set topic to "+topic,channel);
	     return;
	  }
	more = st.hasMoreTokens();

     }
   origin.sendMessage("Sorry, you do not have the required access in that channel",getName());
   return;
}


  CHAN_FUNC (Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getName(),word,parm);
}


  CHAN_FUNC (Module::parseBAN)
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
	origin.sendMessage("Usage:  ban #channel nickname your optional reason goes here",getName());
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
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>150)
	  {
	     StringTokens st (who);
	     String temp1 = st.nextToken('@');
	     String temp2 = st.rest();
	     if(temp2.length()<2)
	       {
		  User *ptr = services->findUser(temp1);
		  String tban = ptr->getHost();
		  String toban = "*!*@"+tban;
		  who = toban;
	       }
	     int cid = services->getChannel().getChanID(channel);
	     services->serverMode(channel,"+b",who);
	     long newt = services->currentTime + 120;
	     services->getChannel().addChanBan(cid,who,origin.getNickname(),newt,reason);
	     String rs = "("+origin.getNickname()+"/"+currnick+") "+reason;
	     services->getChannel().banChan(channel,who,rs);
	     return;
	  }
     }
   origin.sendMessage("\002[\002No Access\002]\002",getName());

}


  CHAN_FUNC (Module::parseREGISTER)
{
   String channel = tokens.nextToken();
   if (channel=="")
     {
        origin.sendMessage("Usage: register #channel",getName());
	return;
     }
   if ( channel[0] != '#' )
     {
	origin.sendMessage("Error: Channel names must begin with the '#' symbol",getName());
	return;
     }
   if(!services->isNickRegistered(origin.getNickname()))
     {
	origin.sendMessage("Error: Your nickname is not registered",getName());
	return;
     }
   if(!origin.isIdentified(origin.getNickname()))
     {
	origin.sendMessage("Error: You must be identified, and using the nickname you wish to own the channel",getName());
	return;
     }
   int owned = services->getChannel().ownedChannels(origin.getNickname());
   if(owned>0)
     {
	origin.sendMessage("Error: You are only permitted own one channel per nickname on IRCDome",getName());
	return;
     }
   if(services->getChannel().isChanRegistered(channel))
     {
	origin.sendMessage("Error: That channel is already registered",getName());
	return;
     }
   services->getChannel().registerChannel(channel,origin.getNickname());
   origin.sendMessage("Registration Successful",getName());
   services->log(origin,"Chan","Registered the channel",channel);
   return;
}


  CHAN_FUNC (Module::parseOP)
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
	origin.sendMessage("Usage: op #channel optional list of nicknames to op",getName());
	return;
     }
   String la = origin.getIDList();
#ifdef DEBUG
   std::cout << "parseOP(); la is " << la << std::endl;
#endif
   StringTokens st (la);
#ifdef DEBUG
   std::cout << "ParseOP(); st is " << st << std::endl;
#endif
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>100)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(!services->isOp(origin.getNickname(),channel))
		    {
		       services->mode("Chan",channel,"+o",origin.getNickname());
		       services->getChannel().internalOp(origin.getNickname(),channel);
		       services->log(origin,"Chan","Opped themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(!services->isOp(foo,channel))
	       {
		  services->mode("Chan",channel,"+o",foo);
		  services->getChannel().internalOp(foo,channel);
		  services->log(origin,"Chan","Opped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!services->isOp(foo,channel))
		    {
		       services->mode("Chan",channel,"+o",foo);
		       services->getChannel().internalOp(foo,channel);
		       services->log(origin,"Chan","Opped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
   return;
}


  CHAN_FUNC (Module::parseDEOP)
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
	origin.sendMessage("Usage: deop #channel optional list of nicknames to op",getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>100)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(services->isOp(origin.getNickname(),channel))
		    {
		       services->mode("Chan",channel,"-o",origin.getNickname());
		       services->getChannel().internalDeOp(origin.getNickname(),channel);
		       services->log(origin,"Chan","Deopped themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(services->isOp(foo,channel))
	       {
		  services->mode("Chan",channel,"-o",foo);
		  services->getChannel().internalDeOp(foo,channel);
		  services->log(origin,"Chan","Deopped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(services->isOp(foo,channel))
		    {
		       services->mode("Chan",channel,"-o",foo);
		       services->getChannel().internalDeOp(foo,channel);
		       services->log(origin,"Chan","Deopped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
   return;
}


  CHAN_FUNC (Module::parseVOICE)
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
	origin.sendMessage("Usage: voice #channel optional list of nicknames to op",getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>50)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(!services->isVoice(origin.getNickname(),channel))
		    {
		       services->mode("Chan",channel,"+v",origin.getNickname());
		       services->getChannel().internalVoice(origin.getNickname(),channel);
		       services->log(origin,"Chan","Voiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(!services->isVoice(foo,channel))
	       {
		  services->mode("Chan",channel,"+v",foo);
		  services->getChannel().internalVoice(foo,channel);
		  services->log(origin,"Chan","Voiced "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!services->isVoice(foo,channel))
		    {
		       services->mode("Chan",channel,"+v",foo);
		       services->getChannel().internalVoice(foo,channel);
		       services->log(origin,"Chan","Voiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
   return;
}


  CHAN_FUNC (Module::parseDEVOICE)
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
	origin.sendMessage("Usage: devoice #channel optional list of nicknames to op",getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>50)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(services->isVoice(origin.getNickname(),channel))
		    {
		       services->mode("Chan",channel,"-v",origin.getNickname());
		       services->getChannel().internalDeVoice(origin.getNickname(),channel);
		       services->log(origin,"Chan","DeVoiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     if(services->isVoice(foo,channel))
	       {
		  services->mode("Chan",channel,"-v",foo);
		  services->getChannel().internalDeVoice(foo,channel);
		  services->log(origin,"Chan","DeVoiced "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(services->isVoice(foo,channel))
		    {
		       services->mode("Chan",channel,"-v",foo);
		       services->getChannel().internalDeVoice(foo,channel);
		       services->log(origin,"Chan","DeVoiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
   return;
}


  CHAN_FUNC (Module::parseKICK)
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
	origin.sendMessage("Usage: kick #channel nickname Your reason here",getName());
	return;
     }
   String thelist = origin.getIDList();
   StringTokens st (thelist);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	int access = services->getChannel().getChanAccess(channel,currnick);
	if(access>100)
	  {
	     if(who.toLower()=="chan")
	       {
		  String rs = "And why would you want to kick me? :-(";
		  services->serviceKick(channel,origin.getNickname(),rs);
		  return;
	       }
	     String rs = "("+origin.getNickname()+"/"+currnick+") "+reason;
	     services->serviceKick(channel,who,rs);
	     services->getChannel().internalDel(who,channel);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage("You do not have the required access to perform that command",getName());
   return;
}

  CHAN_FUNC (Module::parseACCESS)
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
	origin.sendMessage("Usage: /msg Chan access #channel",getName());
	return;
     }
   bool foo = false;
   foo = services->getChannel().isChanRegistered(channel);
   if(foo==true)
     {
     }
   if(foo==false)
     {
	origin.sendMessage("That channel is not registered",getName());
	return;
     }
   origin.sendMessage("Channel access list for "+channel,getName());
   int chanid = services->getChannel().getChanID(channel);
   int nbRes = services->getDatabase().dbSelect("nickid,access", "chanaccess", "chanid="+String::convert(chanid));

   for(int i=0; i<nbRes; i++)
    {
	String tnickid = services->getDatabase().dbGetValue(0);
	String taccess = services->getDatabase().dbGetValue(1);
	String tosend = "Nickname \002"+services->getNick(tnickid.toInt())+"\002 Access \002"+taccess+"\002";
	origin.sendMessage(tosend,getName());
        services->getDatabase().dbGetRow();
    }
   services->log(origin,"Chan","Did a channel access",channel);
}

EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }

void
  Module::handleAway(Exordium::User& origin, const String &reason)
{

   if(reason=="")
     {
	/* User is coming back - do nothing */
	return;
     }
   if(origin.deopAway())
     {
	if(services->getDatabase().dbSelect("chanid","chanstatus",
					   "nickid="+origin.getOnlineIDString()+
					   " AND status=2") > 0)
	  {
	     String foo = services->getDatabase().dbGetValue();
	     String cname = services->getChannel().getChanIDName(foo.toInt());
	     String cstr = origin.getNickname()+" "+origin.getNickname();
	     services->serverMode(cname,"-o+v",cstr);
	     services->getChannel().internalVoice(origin.getNickname(),cname);
	     services->getChannel().internalDeOp(origin.getNickname(),cname);
	     
	     
	  }
	
     }
   
}


// Module information structure
const Module::moduleInfo_type Module::moduleInfo = {
   "Channel Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::CLIENT_AWAY /* AWAY's */
};


// Start the service
void Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;
   
   // Attempt to affirm our database tables..
   unsigned int i = 0;
   while (Tables::tables[i] != 0) {
      // Try to affirm this table..
      if (!services->getDatabase().affirmTable(*(Tables::tables[i]))) {
	 services->logLine(String("Unable to affirm mod_chan database "
				  "table '") +
			   Tables::tables[i]->name + "'",
			   Log::Fatality); 
	 return; // How do we tell services we did not start happily?!
      }
      
      // Next table..
      i++;
   }
   
   // Register ourself to the network
   services->registerService(getName(), getName(), 
			    getConfigData().getHostname(), "+dz",
			    getConfigData().getDescription());
   services->serviceJoin(getName(),"#Debug");
   services->getChannel().synchChannels();
}


// Stop the service - bye bye!
void Module::stop(const String& reason)
{
   // Yes, we would like to QUIT here properly.. :( This is dodgey  -pickle
   services->queueAdd(":" + getName() + " QUIT :" + reason);
}
