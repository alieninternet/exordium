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
#include <exordium/utils.h>
#include <kineircd/str.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::ServModule;

const Module::functionTableStruct Module::functionTable[] =
{
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
     { "status",	&Module::parseSTATUS },
     { "commands",	&Module::parseCOMMANDS },
     { "gethash",	&Module::parseGETHASH },
     { "freeze",	&Module::parseFREEZE },
     { 0, 0}
};

void Module::parseLine(StringTokens& line, User& origin, const bool safe)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	if (command == functionTable[i].command)
	  {
	     if(!origin.isIdentified(origin.getNickname()))
	       {
		  origin.sendMessage("You are not identified to services",getName());
		  services->sendGOper(origin.getNickname()+" tried to use \002"+command+"\002 when not identified",getName());
		  return;
	       }

	     int required = services->getRequiredAccess(getName(),command.toLower());
	     int access = origin.getAccess(getName());
	     if(required>access)
	       {
		  origin.sendMessage("You do not have enough access for that command",getName());
		  String togo = "\002 WARNING!\002 "+origin.getNickname()+" tried to use the \002"+command+"\002 on Serv";
		  services->logLine(togo, Log::Warning);
		  services->sendGOper(getName(),togo);
		  return;
	       }

	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage ("Unrecognised Command", getName());
}

SERV_FUNC (Module::parseSTATUS)
{
   origin.sendMessage("Exordium services status report",getName());
   String togo = "Current build : \002" + String::convert(Services::buildNumber) + "\002";
   origin.sendMessage(togo,getName());
   long tx = services->getCountTx();
   long rx = services->getCountRx();
   String tofo = "Tx : \002" + String::convert(tx) + "\002 Rx : \002" + String::convert(rx) + "\002";
   origin.sendMessage(tofo,getName());
   time_t start_day;
   time_t now_day;

   now_day = time ( NULL );
   String ntime = ctime( &now_day );
   //start_day = time ( services->startTime);
   //String stime = ctime ( &start_day);
   origin.sendMessage("Current Time : \002"+ntime,getName());
}

SERV_FUNC (Module::parseCOMMANDS)
{
   String::size_type lineLength = 200;
   origin.sendMessage("Command list for " + getName() + ":",getName());
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++)
     {
        list << " " << functionTable[i].command;

	// How are we for size?
	if (list.str().length() >= lineLength)
	  {
	     // Dump it and reset the string stream thingy
	     origin.sendMessage(list.str(),getName());
	     list.str() = " -=>";
	  }
     }

   // Anything left to send still?
   if (list.str().length() > 4)
     {
	origin.sendMessage(list.str(),getName());
     }
   // Send the footer (this shouldn't be hard-coded)
   origin.sendMessage("End of command list",getName());

}

