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

#include <mod.serv/serv.h>
#include <mod.serv/tables.h>
#include <mod.serv/language.h>

#include <exordium/services.h>
#include <exordium/channel.h>
#include <exordium/utils.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::ServModule;

const Service::functionTableStruct Service::functionTable[] =
{
     { "clist",		&Service::parseCLIST },
     { "delnick",	&Service::parseDELNICK },
     { "elist",		&Service::parseELIST },
     { "nlist",		&Service::parseNLIST },
     { "helpon",	&Service::parseHELPON },
     { "help",		&Service::parseHELP },
     { "user",		&Service::parseUSER },
     { "raw",		&Service::parseRAW },
     { "chan",		&Service::parseCHAN },
     { "die",		&Service::parseDIE },
     { "news",		&Service::parseNEWS },
     { "setpass",	&Service::parseSETPASS },
     { "status",	&Service::parseSTATUS },
     { "commands",	&Service::parseCOMMANDS },
     { "gethash",	&Service::parseGETHASH },
     { "freeze",	&Service::parseFREEZE },
     { 0, 0}
};

void Service::parseLine(StringTokens& line, User& origin, const bool safe)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	if (command == functionTable[i].command)
	  {
	     if(!origin.isIdentified(origin.getNickname()))
	       {
		  origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),getNickname());
		  services.sendGOper(origin.getNickname()+" tried to use \002"+command+"\002 when not identified",getNickname());
		  return;
	       }

	     int required = services.getRequiredAccess(getNickname(),command.toLower());
	     int access = origin.getAccess(getNickname());
	     if(required>access)
	       {
		  origin.sendMessage(GETLANG(serv_NOT_ENOUGH_ACCESS),getNickname());
		  String togo = "\002 WARNING!\002 "+origin.getNickname()+" tried to use the \002"+command+"\002 on Serv";
		  services.logLine(togo, Log::Warning);
		  services.sendGOper(getNickname(),togo);
		  return;
	       }

	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage (GETLANG(ERROR_UNKNOWN_COMMAND,command), getNickname());
}

SERV_FUNC (Service::parseSTATUS)
{
   origin.sendMessage(GETLANG(serv_STATUS_REPORT_START),getNickname());
   //origin.sendMessage(GETLANG(serv_STATUS_CURRENT_BUILD,String::convert(Services::buildNumber)),getNickname());
//   long tx = services.getCountTx();
//   long rx = services.getCountRx();
//   origin.sendMessage(GETLANG(serv_STATUS_TXRX,
//			      String::convert(tx),
//			      String::convert(rx)),
//		      getNickname());
   time_t now_time = time(NULL);
   origin.sendMessage(GETLANG(serv_STATUS_CURR_TIME,ctime(&now_time)),getNickname());
   time_t start_day;
   start_day = services.getStartTime();
   origin.sendMessage(GETLANG(serv_STATUS_START_TIME,ctime(&start_day)),getNickname());
   int uptime = time(NULL)-start_day;
   origin.sendMessage(GETLANG(serv_STATUS_UP_TIME,String::convert((uptime/86400)),String::convert((uptime/3600)%24),String::convert((uptime/60)%60),String::convert((uptime%60))),getNickname());

}

SERV_FUNC (Service::parseCOMMANDS)
{
   String::size_type lineLength = 200;
   origin.sendMessage(GETLANG(COMMAND_LIST_START,getNickname()),getNickname());
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	list << " " << functionTable[i].command;

	// How are we for size?
	if (list.str().length() >= lineLength)
	  {
	     // Dump it and reset the string stream thingy
	     origin.sendMessage(list.str(),getNickname());
	     list.str() = " -=>";
	  }
     }

   // Anything left to send still?
   if (list.str().length() > 4)
     {
	origin.sendMessage(list.str(),getNickname());
     }
   // Send the footer (this shouldn't be hard-coded)
   origin.sendMessage(GETLANG(COMMAND_LIST_END),getNickname());

}

