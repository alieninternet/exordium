/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/serv.h"
#include "exordium/services.h"
#include "exordium/nickname.h"
#include "exordium/channel.h"
#include "kineircd/str.h"
#include "exordium/sql.h"
#include "exordium/module.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

namespace Exordium {

struct Serv::functionTableStruct const
  Serv::functionTable[] = {
  {"clist", parseCLIST},
  {"delnick", parseDELNICK},
  {"elist", parseELIST},
  {"nlist", parseNLIST},
  {"helpon", parseHELPON},
  {"help", parseHELP},
  {"user", parseUSER},
  {"raw", parseRAW},
  {"chan", parseCHAN},
  {"die", parseDIE},
  {0}
};
void
Serv::parseLine (String const &line, String const &requestor, String const &ch)
{
return;
}

void 
Serv::parseLine (String const &line, String const &requestor)
{   
  StringTokens st (line);
  String origin = requestor;
  String command = st.nextToken ().toLower ();
  for (int i = 0; functionTable[i].command != 0; i++)
    {
      // Does this match?   
      if (command == functionTable[i].command)
        {
	  // Check access :-)
		int required = Nickname::getRequiredAccess("Serv",command.toLower());
		int access = Nickname::getAccess("Serv",origin);
		if(required>access)
		{
			Services::serviceNotice("You do not have enough access for that command","Serv",origin);
			String togo = origin+" tried to use \002"+command+"\002";
			Services::helpme(togo,"Serv");
			return;
		}

          // Run the command and leave
          functionTable[i].function (origin, st);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "Serv", requestor);
}
void
SERV_FUNC (Serv::parseDIE)
{
exit(0);
}
void
SERV_FUNC (Serv::parseRAW)
{
string c = tokens.rest();
Services::queueAdd(c);
String togo = origin+" did \002RAW\002 - "+c;
Services::helpme(String(togo),"Serv");
}
void
SERV_FUNC (Serv::parseCHAN)
{
	String command = tokens.nextToken();
	String channel = tokens.nextToken();
	if(command=="")
	{
		Services::serviceNotice("Usage: chan add/del/mod","Serv",origin);
		return;
	}
	if(command=="mod")
	{
		String newowner = tokens.nextToken();
		if(channel=="" || newowner=="")
		{
			Services::serviceNotice("Usage: chan mod #channel NewOwner","Serv",origin);
			return;
		}
		if(!Nickname::isNickRegistered(newowner))
		{
			Services::serviceNotice("Error: New owner's nickname is not registered","Serv",origin);
			return;
		}
		String newtopic = "This channel is now owned by "+newowner;
		Channel::setTopic(channel,newtopic);
		Channel::updateTopic(channel,newtopic);
		int chanid = Channel::getChanID(channel);
		String oldowner = Channel::getChanOwner(chanid);
		String togo = origin+" changed \002ownership\002 of "+channel+" "+oldowner+"->"+newowner;
		Services::helpme(String(togo),"Serv");
		Channel::chanDelAccess(channel,oldowner);
		Channel::chanAddAccess(channel,newowner,"500");
		String query = "UPDATE chans set owner='" + newowner + "' where name='"+channel+"'";
		Sql::query(query);
		Services::log(origin,"Serv",String("Changed ownership of ")+channel+" to "+newowner+" ("+oldowner+")");
		return;
	}
	if(command=="del")
	{
		String reason = tokens.rest();
		if(channel=="" || reason=="")
		{
			Services::serviceNotice("Usage: chan del #channel reason","Serv",origin);
			return;
		}
		String togo = origin + "\002 de-registered\002 "+channel+" for \002"+reason+"\002";
		Services::helpme(String(togo),"serv");
		Channel::deregisterChannel(channel,reason);
		Services::log(origin,"Serv",String("Deregistered ")+channel+" for "+reason);	
		return;
	}
	if(command=="add")
	{
		String thenick = tokens.nextToken();
		if(channel=="" || thenick=="")
		{
			Services::serviceNotice("Usage: chan add #channel owner","Serv",origin);
			return;
		}
		if(Channel::isChanRegistered(channel))
		{
			Services::serviceNotice("That channel is already registered - Try mod","Serv",origin);
			return;
		}
		if(!Nickname::isNickRegistered(thenick))
		{
			Services::serviceNotice("Target nickname does not exist","Serv",origin);
			return;
		}
		String togo = origin + "\002 registered\002 " + channel + " to "+thenick;
		Services::helpme(String(togo),"Serv");
		Channel::registerChannel(channel,thenick);
		Services::log(origin,"Serv",String("Registered ")+channel+" to "+thenick);
		return;
	}
}
void
SERV_FUNC (Serv::parseHELP)
{
String word = tokens.nextToken();
String parm = tokens.nextToken();
Services::doHelp(origin,"Serv",word,parm);
String tolog = "Did HELP on word " + word + " parm " + parm;
Services::log(origin,"Serv",String(tolog));
}

void
SERV_FUNC (Serv::parseUSER)
{
	String command = tokens.nextToken();
	String toadd = tokens.nextToken();
	String level = tokens.nextToken();
	int access = Nickname::getAccess("Serv",origin);
	if(command=="")
	{
		Services::serviceNotice("Commands are : add/del/mod/list","Serv",origin);
		return;		
	}
	if(command=="mod")
	{
		if(toadd=="" || level=="")
		{
			Services::serviceNotice("Usage: user mod nickname level","Serv",origin);
			return;
		}
		if(toadd==origin)
		{
			Services::serviceNotice("Silly billy... you can't mod yourself!","Serv",origin);
			return;
		}
		int ilevel = level.toInt();
		if(ilevel>access || ilevel==access)
		{
			Services::serviceNotice("Error: You cannot set someones access higher than, or equal to your own","Serv",origin);
			return;
		}
		int taccess = Nickname::getAccess("Serv",toadd);
		if(taccess>access)
		{
			Services::serviceNotice("Error: That person has higher access than you","Serv",origin);
			String togo = origin+" tried to modify access for a higher user than themselves ("+toadd+")";
			Services::helpme(String(togo),"Serv");
			return;
		}
		if(taccess==access)
		{
			Services::serviceNotice("Error: That person has the same access as you","Serv",origin);
			return;
		}
		String togo = origin + " modified access for \002"+toadd+"\002 "+String::convert(taccess)+"->"+level;
		Services::helpme(togo,"Serv"); 
		String query = "UPDATE access set access='" + level + "' WHERE nickname='" + toadd + "'";
		Sql::query(query);
		Services::log(origin,"Serv",String("Modified access for ")+toadd+" from "+String::convert(taccess)+"->"+level);
		return;
	}	
	if(command=="list")
	{
		String query = "SELECT * from access where service='serv' order by id";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while (( row  = res.fetch_row()))
		{
			String nickname = ((string) row[1]).c_str();
			String access = ((string) row[3]).c_str();
			String togo = "\002"+nickname+"\002 has level \002"+access;
			Services::serviceNotice(String(togo),"Serv",origin);
		}
	return;
	}
	if(command=="del")
	{
		if(toadd=="")
		{
			Services::serviceNotice("Usage is user del nickname","Serv",origin);
			return;
		}
		if((Nickname::getAccess("Serv",toadd))==0)
		{
			Services::serviceNotice("That person does not have access to serv","Serv",origin);
			return;
		}
		int faccess = Nickname::getAccess("Serv",toadd);
		if(faccess>access)
		{
			Services::serviceNotice("You do not have enough access to perform that operation on a staff nickname","Serv",origin);
			String togo = origin + " tried to use \002userdel\002 on a \002staff\002 nickname";
			Services::helpme(String(togo),"Serv");
			return;
		}
		String query = "DELETE from access where service='serv' and nickname='" + toadd + "'";
		Sql::query(query);
		Services::serviceNotice("Command complete","Serv",origin);
		String togo = origin + " deleted \002 " + toadd + "\002 from Serv";
		Services::helpme(String(togo),"Serv");
		Services::log(origin,"Serv","Deleted "+toadd+" from Serv");
		return; 
	}
	if(command=="add")
	{
		if(toadd=="" || level=="")
		{
			Services::serviceNotice("Usage: user add nickname level","Serv",origin);
			return;
		}
		if(origin.toLower()==toadd.toLower())
		{
			Services::serviceNotice("You can't adduser yourself, silly.","Serv",origin);
			return;
		}
		if(!Nickname::isNickRegistered(toadd))
		{
			Services::serviceNotice("That nickname is not registered","Serv",origin);
			return;
		}
		if((Nickname::getAccess("Serv",toadd))>0)
		{
			Services::serviceNotice("That person already has access, try mod","Serv",origin);
			return;
		}
		if(level.toInt()<1)
		{
			Services::serviceNotice("Error: You cannot add someone with less than 1 access","Serv",origin);
			return;
		}
		if(level.toInt()>499)
		{
			Services::serviceNotice("Error: You cannot add someone with higher than 499 access","Serv",origin);
			return;
		}
		String query = "INSERT into access values ('','" + toadd + "','serv','" + level + "')";
		Sql::query(query);
		Services::serviceNotice("Command completed","Serv",origin);
		String togo = origin+" added \002"+toadd+"\002 to Serv with level \002"+level;
		Services::helpme(String(togo),"Serv");
		String tolog = "Added "+toadd+" to Serv with level "+toadd;
		Services::log(origin,"Serv",String(tolog));
		return;
	}
	Services::serviceNotice("Uncognised sub-command","Serv",origin);
	return;
}
void
SERV_FUNC (Serv::parseHELPON)
{
	int access = Nickname::getAccess("Serv",origin);
	if(access>50)
	{
		Services::serviceNotice("You are now an IRCDome services assistant","Serv",origin);
		String tosend = ":services.ircdome.org HELPER "+origin+" "+String::convert(access);
		Services::queueAdd(String(tosend));
		Services::log(origin,"Serv","Become a services helper at level "+String::convert(access));
		return;		
	}
Services::log(origin,"Serv","Failed to become a helper (not enough access)");
String tosend = origin+" failed to become a helper - Not enough access";
Services::helpme(tosend,"Serv");
}
void
SERV_FUNC (Serv::parseNLIST)
{
	String tomatch = tokens.nextToken();
	String dest = tokens.nextToken();
	if(tomatch=="")
	{
		Services::serviceNotice("Usage: nlist <match> <optinal destination>","Serv",origin);
		return;
	}
	String query = "SELECT nickname,lasthost,email from nicks WHERE lasthost like '"+tomatch+"'";
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	int f=0;
	while ((row = res.fetch_row()))
	{
		f++;
		String tnick = ((string) row[0]).c_str();
		String thost = ((string) row[1]).c_str();
		String temail = ((string) row[2]).c_str();
		String tosend = String("\002")+tnick+"\002 with last address \002"+thost+"\002"+temail+"\002";
		if(dest=="")
		{
		Services::serviceNotice(String(tosend),"Serv",origin);
		} 
		else
		{
		Services::serviceNotice(String(tosend),"Serv",dest);
		}
	}
	Services::log(origin,"Serv","Did a nlist on "+tomatch+" "+String::convert(f)+" matches found");
	String togo = origin+" did a \002nlist\002 on "+tomatch+" "+String::convert(f)+" matches found";
	Services::helpme(togo,"Serv");
}
void
SERV_FUNC (Serv::parseELIST)
{
String tomatch = tokens.nextToken();
String dest = tokens.nextToken();
if(tomatch=="")
	{
		Services::serviceNotice("Usage: elist <match> <optional destination>","Serv",origin);
		return;
	}
if(dest=="")
	{
		//Return to origin.
		String query = "SELECT nickname,lasthost,email from nicks where email like '" + tomatch + "'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while ((row = res.fetch_row()))
		{
			String nickname = ((string) row[0]).c_str();
			String lasthost = ((string) row[1]).c_str();
			String email = ((string) row[2]).c_str();
			String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
			Services::serviceNotice(tosend,"Serv",origin);
		}
		Services::log(origin,"Serv","Did an elist on "+tomatch);
		String togo = origin + " did an \002elist\002 on "+tomatch;
		Services::helpme(togo,"Serv");
		return;
	}
//Else send to given client
		String query = "SELECT nickname,lasthost,email from nicks where email like '" + tomatch + "'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while ((row = res.fetch_row()))
		{
			String nickname = ((string) row[0]).c_str();
			String lasthost = ((string) row[1]).c_str();
			String email = ((string) row[2]).c_str();
			String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
			Services::serviceNotice(tosend,"Serv",dest);
		}
		Services::log(origin,"Serv","Did an elist on "+tomatch+" and sent it to "+dest);
		String togo = origin + " did an \002elist\002 on "+tomatch+" and sent the results to "+dest;
		Services::helpme(togo,"Serv");


}
void
SERV_FUNC (Serv::parseDELNICK)
{
String who  = tokens.nextToken();
String reason = tokens.rest();
if(who=="" || reason=="")
	{
		Services::serviceNotice("Usage: delnick nickname reason","Serv",origin);
		return;
	}
int faccess = Nickname::getAccess("Serv",who);
int oaccess = Nickname::getAccess("Oper",who);
Services::Debug(String::convert(faccess));
Services::Debug(String::convert(oaccess));

String togo = origin+" did \002delnick\002 on "+who+" for \002"+reason;
Services::helpme(togo,"Serv");
String query = "DELETE from nicks where nickname='" + who + "'";
Sql::query(query);
Services::log(origin,"Serv","Deleted nickname "+who+" : "+reason);
}

void
SERV_FUNC (Serv::parseCLIST)
{
String who = tokens.nextToken();
String send = tokens.nextToken();
if(!Nickname::isNickRegistered(who))
	{
		Services::serviceNotice("That nickname is not registered","Serv",origin);
		return;
	}
if(send=="")
	{
		int totalc = Channel::maxChannels();
		int userc = Channel::maxChannelsUser(who);
		int totala = Channel::maxChannelsAccess();
		String togo = origin + " did a \002clist\002 on "+who+", "+String::convert(userc)+" matches found from "+String::convert(totalc)+" channels and "+String::convert(totala)+" access entries";
		Services::helpme(togo,"Serv");
		int theid = Nickname::getRegisteredNickID(who);
		String query = "SELECT chanid,access from chanaccess where nickid='" + String::convert(theid) + "'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while ((row = res.fetch_row()))
		{
			String cname = ((string) row[0]).c_str();
			String caxs = ((string) row[1]).c_str();
			String ccname = Channel::getChanName(cname.toInt());
			String tosend = ccname+" with "+caxs;
			Services::serviceNotice(tosend,"Serv",origin);
		}
		Services::log(origin,"Serv","Did a clist on "+who);
		return;
	}
}

extern "C" Module *service_init(void) {
   return new Module("serv", new Serv());
}

};