SERV_FUNC (Module::parseDIE)
{
   String reason = tokens.rest();
   if(reason=="")
     {
	origin.sendMessage("Usage: die reason for shutdown here",getName());
	return;
     }
   String tofo = origin.getNickname()+" issued an immediate services \002shutdown\002 for \002"+reason;

   services->sendGOper(getName(),tofo);
   String togo = "\002"+origin.getNickname()+"\002 - "+reason;
   services->shutdown(togo);
}
SERV_FUNC (Module::parseFREEZE)
{
   String func = tokens.nextToken();
   if(func=="")
     {
	origin.sendMessage("Usage: freeze add/del/list",getName());
	return;
     }
   if(func=="add")
     {
	String chan = tokens.nextToken();
	String reason = tokens.rest();
	if(chan=="" | reason=="")
	  {
	     origin.sendMessage("Usage: freeze add #channel reason",getName());
	     return;
	  }
        if(!services->getChannel().isChanRegistered(chan))
	  {
	     origin.sendMessage("Error: That channel is not registered",getName());
	     return;
	  }
	int times = services->timesFreezed(chan);
	std::cout << "Counter returned " << times << std::endl;
	int expires = 0;
	String flong;
	if(times==0)
	  {
	     expires = time(NULL)+3600;
	     flong = "1 hour";
	  }
	if(times==1)
	  {
	     expires = time(NULL)+7200;
	     flong = "2 hours";
	  }

	if(times==2)
	  {
	     flong = "12 hours";
	     expires = time(NULL)+43200;
	  }

	if(times==3)
	  {
	     flong = "1 day";
	     expires = time(NULL)+86400;
	  }

	if(times==4)
	  {
	     flong = "2 days";
	     expires = time(NULL)+172800;
	  }

	if(times==5)
	  {
	     flong = "7 days";
	     expires = time(NULL)+604800;
	  }

	if(times==6)
	  {
	     origin.sendMessage("Error: That channel has already been frozen 6 times",getName());
	     return;
	  }

	services->addFreeze(chan,origin.getNickname(),expires,reason);
	int cid = services->getChannel().getChanID(chan.IRCtoLower());
	int coid = services->getChannel().getOnlineChanID(chan);
	int nbRes = services->getDatabase().dbSelect("nickid","chanstatus","chanid="+String::convert(coid)+" AND status=2");
	CResult *myRes = services->getDatabase().dbGetResultSet();
	std::cout << "entering loop with " << nbRes << std::endl;
	for(int i=0;i<nbRes;i++)
	  {
	     String inick = services->getOnlineNick(myRes->getValue(i,0).toInt());
	     std::cout << inick << std::endl;
	     services->mode(getName(),chan,"-o",inick);
	  }
	delete myRes;
	int mbRes = services->getDatabase().dbSelect("nickid","chanstatus","chanid="+String::convert(coid)+" AND status=1");
	if(mbRes!=0)
	  {

	     CResult *mvRes = services->getDatabase().dbGetResultSet();
	     for(int j=0;j<mbRes;j++)
	       {
		  String inick = services->getOnlineNick(mvRes->getValue(j,0).toInt());
		  services->mode(getName(),chan,"-v",inick);
	       }
	     delete mvRes;
	  }

	services->mode(getName(),chan,"+ntm-ilsl","");
	String topic = "This channel has been frozen by "+origin.getNickname()+" because \002"+reason;
	services->getChannel().setTopic(chan,topic);
	services->sendGOper(getName(),origin.getNickname()+" set a \002freeze\002 on "+chan+" for \002"+reason+"\002 ("+flong+")");
	return;

     }
   if(func=="del")
     {
	String channel = tokens.nextToken();
	if(!services->isFreezed(channel))
	  {
	     origin.sendMessage("Error: That channel is not frozen",getName());
	     return;
	  }

	String topic = "This channel has been unfrozen by "+origin.getNickname();
	services->getChannel().synchChannel(channel,topic,"+nt-m");
	services->delFreeze(channel);
     }
   if(func=="list")
     {
	String channel = tokens.nextToken();
	if(channel=="")
	  {
	     origin.sendMessage("Usage: freeze list #channel",getName());
	     return;
	  }

	/* Lists all current freezes*/
	int cid = services->getChannel().getChanID(channel.IRCtoLower());
	int nbRes = services->getDatabase().dbSelect("*","chanfreeze","name='"+String::convert(cid)+"'");
	CResult *myRes = services->getDatabase().dbGetResultSet();
	if(nbRes==0)
	  {
	     origin.sendMessage("No freeze's found for "+channel,getName());
	     return;
	  }
	origin.sendMessage("Freeze history for channel "+channel,getName());

	for(int i=0;i<nbRes;i++)
	  {
	     String id = myRes->getValue(i,0);
	     String setby = myRes->getValue(i,2);
	     String seton = myRes->getValue(i,3);
	     String expires = myRes->getValue(i,4);
	     String reason = myRes->getValue(i,5);
	     origin.sendMessage("\002["+id+"]\002 Setby: \002"+setby+"\002 SetOn: \002"+seton+"\002 Expires: \002"+expires+"\002 Reason: \002"+reason,getName());
	  }
	delete myRes;

     }

}