SERV_FUNC (Service::parseDIE)
{
   String reason = tokens.rest();
   if(reason=="")
     {
	origin.sendMessage(GETLANG(serv_DIE_USAGE),getNickname());
	return;
     }
   origin.sendMessage(GETLANG(serv_DIE_SUCCESS),getNickname());
   services.sendGOper(getNickname(),origin.getNickname()+" issued an immediate services \002shutdown\002 for \002"+reason);
//   services.shutdown("\002"+origin.getNickname()+"\002 - "+reason);
}
SERV_FUNC (Service::parseFREEZE)
{
   String func = tokens.nextToken();
   if(func=="")
     {
	origin.sendMessage(GETLANG(serv_FREEZE_USAGE),getNickname());
	return;
     }
   if(func=="add")
     {
	Kine::ChannelName chan = tokens.nextToken();
	String reason = tokens.rest();
	if(chan=="" | reason=="")
	  {
	     origin.sendMessage(GETLANG(serv_FREEZE_ADD_USAGE),getNickname());
	     return;
	  }
	if(!services.getChannel().isChanRegistered(chan))
	  {
	     origin.sendMessage(GETLANG(serv_FREEZE_NOT_REG),getNickname());
	     return;
	  }
	int times = timesFreezed(chan);
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
	     origin.sendMessage(GETLANG(serv_FREEZE_MAX_ALREADY),getNickname());
	     return;
	  }

	addFreeze(chan,origin.getNickname(),expires,reason);
	int cid = services.getChannel().getChanID(chan.IRCtoLower());
	int coid = services.getChannel().getOnlineChanID(chan);
	int nbRes = services.getDatabase().dbSelect("nickid","chanstatus","chanid="+String::convert(coid)+" AND status=2");
	CResult *myRes = services.getDatabase().dbGetResultSet();
	for(int i=0;i<nbRes;i++)
	  {
	     String inick = services.getOnlineNick(myRes->getValue(i,0).toInt());
	     std::cout << inick << std::endl;
	     services.mode(getNickname(),chan,"-o",inick);
	  }
	delete myRes;
	int mbRes = services.getDatabase().dbSelect("nickid","chanstatus","chanid="+String::convert(coid)+" AND status=1");
	if(mbRes!=0)
	  {

	     CResult *mvRes = services.getDatabase().dbGetResultSet();
	     for(int j=0;j<mbRes;j++)
	       {
		  String inick = services.getOnlineNick(mvRes->getValue(j,0).toInt());
		  services.mode(getNickname(),chan,"-v",inick);
	       }
	     delete mvRes;
	  }
	services.mode(getNickname(),chan,"+ntm-ilsl","");
	String topic = "This channel has been frozen by "+origin.getNickname()+" because \002"+reason;
	services.getChannel().setTopic(chan,topic);
	services.sendGOper(getNickname(),origin.getNickname()+" set a \002freeze\002 on "+chan+" for \002"+reason+"\002 ("+flong+")");
	return;

     }
   if(func=="del")
     {
	String channel = tokens.nextToken();
	if(channel.empty())
	  {
	     origin.sendMessage(GETLANG(serv_FREEZE_DEL_USAGE),getNickname());
	     return;
	  }

	if(!isFreezed(channel))
	  {
	     origin.sendMessage(GETLANG(serv_FREEZE_NOT_FREEZED),getNickname());
	     return;
	  }

	String topic = "This channel has been unfrozen by "+origin.getNickname();
	services.getChannel().synchChannel(channel,topic,"+nt-m");
	delFreeze(channel);
     }
   if(func=="list")
     {
	Kine::ChannelName channel = tokens.nextToken();
	if(channel=="")
	  {
	     origin.sendMessage(GETLANG(serv_FREEZE_LIST_USAGE),getNickname());
	     return;
	  }

	/* Lists all current freezes*/
	int cid = services.getChannel().getChanID(channel.IRCtoLower());
	int nbRes = services.getDatabase().dbSelect("*","chanfreeze","name='"+String::convert(cid)+"'");
	CResult *myRes = services.getDatabase().dbGetResultSet();
	if(nbRes==0)
	  {
	     origin.sendMessage(GETLANG(serv_FREEZE_LIST_NONFOUND,channel),getNickname());
	     return;
	  }
	origin.sendMessage(GETLANG(serv_FREEZE_LIST_HISTORY,channel),getNickname());

	for(int i=0;i<nbRes;i++)
	  {
	     String id = myRes->getValue(i,0);
	     String setby = myRes->getValue(i,2);
	     String seton = myRes->getValue(i,3);
	     String expires = myRes->getValue(i,4);
	     String reason = myRes->getValue(i,5);
	     origin.sendMessage("\002["+id+"]\002 Setby: \002"+setby+"\002 SetOn: \002"+seton+"\002 Expires: \002"+expires+"\002 Reason: \002"+reason,getNickname());
	  }
	delete myRes;

     }

}

