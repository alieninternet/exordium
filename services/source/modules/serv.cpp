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

#include "include/serv.h"
#include "exordium/services.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include "exordium/sql.h"
#include <sys/time.h>

using LibAIS::String;
using LibAIS::StringTokens;
using namespace Exordium;

struct Serv::functionTableStruct const
  Serv::functionTable[] =
{
     {"clist", &Serv::parseCLIST},
     {"delnick", &Serv::parseDELNICK},
     {"elist", &Serv::parseELIST},
     {"nlist", &Serv::parseNLIST},
     {"helpon", &Serv::parseHELPON},
     {"help", &Serv::parseHELP},
     {"user", &Serv::parseUSER},
     {"raw", &Serv::parseRAW},
     {"chan", &Serv::parseCHAN},
     {"die", &Serv::parseDIE},
     {"news", &Serv::parseNEWS},
     {"setpass", &Serv::parseSETPASS},
     {0, 0}
};
void
  Serv::parseLine (StringTokens& line, User& origin, String const &ch)
{
   return;
}

void
  Serv::parseLine (StringTokens& line, User& origin)
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
		  services->helpme(togo,getName());
		  return;
	       }

	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage ("Unrecognised Command", getName());
}
void
  SERV_FUNC (Serv::parseDIE)
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

void
  SERV_FUNC (Serv::parseSETPASS)
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
   String query = "UPDATE nicks set password='" + epass + "' where nickname='"+who+"'";
   services->getDatabase().query(query);
   String togo = "\002"+origin.getNickname()+"\002 changed password for nickname "+who+" to [HIDDEN]";
   services->helpme(togo,getName());
}

void
  SERV_FUNC (Serv::parseRAW)
{
   std::string c = tokens.rest();
   services->queueAdd(c);
   String togo = origin.getNickname()+" did \002RAW\002 - "+c;
   services->helpme(String(togo),getName());
}
void
  SERV_FUNC (Serv::parseNEWS)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("Usage: news add/del/list",getName());
	return;
     }
   if(command=="list")
     {
	String query = "SELECT * from news";
	MysqlRes res = services->getDatabase().query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String id = row[0];
	     String level = row[1];
	     String expires = row[2];
	     String txt = row[3];
	     String togo = "ID \002[\002"+id+"\002]\002 Level \002[\002"+level+"\002]\002 Expires \002[\002"+expires+"\002]\002 Text \002[\002"+txt+"\002]\002";
	     origin.sendMessage(togo,getName());
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
	String query = "DELETE FROM news where id="+id;
	services->getDatabase().query(query);
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
	String query = "INSERT into news values ('','"+type+"','"+String::convert(nexpires)+"','"+text+"');";
	services->getDatabase().query(query);
	origin.sendMessage("New news item added successfully",getName());
     }

}
void
  SERV_FUNC (Serv::parseCHAN)
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
	services->helpme(String(togo),getName());
	services->getChannel().chanDelAccess(channel,oldowner);
	services->getChannel().chanAddAccess(channel,newowner,"500");
	String query = "UPDATE chans set owner='" + newowner + "' where name='"+channel+"'";
	services->getDatabase().query(query);
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
	services->helpme(String(togo),getName());
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
	services->helpme(String(togo),getName());
	services->getChannel().registerChannel(channel,thenick);
	services->log(origin,getName(),String("Registered ")+channel+" to "+thenick);
	return;
     }
}
void
  SERV_FUNC (Serv::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getName(),word,parm);
   String tolog = "Did HELP on word " + word + " parm " + parm;
   services->log(origin,getName(),String(tolog));
}