SERV_FUNC (Module::parseGETHASH)
{
   String who = tokens.nextToken();
   if(who=="")
     {
	origin.sendMessage("Usage: gethash nickname",getName());
	return;
     }
   if(!services->isNickRegistered(who))
     {
	origin.sendMessage("Error: Nickname is not registered",getName());
	return;
     }
   if(!services->getDatabase().dbSelect("id","nickspending","nickname='"+who+"'"))
     {
	origin.sendMessage("Error: That nickname is not in a pending status",getName());
	return;
     }
   int nbRes = services->getDatabase().dbSelect("auth","nickspending","nickname='"+who+"'");
   String auth = services->getDatabase().dbGetValue();
   String togo = "The authorisation code for the nickname \002"+who+"\002 is \002"+auth;
   origin.sendMessage(togo,getName());
   services->sendGOper(getName(),origin.getNickname()+" did a \002gethash\002 on "+who);
   return;
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
   if((services->getAccess(getName(),who)>0))
     {
	/* Ok staff nickname, unless the user is a level 500, reject it. */
	if(services->getAccess(getName(),origin.getNickname())<500)
	  {
	     origin.sendMessage("Error: You may not perform that command on staff",getName());
	     services->sendGOper(getName(),"\002Warning\002 "+origin.getNickname()+" tried to perform a \002setpass\002 on a staff nickname ("+who+")");
	     return;
	  }

     }

   String epass =  Utils::generatePassword(who, newpass);
   services->getDatabase().dbUpdate("nicks", "password='"+epass+"'", "nickname='"+who+"'");
   String togo = "\002"+origin.getNickname()+"\002 changed password for nickname "+who+" to [HIDDEN]";
   services->logLine(togo);
   services->sendGOper(getName(),togo);
}