SERV_FUNC (Service::parseGETHASH)
{
   Kine::ClientName who = tokens.nextToken();
   if(who=="")
     {
	origin.sendMessage(GETLANG(serv_GETHASH_USAGE),getNickname());
	return;
     }
   if(!services.isNickRegistered(who))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   if(!services.getDatabase().dbSelect("id","nickspending","nickname='"+who+"'"))
     {
	origin.sendMessage(GETLANG(serv_GETHASH_NOT_PENDING),getNickname());
	return;
     }
   int nbRes = services.getDatabase().dbSelect("auth","nickspending","nickname='"+who+"'");
   String auth = services.getDatabase().dbGetValue();
   String togo = GETLANG(serv_GETHASH_IS,who,auth);
   origin.sendMessage(togo,getNickname());
   services.sendGOper(getNickname(),origin.getNickname()+" did a \002gethash\002 on "+who);
   return;
}

SERV_FUNC (Service::parseSETPASS)
{
   String who = tokens.nextToken();
   String newpass = tokens.nextToken();
   if(who=="" | newpass=="")
     {
	origin.sendMessage(GETLANG(serv_SETPASS_USAGE),getNickname());
	return;
     }
   if(!services.isNickRegistered(who))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   if((services.getStatic().getServiceAccess(getNickname(),who)>0))
     {
	/* Ok staff nickname, unless the user is a level 500, reject it. */
	if(services.getStatic().getServiceAccess(getNickname(),origin.getNickname())<500)
	  {
	     origin.sendMessage(GETLANG(serv_NOT_ON_STAFF),getNickname());
	     services.sendGOper(getNickname(),"\002Warning\002 "+origin.getNickname()+" tried to perform a \002setpass\002 on a staff nickname ("+who+")");
	     return;
	  }

     }

   String epass =  Utils::generatePassword(who, newpass);
   services.getDatabase().dbUpdate("nicks", "password='"+epass+"'", "nickname='"+who+"'");
   String togo = "\002"+origin.getNickname()+"\002 changed password for nickname "+who+" to [HIDDEN]";
   services.logLine(togo);
   services.sendGOper(getNickname(),togo);
   origin.sendMessage(GETLANG(serv_SETPASS_SUCCESS,who,epass),getNickname());

}

SERV_FUNC (Service::parseRAW)
{
   std::string c = tokens.rest();
   if(c=="")
     {
	origin.sendMessage(GETLANG(serv_RAW_USAGE),getNickname());
	return;
     }
//   services.queueAdd(c);
   String togo = origin.getNickname()+" did a services \002raw\002 - "+c;
   services.logLine(String(togo), Log::Warning);
   services.sendGOper(getNickname(),togo);
   origin.sendMessage(GETLANG(serv_RAW_SUCCESS),getNickname());
}

