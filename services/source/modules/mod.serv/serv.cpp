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

#include "serv.h"
#include "tables.h"
#include <exordium/services.h>
#include <exordium/channel.h>
#include <kineircd/str.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::ServModule;


const Module::functionTableStruct Module::functionTable[] = {
     { "clist",		&Module::parseCLIST },
     { "delnick",	&Module::parseDELNICK },
     { "elist",		&Module::parseELIST },
     { "nlist",		&Module::parseNLIST },
     { "helpon",	&Module::parseHELPON },
     { "help",		&Module::parseHELP },
     { "user",		&Module::parseUSER },
     { "raw",		&Module::parseRAW },
     { "chan",		&Module::parseCHAN },
     { "die",		&Module::parseDIE },
     { "news",		&Module::parseNEWS },
     { "setpass",	&Module::parseSETPASS },
     { 0, 0}
};


void
  Module::parseLine (StringTokens& line, User& origin)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	if (command == functionTable[i].command)
	  {
	     int required = services->getRequiredAccess(getName(),command.toLower());
	     int access = origin.getAccess(getName());
	     if(required>access)
	       {
		  origin.sendMessage("You do not have enough access for that command",getName());
		  String togo = origin.getNickname()+" tried to use \002"+command+"\002";
		  services->logLine(togo, Log::Warning);
		  return;
	       }

	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage ("Unrecognised Command", getName());
}


  SERV_FUNC (Module::parseDIE)
{
   String reason = tokens.rest();
   if(reason=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: die reason for shutdown here",getName());
	return;
     }

   String togo = "\002"+origin.getNickname()+"\002 - "+reason;
   services->shutdown(togo);
}


  SERV_FUNC (Module::parseSETPASS)
{
   String who = tokens.nextToken();
   String newpass = tokens.nextToken();
   if(who=="" | newpass=="")
     {
	String togo = "Usage: setpass nickname newpassword";
	origin.sendMessage(togo,getName());
	return;
     }
   if(!services->isNickRegistered(who))
     {

	origin.sendMessage("Error: Nickname is not registered",getName());
	return;
     }
   String epass =  String::convert(services->generatePassword(who,newpass));
   services->getDatabase().dbUpdate("nicks", "password='"+epass+"'", "nickname='"+who+"'");
   String togo = "\002"+origin.getNickname()+"\002 changed password for nickname "+who+" to [HIDDEN]";
   services->logLine(togo);
}


  SERV_FUNC (Module::parseRAW)
{
   std::string c = tokens.rest();
   services->queueAdd(c);
   String togo = origin.getNickname()+" did \002RAW\002 - "+c;
   services->logLine(String(togo), Log::Warning);
}


  SERV_FUNC (Module::parseNEWS)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("Usage: news add/del/list",getName());
	return;
     }
   if(command=="list")
     {
        int nbRes = services->getDatabase().dbSelect("news");

	for(int i=0; i<nbRes; i++)
	  {
	     String id = services->getDatabase().dbGetValue(0);
	     String level = services->getDatabase().dbGetValue(1);
	     String expires = services->getDatabase().dbGetValue(2);
	     String txt = services->getDatabase().dbGetValue(3);
	     String togo = "ID \002[\002"+id+"\002]\002 Level \002[\002"+level+"\002]\002 Expires \002[\002"+expires+"\002]\002 Text \002[\002"+txt+"\002]\002";
	     origin.sendMessage(togo,getName());
             services->getDatabase().dbGetRow();
	  }

     }
   if(command=="del")
     {
	String id = tokens.nextToken();
	if(id.empty())
	  {
	     origin.sendMessage("\002[\002Fatal Error\002]\002 Usage: del ID",getName());
	     return;
	  }
        services->getDatabase().dbDelete("news", "id="+id);
	origin.sendMessage("News has been deleted",getName());
	return;
     }
   if(command=="add")
     {
	String type = tokens.nextToken();
	String expires = tokens.nextToken();
	String text = tokens.rest();
	if(type=="" | expires=="" | text=="")
	  {
	     origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: news add type expires news text here",getName());
	     return;
	  }
	int nexpires = expires.toInt();
	nexpires = services->currentTime + (nexpires * 3600);
	if(services->currentTime>nexpires)
	  {
	     origin.sendMessage("\002[\002Fatal Error\002]\002 Your expiry time cannot be in the past",getName());
	     return;
	  }
        services->getDatabase().dbInsert("news", "'','"+type+"','"+String::convert(nexpires)+"','"+text+"'");
	origin.sendMessage("New news item added successfully",getName());
     }

}


  SERV_FUNC (Module::parseCHAN)
{
   String command = tokens.nextToken();
   String channel = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("Usage: chan add/del/mod",getName());
	return;
     }
   if(command=="mod")
     {
	String newowner = tokens.nextToken();
	if(channel=="" || newowner=="")
	  {
	     origin.sendMessage("Usage: chan mod #channel NewOwner",getName());
	     return;
	  }
	if(!services->isNickRegistered(newowner))
	  {
	     origin.sendMessage("Error: New owner's nickname is not registered",getName());
	     return;
	  }
	String newtopic = "This channel is now owned by "+newowner;
	services->getChannel().setTopic(channel,newtopic);
	services->getChannel().updateTopic(channel,newtopic);
	int chanid = services->getChannel().getChanID(channel);
	String oldowner = services->getChannel().getChanOwner(chanid);
	String togo = origin.getNickname()+" changed \002ownership\002 of "+channel+" "+oldowner+"->"+newowner;
	services->logLine(String(togo));
	services->getChannel().chanDelAccess(channel,oldowner);
	services->getChannel().chanAddAccess(channel,newowner,"500");
        services->getDatabase().dbUpdate("chans", "owner='"+newowner+"'", "name='"+channel+"'");
	services->log(origin,getName(),String("Changed ownership of ")+channel+" to "+newowner+" ("+oldowner+")");
	return;
     }
   if(command=="del")
     {
	String reason = tokens.rest();
	if(channel=="" || reason=="")
	  {
	     origin.sendMessage("Usage: chan del #channel reason",getName());
	     return;
	  }
	String togo = origin.getNickname() + "\002 de-registered\002 "+channel+" for \002"+reason+"\002";
	services->logLine(String(togo));
	services->getChannel().deregisterChannel(channel,reason);
	services->log(origin,getName(),String("Deregistered ")+channel+" for "+reason);
	return;
     }
   if(command=="add")
     {
	String thenick = tokens.nextToken();
	if(channel=="" || thenick=="")
	  {
	     origin.sendMessage("Usage: chan add #channel owner",getName());
	     return;
	  }
	if(services->getChannel().isChanRegistered(channel))
	  {
	     origin.sendMessage("That channel is already registered - Try mod",getName());
	     return;
	  }
	if(!services->isNickRegistered(thenick))
	  {
	     origin.sendMessage("Target nickname does not exist",getName());
	     return;
	  }
	String togo = origin.getNickname() + "\002 registered\002 " + channel + " to "+thenick;
	services->logLine(String(togo));
	services->getChannel().registerChannel(channel,thenick);
	services->log(origin,getName(),String("Registered ")+channel+" to "+thenick);
	return;
     }
}


  SERV_FUNC (Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getName(),word,parm);
   String tolog = "Did HELP on word " + word + " parm " + parm;
   services->log(origin,getName(),String(tolog));
}


  SERV_FUNC (Module::parseUSER)
{
   String command = tokens.nextToken();
   String toadd = tokens.nextToken();
   String level = tokens.nextToken();
   int access = origin.getAccess(getName());
   if(command=="")
     {
	origin.sendMessage("Commands are : add/del/mod/list",getName());
	return;
     }
   if(command=="mod")
     {
	if(toadd=="" || level=="")
	  {
	     origin.sendMessage("Usage: user mod nickname level",getName());
	     return;
	  }
	if(toadd==origin.getNickname())
	  {
	     origin.sendMessage("Silly billy... you can't mod yourself!",getName());
	     return;
	  }
	int ilevel = level.toInt();
	if(ilevel>access || ilevel==access)
	  {
	     origin.sendMessage("Error: You cannot set someones access higher than, or equal to your own",getName());
	     return;
	  }
	int taccess = origin.getAccess(getName());
	if(taccess>access)
	  {
	     origin.sendMessage("Error: That person has higher access than you",getName());
	     String togo = origin.getNickname()+" tried to modify access for a higher user than themselves ("+toadd+")";
	     services->logLine(String(togo), Log::Warning);
	     return;
	  }
	if(taccess==access)
	  {
	     origin.sendMessage("Error: That person has the same access as you",getName());
	     return;
	  }
	String togo = origin.getNickname() + " modified access for \002"+toadd+"\002 "+String::convert(taccess)+"->"+level;
	services->logLine(togo);
        services->getDatabase().dbUpdate("access", "access='"+level+"'", "nickname='"+toadd+"'");
	services->log(origin,getName(),String("Modified access for ")+toadd+" from "+String::convert(taccess)+"->"+level);
	return;
     }
   if(command=="list")
     {
        int nbRes = services->getDatabase().dbSelect("*", "access", "service='serv'", "id");

	for(int i=0; i<nbRes; i++)
	  {
	     String nickname = services->getDatabase().dbGetValue(1);
	     String access = services->getDatabase().dbGetValue(3);
	     String togo = "\002"+nickname+"\002 has level \002"+access;
	     origin.sendMessage(String(togo),getName());
             services->getDatabase().dbGetRow();
	  }
	return;
     }
   if(command=="del")
     {
	if(toadd=="")
	  {
	     origin.sendMessage("Usage is user del nickname",getName());
	     return;
	  }
	// Blah only temp.
	String blah = getName();
	if(services->getAccess(blah,toadd)==0)
	  {
	     origin.sendMessage("That person does not have access to serv",getName());
	     return;
	  }
	int faccess = services->getAccess(blah,toadd);
	if(faccess>access)
	  {
	     origin.sendMessage("You do not have enough access to perform that operation on a staff nickname",getName());
	     String togo = origin.getNickname() + " tried to use \002userdel\002 on a \002staff\002 nickname";
	     services->logLine(String(togo), Log::Warning);
	     return;
	  }
        services->getDatabase().dbDelete("access", "service='serv' AND nickname='" + toadd+"'");
	origin.sendMessage("Command complete",getName());
	String togo = origin.getNickname() + " deleted \002 " + toadd + "\002 from Serv";
	services->logLine(String(togo), Log::Warning);
	services->log(origin,getName(),"Deleted "+toadd+" from Serv");
	return;
     }
   if(command=="add")
     {
	if(toadd=="" || level=="")
	  {
	     origin.sendMessage("Usage: user add nickname level",getName());
	     return;
	  }
	if(origin.getNickname().toLower()==toadd.toLower())
	  {
	     origin.sendMessage("You can't adduser yourself, silly.",getName());
	     return;
	  }
	if(!services->isNickRegistered(toadd))
	  {
	     origin.sendMessage("That nickname is not registered",getName());
	     return;
	  }
	// Again temp.
	String foo = getName();
	if(services->getAccess(foo,toadd)>0)
	  {
	     origin.sendMessage("That person already has access, try mod",getName());
	     return;
	  }
	if(level.toInt()<1)
	  {
	     origin.sendMessage("Error: You cannot add someone with less than 1 access",getName());
	     return;
	  }
	if(level.toInt()>499)
	  {
	     origin.sendMessage("Error: You cannot add someone with higher than 499 access",getName());
	     return;
	  }
        services->getDatabase().dbInsert("access", "'','" + toadd + "','serv','" + level + "'");
	origin.sendMessage("Command completed",getName());
	String togo = origin.getNickname()+" added \002"+toadd+"\002 to Serv with level \002"+level;
	services->logLine(String(togo), Log::Warning);
	String tolog = "Added "+toadd+" to Serv with level "+toadd;
	services->log(origin,getName(),String(tolog));
	return;
     }
   origin.sendMessage("Uncognised sub-command",getName());
   return;
}


  SERV_FUNC (Module::parseHELPON)
{
   int access = origin.getAccess(getName());
   if(access>50)
     {
	origin.sendMessage("You are now an IRCDome services assistant",getName());
	String tosend = ":services->ircdome.org HELPER "+origin.getNickname()+" "+String::convert(access);
	services->queueAdd(String(tosend));
	services->log(origin,getName(),"Become a services helper at level "+String::convert(access));
	return;
     }
   services->log(origin,"Serv","Failed to become a helper (not enough access)");
   String tosend = origin.getNickname()+" failed to become a helper - Not enough access";
   services->logLine(tosend, Log::Warning);
}


  SERV_FUNC (Module::parseNLIST)
{
   String tomatch = tokens.nextToken();
   String dest = tokens.nextToken();
   if(tomatch=="")
     {
	origin.sendMessage("Usage: nlist <match> <optinal destination>",getName());
	return;
     }
   int nbRes = services->getDatabase().dbSelect("nickname,lasthost,email", "nicks", "lasthost like '"+tomatch+"'");
   int f=0;
   for(int i=0; i<nbRes; i++)
     {
	f++;
	String tnick = services->getDatabase().dbGetValue(0);
	String thost = services->getDatabase().dbGetValue(1);
	String temail = services->getDatabase().dbGetValue(2);
	String tosend = String("\002")+tnick+"\002 with last address \002"+thost+"\002 "+temail+"\002";
        services->getDatabase().dbGetRow();
	if(dest=="")
	  {
	     origin.sendMessage(String(tosend),getName());
	  }
	else
	  {
	     services->serviceNotice(String(tosend),"Serv",dest);
	  }
     }
   services->log(origin,"Serv","Did a nlist on "+tomatch+" "+String::convert(f)+" matches found");
   String togo = origin.getNickname()+" did a \002nlist\002 on "+tomatch+" "+String::convert(f)+" matches found";
   services->logLine(togo);
}


  SERV_FUNC (Module::parseELIST)
{
   String tomatch = tokens.nextToken();
   String dest = tokens.nextToken();
   if(tomatch=="")
     {
	origin.sendMessage("Usage: elist <match> <optional destination>",getName());
	return;
     }
   if(dest=="")
     {
	//Return to origin.
        int nbRes = services->getDatabase().dbSelect("nickname, lasthost, email", "nicks", "email like '"+tomatch+"'");
	for(int i=0; i<nbRes; i++)
	  {
	     String nickname = services->getDatabase().dbGetValue(0);
	     String lasthost = services->getDatabase().dbGetValue(1);
	     String email = services->getDatabase().dbGetValue(2);
	     String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
	     origin.sendMessage(tosend,getName());
             services->getDatabase().dbGetRow();
	  }
	services->log(origin,"Serv","Did an elist on "+tomatch);
	String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch;
	services->logLine(togo);
	return;
     }
   //Else send to given client
   /* This should proberly be wrapped up into one function, just checking
    * for the correct place to send .. like above *points up* */
   int nbRes = services->getDatabase().dbSelect("nickname, lasthost, email", "nicks", "email like '"+tomatch+"'");

   for(int i=0; i<nbRes; i++)
     {
        String nickname = services->getDatabase().dbGetValue(0);
        String lasthost = services->getDatabase().dbGetValue(1);
        String email = services->getDatabase().dbGetValue(2);
	String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
	services->serviceNotice(tosend,getName(),dest);
     }
   services->log(origin,"Serv","Did an elist on "+tomatch+" and sent it to "+dest);
   String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch+" and sent the results to "+dest;
   services->logLine(togo);

}


  SERV_FUNC (Module::parseDELNICK)
{
   String who  = tokens.nextToken();
   String reason = tokens.rest();
   if(who=="" || reason=="")
     {
	origin.sendMessage("Usage: delnick nickname reason",getName());
	return;
     }
   if(!services->isNickRegistered(who))
     {
	origin.sendMessage("Error: Nickname is not registered",getName());
	return;
     }

   String togo = origin.getNickname()+" did \002delnick\002 on "+who+" for \002"+reason;
   services->logLine(togo, Log::Warning);
   services->getDatabase().dbDelete("nicks", "nickname='"+who+"'");
   services->log(origin,"Serv","Deleted nickname "+who+" : "+reason);
}


  SERV_FUNC (Module::parseCLIST)
{
   String who = tokens.nextToken();
   String send = tokens.nextToken();
   if(who=="")
     {
	origin.sendMessage("Usage: clist nickname <optional nick to send to>",getName());
	return;
     }

   if(!services->isNickRegistered(who))
     {
	origin.sendMessage("That nickname is not registered",getName());
	return;
     }
   if(send=="")
     {
	int totalc = services->getChannel().maxChannels();
	int userc = services->getChannel().maxChannelsUser(who);
	int totala = services->getChannel().maxChannelsAccess();
	String togo = origin.getNickname() + " did a \002clist\002 on "+who+", "+String::convert(userc)+" matches found from "+String::convert(totalc)+" channels and "+String::convert(totala)+" access entries";
	services->logLine(togo);
	int theid = services->getRegisteredNickID(who);
        int nbRes = services->getDatabase().dbSelect("chanid,access", "chanaccess", "nickid='"+String::convert(theid)+"'");

	for(int i=0; i<nbRes; i++)
	  {
	     String cname = services->getDatabase().dbGetValue(0);
	     String caxs = services->getDatabase().dbGetValue(1);
	     String ccname = services->getChannel().getChanName(cname.toInt());
	     String tosend = ccname+" with "+caxs;
	     origin.sendMessage(tosend,getName());
             services->getDatabase().dbGetRow();
	  }
	services->log(origin,"Serv","Did a clist on "+who);
	return;
     }
}


EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }


// Module information structure
const Module::moduleInfo_type Module::moduleInfo =
{
   "Service Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::NONE
};


// Start the service
void Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;
   
   // Attempt to affirm our database table..
   if (!services->getDatabase().affirmTable(Tables::serverlistTable)) {
      services->logLine("Unable to affirm mod_serv database table "
			"'serverlist'",
			Log::Fatality);
      return; // How do we tell services we did not start happily?!
   }
   
   // Register ourself to the network
   services->registerService(getName(), getName(),
			     getConfigData().getHostname(), "+dz",
			     getConfigData().getDescription());
}