void
  SERV_FUNC (Serv::parseUSER)
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
	     services->helpme(String(togo),getName());
	     return;
	  }
	if(taccess==access)
	  {
	     origin.sendMessage("Error: That person has the same access as you",getName());
	     return;
	  }
	String togo = origin.getNickname() + " modified access for \002"+toadd+"\002 "+String::convert(taccess)+"->"+level;
	services->helpme(togo,getName());
	String query = "UPDATE access set access='" + level + "' WHERE nickname='" + toadd + "'";
	services->getDatabase().query(query);
	services->log(origin,getName(),String("Modified access for ")+toadd+" from "+String::convert(taccess)+"->"+level);
	return;
     }
   if(command=="list")
     {
	String query = "SELECT * from access where service='serv' order by id";
	MysqlRes res = services->getDatabase().query(query);
	MysqlRow row;
	while (( row  = res.fetch_row()))
	  {
	     String nickname = row[1];
	     String access = row[3];
	     String togo = "\002"+nickname+"\002 has level \002"+access;
	     origin.sendMessage(String(togo),getName());
	  }
	res.free_result();
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
	     services->helpme(String(togo),getName());
	     return;
	  }
	String query = "DELETE from access where service='serv' and nickname='" + toadd + "'";
	services->getDatabase().query(query);
	origin.sendMessage("Command complete",getName());
	String togo = origin.getNickname() + " deleted \002 " + toadd + "\002 from Serv";
	services->helpme(String(togo),getName());
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
	String query = "INSERT into access values ('','" + toadd + "','serv','" + level + "')";
	services->getDatabase().query(query);
	origin.sendMessage("Command completed",getName());
	String togo = origin.getNickname()+" added \002"+toadd+"\002 to Serv with level \002"+level;
	services->helpme(String(togo),getName());
	String tolog = "Added "+toadd+" to Serv with level "+toadd;
	services->log(origin,getName(),String(tolog));
	return;
     }
   origin.sendMessage("Uncognised sub-command",getName());
   return;
}
void
  SERV_FUNC (Serv::parseHELPON)
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
   services->helpme(tosend,getName());
}
void
  SERV_FUNC (Serv::parseNLIST)
{
   String tomatch = tokens.nextToken();
   String dest = tokens.nextToken();
   if(tomatch=="")
     {
	origin.sendMessage("Usage: nlist <match> <optinal destination>",getName());
	return;
     }
   String query = "SELECT nickname,lasthost,email from nicks WHERE lasthost like '"+tomatch+"'";
   MysqlRes res = services->getDatabase().query(query);
   MysqlRow row;
   int f=0;
   while ((row = res.fetch_row()))
     {
	f++;
	String tnick = row[0];
	String thost = row[1];
	String temail = row[2];
	String tosend = String("\002")+tnick+"\002 with last address \002"+thost+"\002 "+temail+"\002";
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
   services->helpme(togo,"Serv");
   res.free_result();
}
void
  SERV_FUNC (Serv::parseELIST)
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
	String query = "SELECT nickname,lasthost,email from nicks where email like '" + tomatch + "'";
	MysqlRes res = services->getDatabase().query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String nickname = row[0];
	     String lasthost = row[1];
	     String email = row[2];
	     String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
	     origin.sendMessage(tosend,getName());
	  }
	services->log(origin,"Serv","Did an elist on "+tomatch);
	String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch;
	services->helpme(togo,"Serv");
	res.free_result();
	return;
     }
   //Else send to given client
   /* This should proberly be wrapped up into one function, just checking
    * for the correct place to send .. like above *points up* */
   String query = "SELECT nickname,lasthost,email from nicks where email like '" + tomatch + "'";
   MysqlRes res = services->getDatabase().query(query);
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	String nickname = row[0];
	String lasthost = row[1];
	String email = row[2];
	String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
	services->serviceNotice(tosend,getName(),dest);
     }
   services->log(origin,"Serv","Did an elist on "+tomatch+" and sent it to "+dest);
   String togo = origin.getNickname() + " did an \002elist\002 on "+tomatch+" and sent the results to "+dest;
   services->helpme(togo,"Serv");
   res.free_result();

}
void
  SERV_FUNC (Serv::parseDELNICK)
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
   services->helpme(togo,"Serv");
   String query = "DELETE from nicks where nickname='" + who + "'";
   services->getDatabase().query(query);
   services->log(origin,"Serv","Deleted nickname "+who+" : "+reason);
}

void
  SERV_FUNC (Serv::parseCLIST)
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
	services->helpme(togo,"Serv");
	int theid = services->getRegisteredNickID(who);
	String query = "SELECT chanid,access from chanaccess where nickid='" + String::convert(theid) + "'";
	MysqlRes res = services->getDatabase().query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String cname = row[0];
	     String caxs = row[1];
	     String ccname = services->getChannel().getChanName(cname.toInt());
	     String tosend = ccname+" with "+caxs;
	     origin.sendMessage(tosend,getName());
	  }
	services->log(origin,"Serv","Did a clist on "+who);
	res.free_result();
	return;
     }
}

EXORDIUM_SERVICE_INIT_FUNCTION
{
   return new Serv();
}

// Module information structure
const Serv::moduleInfo_type Serv::moduleInfo =
{
   "Service Service",
     0, 0
};

// Start the service
void Serv::start(Exordium::Services& s)
{
   services = &s;
   services->registerService(getName(),getName(),"ircdome.org","+dz",
			    "\037Serv\037ice :)");
   services->serviceJoin(getName(),"#Debug");
}