SERV_FUNC (Service::parseNEWS)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage(GETLANG(serv_NEWS_USAGE),getNickname());
	return;
     }
   if(command=="list")
     {
	int nbRes = services.getDatabase().dbSelect("news");

	if(nbRes==0)
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_LIST_NO),getNickname());
	     return;
	  }
	CResult *myRes = services.getDatabase().dbGetResultSet();

	for(int i=0; i<nbRes; i++)
	  {
	     String id = myRes->getValue(i,0);
	     String level = myRes->getValue(i,1);
	     String expires = myRes->getValue(i,2);
	     String txt = myRes->getValue(i,3);
	     String togo = "ID \002[\002"+id+"\002]\002 Level \002[\002"+level+"\002]\002 Expires \002[\002"+expires+"\002]\002 Text \002[\002"+txt+"\002]\002";
	     origin.sendMessage(togo,getNickname());
	  }
	delete myRes;

     }
   if(command=="del")
     {
	String id = tokens.nextToken();
	if(id.empty())
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_DEL_USAGE),getNickname());
	     return;
	  }
	services.getDatabase().dbDelete("news", "id="+id);
	origin.sendMessage(GETLANG(serv_NEWS_DEL_SUCCESS,id),getNickname());
	services.sendGOper(getNickname(),origin.getNickname()+" \002deleted\002 news item "+id);
	return;
     }

   if(command=="add")
     {
	String type = tokens.nextToken();
	String expires = tokens.nextToken();
	String text = tokens.rest();
	if(type=="" | expires=="" | text=="")
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_ADD_USAGE),getNickname());
	     return;
	  }
	if(type.toInt()!=0 || type.toInt()!=1)
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_ADD_TYPE_USAGE),getNickname());
	     return;
	  }
	if(expires.toInt()<services.currentTime)
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_ADD_EXPIRE_IN_PAST),getNickname());
	     return;
	  }
	if(expires.toInt()>services.currentTime+ 36000)
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_ADD_EXPIRE_TIME_MACHINE_NEEDED),getNickname());
	     return;
	  }

	int nexpires = expires.toInt();
	nexpires = services.currentTime + (nexpires * 3600);
	if(services.currentTime>nexpires)
	  {
	     origin.sendMessage(GETLANG(serv_NEWS_ADD_EXPIRE_IN_PAST),getNickname());
	     return;
	  }
	services.getDatabase().dbInsert("news", "'','"+type+"','"+String::convert(nexpires)+"','"+text+"'");
	origin.sendMessage(GETLANG(serv_NEWS_ADD_SUCCESS),getNickname());
	services.sendGOper(getNickname(),origin.getNickname() + "\002Added\002 a new news item");
     }

}

SERV_FUNC (Service::parseCHAN)
{
   String command = tokens.nextToken();
   String channel = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage(GETLANG(serv_CHAN_USAGE),getNickname());
	return;
     }
   if(command=="mod")
     {
	String newowner = tokens.nextToken();
	if(channel=="" || newowner=="")
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_MOD_USAGE),getNickname());
	     return;
	  }
	if(!services.isNickRegistered(newowner))
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_MOD_OWNER_NOT_REGGED),getNickname());
	     return;
	  }
	if(!services.getChannel().isChanRegistered(channel))
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_MOD_NOT_REGGED),getNickname());
	     return;
	  }

	String newtopic = "This channel is now owned by "+newowner;
	services.getChannel().setTopic(channel,newtopic);
	services.getChannel().updateTopic(channel,newtopic);
	int chanid = services.getChannel().getChanID(channel);
	String oldowner = services.getChannel().getChanOwner(chanid);
	String togo = origin.getNickname()+" changed \002ownership\002 of "+channel+" "+oldowner+"->"+newowner;
	services.logLine(String(togo));
	services.getChannel().chanDelAccess(channel,oldowner);
	services.getChannel().chanAddAccess(channel,newowner,"500");
	services.getDatabase().dbUpdate("chans", "owner='"+newowner+"'", "name='"+channel+"'");
	services.log(origin,getNickname(),String("Changed ownership of ")+channel+" to "+newowner+" ("+oldowner+")");
	services.sendGOper(getNickname(),origin.getNickname()+" \002Modified\002 channel ownedship of "+channel+" "+oldowner+"->"+newowner);
	origin.sendMessage(GETLANG(serv_CHAN_MOD_SUCCESS,channel,newowner),getNickname());
	return;
     }
   if(command=="del")
     {
	String reason = tokens.rest();
	if(channel=="" || reason=="")
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_DEL_USAGE),getNickname());
	     return;
	  }
	if(!services.getChannel().isChanRegistered(channel))
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_MOD_NOT_REGGED),getNickname());
	     return;
	  }

	String togo = origin.getNickname() + "\002 de-registered\002 "+channel+" for \002"+reason+"\002";
	services.logLine(String(togo));
	services.getChannel().deregisterChannel(channel);
	togo = "This channel has been deregistered \002"+reason;
	services.serviceNotice((String)togo,getNickname(),channel);
	origin.sendMessage(GETLANG(serv_CHAN_DEL_SUCCESS,channel),getNickname());
	services.log(origin,getNickname(),String("Deregistered ")+channel+" for "+reason);
	services.sendGOper(getNickname(),origin.getNickname()+" \002Deregistered\002 channel "+channel+" for "+reason);
	return;
     }
   if(command=="add")
     {
	String thenick = tokens.nextToken();
	if(channel=="" || thenick=="")
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_ADD_USAGE),getNickname());
	     return;
	  }
	if(services.getChannel().isChanRegistered(channel))
	  {
	     origin.sendMessage(GETLANG(serv_CHAN_ADD_ALREADY_REG),getNickname());
	     return;
	  }
	if(!services.isNickRegistered(thenick))
	  {
	     origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),getNickname());
	     return;
	  }
	String togo = origin.getNickname() + "\002 registered\002 " + channel + " to "+thenick;
	services.logLine(String(togo));
	services.getChannel().registerChannel(channel,thenick);
	services.log(origin,getNickname(),String("Registered ")+channel+" to "+thenick);
	services.sendGOper(getNickname(),origin.getNickname()+" \002Registered\002 "+channel+" to "+thenick);
	origin.sendMessage(GETLANG(serv_CHAN_ADD_SUCCESS,channel,thenick),getNickname());

	return;
     }
}