SERV_FUNC (Module::parseRAW)
{
   std::string c = tokens.rest();
   services->queueAdd(c);
   String togo = origin.getNickname()+" did a services \002raw\002 - "+c;
   services->logLine(String(togo), Log::Warning);
   services->sendGOper(getName(),togo);
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

	if(nbRes==0)
	  {
	     origin.sendMessage("No News found",getName());
	     return;
	  }
	CResult *myRes = services->getDatabase().dbGetResultSet();

	for(int i=0; i<nbRes; i++)
	  {
	     String id = myRes->getValue(i,0);
	     String level = myRes->getValue(i,1);
	     String expires = myRes->getValue(i,2);
	     String txt = myRes->getValue(i,3);
	     String togo = "ID \002[\002"+id+"\002]\002 Level \002[\002"+level+"\002]\002 Expires \002[\002"+expires+"\002]\002 Text \002[\002"+txt+"\002]\002";
	     origin.sendMessage(togo,getName());
	  }
	delete myRes;

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
	services->sendGOper(getName(),origin.getNickname()+" \002deleted\002 news item "+id);
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
	if(type.toInt()!=0 || type.toInt()!=1)
	  {
	     origin.sendMessage("Error: Type must be either 0 (all users) or 1 (staff)",getName());
	     return;
	  }
	if(expires.toInt()<services->currentTime)
	  {
	     origin.sendMessage("Error: The expire time cannot be before now",getName());
	     return;
	  }
	if(expires.toInt()>services->currentTime+ 36000)
	  {
	     origin.sendMessage("Error: Expire time is to far in the future",getName());
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
	services->sendGOper(getName(),origin.getNickname() + "\002Added\002 a new news item");
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
	if(!services->getChannel().isChanRegistered(channel))
	  {
	     origin.sendMessage("Error: That channel is not registered",getName());
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
	services->sendGOper(getName(),origin.getNickname()+" \002Modified\002 channel ownedship of "+channel+" "+oldowner+"->"+newowner);
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
	if(!services->getChannel().isChanRegistered(channel))
	  {
	     origin.sendMessage("That channel is not registered",getName());
	     return;
	  }

	String togo = origin.getNickname() + "\002 de-registered\002 "+channel+" for \002"+reason+"\002";
	services->logLine(String(togo));
	services->getChannel().deregisterChannel(channel);

        togo = "This channel has been deregistered \002"+reason;
        services->serviceNotice((String)togo,getName(),channel);
        origin.sendMessage("Channel " + channel + " dropped successfully",getName());

	services->log(origin,getName(),String("Deregistered ")+channel+" for "+reason);
	services->sendGOper(getName(),origin.getNickname()+" \002Deregistered\002 channel "+channel+" for "+reason);
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
	services->sendGOper(getName(),origin.getNickname()+" \002Registered\002 "+channel+" to "+thenick);
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
	if(ilevel<1 || ilevel>499)
	  {
	     origin.sendMessage("Error: Level must be between 1 and 499",getName());
	     return;
	  }

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
	String togo = origin.getNickname() + " modified access for \002"+toadd+"\002 "+String::convert(taccess)+"->"+String::convert(ilevel);
	services->logLine(togo);
	services->sendGOper(getName(),togo);
	services->getDatabase().dbUpdate("access", "access='"+String::convert(level)+"'", "nickname='"+toadd+"'");
	services->log(origin,getName(),String("Modified access for ")+toadd+" from "+String::convert(taccess)+"->"+String::convert(level));
	return;
     }
   if(command=="list")
     {
	int nbRes = services->getDatabase().dbSelect("*", "access", "service='serv'");
	CResult *myRes = services->getDatabase().dbGetResultSet();

	for(int i=0; i<nbRes; i++)
	  {
	     String nickname = myRes->getValue(i,1);
	     String access = myRes->getValue(i,3);
	     String togo = "\002"+nickname+"\002 has level \002"+access;
	     origin.sendMessage(String(togo),getName());
	  }
	delete myRes;
	return;
     }
   if(command=="del")
     {
	if(toadd=="")
	  {
	     origin.sendMessage("Usage is user del nickname",getName());
	     return;
	  }
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
	services->sendGOper(getName(),togo);
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
	services->sendGOper(getName(),togo);
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
	origin.sendMessage("You are now a PeopleChat services assistant",getName());
	String tosend = ":services.peoplechat.org SVSMODE "+origin.getNickname()+" +gsao";
	services->queueAdd(String(tosend));
	services->log(origin,getName(),origin.getNickname()+ " became a services helper at level "+String::convert(access));
	services->sendGOper(getName(),origin.getNickname() + " became a services helper at level "+String::convert(access));

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
   CResult *myRes = services->getDatabase().dbGetResultSet();
   int f=0;
   if(dest!="")
     {
	User *tmp = services->findUser(dest);
	if(tmp==0)
	  {
	     origin.sendMessage("Error: That user (%p0) is not online",getName());
	     return;
	  }
	delete tmp;
     }

   for(int i=0; i<nbRes; i++)
     {
	f++;
	String tnick = myRes->getValue(i,0);
	String thost = myRes->getValue(i,1);
	String temail = myRes->getValue(i,2);
	if(thost.length()<2)
	  thost = "No Recorded Host (Never Identified)";
	String tosend = "\002["+String::convert(i)+"]\002 Nickname: \002"+tnick+"\002 Last ID'd Host: \002"+thost+"\002 Email Address: \002"+temail+"\002";
	if(f>30)
	  {
	     origin.sendMessage("More than 30 results found! Please be more specific in your search",getName());
	     return;
	  }

	if(dest=="")
	  {
	     origin.sendMessage(String(tosend),getName());
	  }
	else
	  {
	     services->serviceNotice(String(tosend),"Serv",dest);
	  }
     }
   if(nbRes==0)
     {
	origin.sendMessage("No Results found!",getName());
     }

   delete myRes;
   services->log(origin,"Serv","Did a nlist on "+tomatch+" "+String::convert(f)+" matches found");
   String togo;
   if(dest=="")
     {
	togo = origin.getNickname()+" did a \002nlist\002 on "+
	  tomatch+" "+String::convert(f)+" matches found";
     }
   else
     {
	togo = origin.getNickname()+" did a \002nlist\002 on "+tomatch+" "+
	  String::convert(f)+" matches found and sent it to "+dest;
     }

   services->logLine(togo);
   services->sendGOper(getName(),togo);
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
   int nbRes = services->getDatabase().dbSelect("nickname, lasthost, email", "nicks", "email like '"+tomatch+"'");
   CResult *myRes = services->getDatabase().dbGetResultSet();
   if(dest!="")
     {
	User *tmp = services->findUser(dest);
	if(tmp==0)
	  {
	     origin.sendMessage("Error: That user (%p0) is not online",getName());
	     return;
	  }
	delete tmp;
     }

   for(int i=0; i<nbRes; i++)
     {
	String nickname = myRes->getValue(i,0);
	String lasthost = myRes->getValue(i,1);
	String email = myRes->getValue(i,2);
	String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
	if(i>30)
	  {
	     origin.sendMessage("More than 30 results found! Please be more specific",getName());
	     return;
	  }

	if(dest=="")
	  origin.sendMessage(tosend,getName());
	else
	  services->serviceNotice(String(tosend),"Serv",dest);

	services->getDatabase().dbGetRow();
     }
   if(nbRes==0)
     {
	origin.sendMessage("No Results found!",getName());
     }

   if(dest=="")
     {
	services->log(origin,"Serv","Did an elist on "+tomatch);
	String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch;
	services->logLine(togo);
	services->sendGOper(getName(),togo);
     }
   else
     {

	services->log(origin,"Serv","Did an elist on "+tomatch+" and sent it to "+dest);
	String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch+" and sent the results to "+dest;
	services->logLine(togo);
	services->sendGOper(getName(),togo);
     }
   delete myRes;

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
   /* Are we trying to delete a staff nickname? */
   if(services->getAccess(getName(),who)>0)
     {
	/* Yep... Unless a 500 tell em to get stuffed */
	if(services->getAccess(getName(),origin.getNickname())<500)
	  {
	     origin.sendMessage("Error: You may not perform that command on a staff nick",getName());
	     services->sendGOper(getName(),origin.getNickname()+" tried to perform a \002delnick\002 on a staff nickname ("+who+")");
	     return;
	  }

     }

   String togo = origin.getNickname()+" did \002delnick\002 on "+who+" for \002"+reason;
   services->logLine(togo, Log::Warning);
   services->sendGOper(getName(),togo);
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
   if(send!="")
     {

	User *ptr = services->findUser(send);
	if(ptr==0)
	  {
	     origin.sendMessage("That nickname is not online",getName());
	     return;
	  }

     }

   int totalc = services->getChannel().maxChannels();
   int userc = services->getChannel().maxChannelsUser(who);
   int totala = services->getChannel().maxChannelsAccess();
   int theid = services->getRegisteredNickID(who);
   int nbRes = services->getDatabase().dbSelect("chanid,access", "chanaccess", "nickid='"+String::convert(theid)+"'");
   String togo = origin.getNickname() + " did a \002clist\002 on "+who+", "
     +String::convert(nbRes)+" matches found from "+String::convert(totalc)+
     " channels and "+String::convert(totala)+" access entries";
   if(send!="")
     togo = togo + " and sent it to "+send;
   services->sendGOper(getName(),togo);

   CResult *myRes = services->getDatabase().dbGetResultSet();
   if(send=="")
     origin.sendMessage("Channel Access list for "+who,getName());
   else
     {
	origin.sendMessage("Sending channel access list for "+who+" to "+send,getName());
	services->serviceNotice("Channel access list for "+who+" sent by "+origin.getNickname(),getName(),send);
     }

   for(int i=0; i<nbRes; i++)
     {
	String cname = myRes->getValue(i,0);
	String caxs = myRes->getValue(i,1);
	String ccname = services->getChannel().getChanName(cname.toInt());
	String tosend = "Channel: \002"+ccname+"\002 with level \002"+caxs;
	if(send=="")

	  origin.sendMessage(tosend,getName());
	else
	  services->serviceNotice(String(tosend),"Serv",send);
	services->getDatabase().dbGetRow();
     }
   if(nbRes==0)
     origin.sendMessage("No Results Found!",getName());
   services->log(origin,"Serv","Did a clist on "+who);
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
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;

   // Attempt to affirm our database table..
   if (!services->getDatabase().affirmTable(Tables::serverlistTable))
     {
	services->logLine("Unable to affirm mod_serv database table "
			  "'serverlist'",
			  Log::Fatality);
	return false;
     }

   // Register ourself to the network
   services->registerService(getName(), getIdent(),
			     getConfigData().getHostname(),
			     getConfigData().getDescription());

   // We started okay :)
   return true;
}