SERV_FUNC (Service::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services.doHelp(origin,getNickname(),word,parm);
   String tolog = "Did HELP on word " + word + " parm " + parm;
   services.log(origin,getNickname(),String(tolog));
}

SERV_FUNC (Service::parseUSER)
{
   String command = tokens.nextToken();
   String toadd = tokens.nextToken();
   String level = tokens.nextToken();
   int access = origin.getAccess(getNickname());
   if(command=="")
     {
	origin.sendMessage(GETLANG(serv_USER_USAGE),getNickname());
	return;
     }
   if(command=="mod")
     {
	if(toadd=="" || level=="")
	  {
	     origin.sendMessage(GETLANG(serv_USER_MOD_USAGE),getNickname());
	     return;
	  }
	if(toadd==origin.getNickname())
	  {
	     origin.sendMessage(GETLANG(serv_USER_MOD_DUMB_USER),getNickname());
	     return;
	  }
	int ilevel = level.toInt();
	if(ilevel<1 || ilevel>499)
	  {
	     origin.sendMessage(GETLANG(serv_USER_MOD_LEVEL_MISMATCH),getNickname());
	     return;
	  }

	if(ilevel>access || ilevel==access)
	  {
	     origin.sendMessage(GETLANG(serv_USER_MOD_DUMBER_USER),getNickname());
	     return;
	  }
	int taccess = origin.getAccess(getNickname());
	if(taccess>access)
	  {
	     origin.sendMessage(GETLANG(serv_USER_MOD_SHOULD_YOU_BE_STAFF),getNickname());
	     String togo = origin.getNickname()+" tried to modify access for a higher user than themselves ("+toadd+")";
	     services.logLine(String(togo), Log::Warning);
	     return;
	  }
	if(taccess==access)
	  {
	     origin.sendMessage(GETLANG(serv_USER_MOD_DUMBER_USER),getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " modified access for \002"+toadd+"\002 "+String::convert(taccess)+"->"+String::convert(ilevel);
	services.logLine(togo);
	services.sendGOper(getNickname(),togo);
	services.getDatabase().dbUpdate("access", "access='"+String::convert(level)+"'", "nickname='"+String::convert(services.getStatic().getRegisteredNickID(toadd))+"'");
	services.log(origin,getNickname(),String("Modified access for ")+toadd+" from "+String::convert(taccess)+"->"+String::convert(level));
	origin.sendMessage(GETLANG(serv_USER_MOD_SUCCESS,toadd,String::convert(level)),getNickname());
	return;
     }
   if(command=="list")
     {
	int nbRes = services.getDatabase().dbSelect("*", "access", "service='serv'");
	CResult *myRes = services.getDatabase().dbGetResultSet();
	if(nbRes==0)
	  {
	     origin.sendMessage(GETLANG(serv_USER_LIST_NO_MATCHES),getNickname());
	     return;
	  }

	for(int i=0; i<nbRes; i++)
	  {
	     String nickname = services.getNick(myRes->getValue(i,1).toInt());
	     String access = myRes->getValue(i,3);
	     origin.sendMessage(GETLANG(serv_USER_LIST_MATCH,nickname,access),getNickname());
	  }
	delete myRes;
	return;
     }
   if(command=="del")
     {
	if(toadd=="")
	  {
	     origin.sendMessage(GETLANG(serv_USER_DEL_USAGE),getNickname());
	     return;
	  }
	String blah = getNickname();
	if(services.getStatic().getServiceAccess(blah,toadd)==0)
	  {
	     origin.sendMessage(GETLANG(serv_USER_DEL_NO_ACCESS),getNickname());
	     return;
	  }
	int faccess = services.getStatic().getServiceAccess(blah,toadd);
	if(faccess>access)
	  {
	     origin.sendMessage(GETLANG(serv_NOT_ON_STAFF),getNickname());
	     String togo = origin.getNickname() + " tried to use \002userdel\002 on a \002staff\002 nickname";
	     services.logLine(String(togo), Log::Warning);
	     return;
	  }
	services.getDatabase().dbDelete("access", "service='serv' AND nickname='" + String::convert(services.getStatic().getRegisteredNickID(toadd))+"'");
	origin.sendMessage(GETLANG(serv_USER_DEL_SUCCESS,toadd),getNickname());
	String togo = origin.getNickname() + " deleted \002 " + toadd + "\002 from Serv";
	services.logLine(String(togo), Log::Warning);
	services.sendGOper(getNickname(),togo);
	services.log(origin,getNickname(),"Deleted "+toadd+" from "+getNickname());
	return;
     }
   if(command=="add")
     {
	if(toadd=="" || level=="")
	  {
	     origin.sendMessage(GETLANG(serv_USER_ADD_USAGE),getNickname());
	     return;
	  }
	if(origin.getNickname().toLower()==toadd.toLower())
	  {
	     origin.sendMessage(GETLANG(serv_USER_ADD_TWIT),getNickname());
	     return;
	  }
	if(!services.isNickRegistered(toadd))
	  {
	     origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	     return;
	  }
	// Again temp.
	String foo = getNickname();
	if(services.getStatic().getServiceAccess(foo,toadd)>0)
	  {
	     origin.sendMessage(GETLANG(serv_USER_ADD_ALREADY_IN),getNickname());
	     return;
	  }
	if(level.toInt()<1 || level.toInt()>499)
	  {
	     origin.sendMessage(GETLANG(serv_USER_ADD_LEVEL_ERROR),getNickname());
	     return;
	  }
	services.getDatabase().dbInsert("access", "'','" + String::convert(services.getStatic().getRegisteredNickID(toadd)) + "','serv','" + level + "'");
	origin.sendMessage(GETLANG(serv_USER_ADD_SUCCESS,toadd,level),getNickname());
	String togo = origin.getNickname()+" added \002"+toadd+"\002 to Serv with level \002"+level;
	services.logLine(String(togo), Log::Warning);
	services.sendGOper(getNickname(),togo);
	String tolog = "Added "+toadd+" to Serv with level "+String::convert(level);
	services.log(origin,getNickname(),String(tolog));
	return;
     }
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND),getNickname());
   return;
}

SERV_FUNC (Service::parseHELPON)
{
   int access = origin.getAccess(getNickname());
   if(access>50)
     {
	origin.sendMessage(GETLANG(serv_HELPON_SUCCESS,String::convert(access)),getNickname());
	String tosend = ":services.peoplechat.org SVSMODE "+origin.getNickname()+" +gsao";
//	services.queueAdd(String(tosend));
	services.log(origin,getNickname(),origin.getNickname()+ " became a services helper at level "+String::convert(access));
	services.sendGOper(getNickname(),origin.getNickname() + " became a services helper at level "+String::convert(access));
	return;
     }
   services.log(origin,"Serv","Failed to become a helper (not enough access)");
   String tosend = origin.getNickname()+" failed to become a helper - Not enough access";
   services.logLine(tosend, Log::Warning);
   origin.sendMessage(GETLANG(serv_HELPON_FAILURE,String::convert(access)),getNickname());

}

SERV_FUNC (Service::parseNLIST)
{
   String tomatch = tokens.nextToken();
   Kine::ClientName dest = tokens.nextToken();
   if(tomatch=="")
     {
	origin.sendMessage(GETLANG(serv_NLIST_USAGE),getNickname());
	return;
     }
   int nbRes = services.getDatabase().dbSelect("nickname,lasthost,email", "nicks", "lasthost like '"+tomatch+"'");
   CResult *myRes = services.getDatabase().dbGetResultSet();
   int f=0;
   if(dest!="")
     {
	User *tmp = services.findUser(dest);
	if(tmp==0)
	  {
	     origin.sendMessage(GETLANG(serv_NLIST_ERROR_NOT_ONLINE,dest),getNickname());
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
	     origin.sendMessage(GETLANG(serv_NLIST_TOO_MANY_FOUND),getNickname());
	     return;
	  }

	if(dest=="")
	  {
	     origin.sendMessage(String(tosend),getNickname());
	  }
	else
	  {
	     services.serviceNotice(String(tosend),"Serv",dest);
	  }
     }
   if(nbRes==0)
     {
	origin.sendMessage(GETLANG(serv_NLIST_NONE_FOUND,tomatch),getNickname());
     }

   delete myRes;
   services.log(origin,"Serv","Did a nlist on "+tomatch+" "+String::convert(f)+" matches found");
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

   services.logLine(togo);
   services.sendGOper(getNickname(),togo);
}

SERV_FUNC (Service::parseELIST)
{
   String tomatch = tokens.nextToken();
   Kine::ClientName dest = tokens.nextToken();
   if(tomatch=="")
     {
	origin.sendMessage(GETLANG(serv_ELIST_USAGE),getNickname());
	return;
     }
   int nbRes = services.getDatabase().dbSelect("nickname, lasthost, email", "nicks", "email like '"+tomatch+"'");
   CResult *myRes = services.getDatabase().dbGetResultSet();
   if(dest!="")
     {
	User *tmp = services.findUser(dest);
	if(tmp==0)
	  {
	     origin.sendMessage(GETLANG(serv_NLIST_ERROR_NOT_ONLINE,dest),getNickname());
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
	     origin.sendMessage(GETLANG(serv_NLIST_TOO_MANY_FOUND),getNickname());
	     return;
	  }

	if(dest=="")
	  origin.sendMessage(tosend,getNickname());
	else
	  services.serviceNotice(String(tosend),getNickname(),dest);

	services.getDatabase().dbGetRow();
     }
   if(nbRes==0)
     {
	origin.sendMessage(GETLANG(serv_NLIST_NONE_FOUND,tomatch),getNickname());
     }

   if(dest=="")
     {
	services.log(origin,getNickname(),"Did an elist on "+tomatch);
	String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch;
	services.logLine(togo);
	services.sendGOper(getNickname(),togo);
     }
   else
     {

	services.log(origin,getNickname(),"Did an elist on "+tomatch+" and sent it to "+dest);
	String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch+" and sent the results to "+dest;
	services.logLine(togo);
	services.sendGOper(getNickname(),togo);
     }
   delete myRes;

}

SERV_FUNC (Service::parseDELNICK)
{
   String who  = tokens.nextToken();
   String reason = tokens.rest();
   if(who=="" || reason=="")
     {
	origin.sendMessage(GETLANG(serv_DELNICK_USAGE),getNickname());
	return;
     }
   if(!services.isNickRegistered(who))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   /* Are we trying to delete a staff nickname? */
   if(services.getStatic().getServiceAccess(getNickname(),who)>0)
     {
	/* Yep... Unless a 500 tell em to get stuffed */
	if(services.getStatic().getServiceAccess(getNickname(),origin.getNickname())<500)
	  {
	     origin.sendMessage(GETLANG(serv_NOT_ON_STAFF),getNickname());
	     services.sendGOper(getNickname(),origin.getNickname()+" tried to perform a \002delnick\002 on a staff nickname ("+who+")");
	     return;
	  }

     }

   String togo = origin.getNickname()+" did \002delnick\002 on "+who+" for \002"+reason;
   services.logLine(togo, Log::Warning);
   services.sendGOper(getNickname(),togo);
   services.getDatabase().dbDelete("nicks", "nickname='"+who+"'");
   services.log(origin,"Serv","Deleted nickname "+who+" : "+reason);
   origin.sendMessage(GETLANG(serv_DELNICK_SUCCESS,who,reason),getNickname());

}

SERV_FUNC (Service::parseCLIST)
{
   Kine::ClientName who = tokens.nextToken();
   Kine::ClientName send = tokens.nextToken();
   if(who=="")
     {
	origin.sendMessage(GETLANG(serv_CLIST_USAGE),getNickname());
	return;
     }

   if(!services.isNickRegistered(who))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   if(send!="")
     {

	User *ptr = services.findUser(send);
	if(ptr==0)
	  {
	     origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),getNickname());
	     return;
	  }

     }

   int totalc = services.getChannel().maxChannels();
   int userc = services.getChannel().maxChannelsUser(who);
   int totala = services.getChannel().maxChannelsAccess();
   int theid = services.getStatic().getRegisteredNickID(who);
   int nbRes = services.getDatabase().dbSelect("chanid,access", "chanaccess", "nickid='"+String::convert(theid)+"'");
   String togo = origin.getNickname() + " did a \002clist\002 on "+who+", "
     +String::convert(nbRes)+" matches found from "+String::convert(totalc)+
     " channels and "+String::convert(totala)+" access entries";
   if(send!="")
     togo = togo + " and sent it to "+send;
   services.sendGOper(getNickname(),togo);

   CResult *myRes = services.getDatabase().dbGetResultSet();
   if(send=="")
     origin.sendMessage(GETLANG(serv_CLIST_START_SELF,who),getNickname());
   else
     {
	origin.sendMessage(GETLANG(serv_CLIST_START_DEST,who,send),getNickname());
	services.serviceNotice(GETLANG(serv_CLIST_START_OTHER,who,origin.getNickname()),getNickname(),send);
     }

   for(int i=0; i<nbRes; i++)
     {
	String cname = myRes->getValue(i,0);
	String caxs = myRes->getValue(i,1);
	String ccname = services.getChannel().getChanName(cname.toInt());
	String tosend = "Channel: \002"+ccname+"\002 with level \002"+caxs;
	if(send=="")

	  origin.sendMessage(tosend,getNickname());
	else
	  services.serviceNotice(String(tosend),getNickname(),send);
	services.getDatabase().dbGetRow();
     }
   if(nbRes==0)
     origin.sendMessage(GETLANG(serv_CLIST_NONE_FOUND),getNickname());
   services.log(origin,getNickname(),"Did a clist on "+who);
}

void Service::delFreeze(Kine::ChannelName const &chan)
{
   services.getDatabase().dbUpdate("chanfreeze","expires=0","name='"+
				    String::convert(services.getChannel().getChanID(chan.IRCtoLower()))+"'");
}

void Service::addFreeze(Kine::ChannelName const &chan, String const &setby,
		       int const &expires, String const &reason)
{
   services.getDatabase().dbInsert("chanfreeze","'','"+String::convert(services.getChannel().getChanID(chan.IRCtoLower()))+"','"
				    +setby+"',NOW(),"+String::convert(expires)
				    +",'"+reason+"'");
}

bool Service::isFreezed(Kine::ChannelName const &chan)
{
   if(services.getDatabase().dbSelect("id","chanfreeze","name='"
				       +String::convert(services.getChannel().getChanID(chan.IRCtoLower()))
				       +"' AND expires>"+String::convert(services.getCurrentTime())) < 1)
     return false;
   else
     return true;
}

int Service::timesFreezed(Kine::ChannelName const &chan)
{
   return services.getDatabase().dbSelect("id","chanfreeze","name='"
					   +String::convert(services.getChannel().getChanID(chan.IRCtoLower()))
					   +"'");
}

