/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

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
  Chan::functionTable[] = {
  {"access", parseACCESS},
  {".access", parseACCESS},
  {"ban", parseBAN},
  {".ban", parseBAN},
  {".kick", parseKICK},
  {"kick", parseKICK},
  {".op", parseOP},
  {"op", parseOP},
  {".voice", parseVOICE},
  {"voice", parseVOICE},
  {"register", parseREGISTER},
  {"help", parseHELP},
  {"deop", parseDEOP},
  {".deop", parseDEOP},
  {"devoice", parseDEVOICE},
  {".devoice", parseDEVOICE},
  {"topic", parseTOPIC},
  {".topic", parseTOPIC},
  {"adduser", parseADDUSER},
  {".adduser", parseADDUSER},
  {"info", parseINFO},
  {".info", parseINFO},
  {"listban", parseLISTBAN},
  {".listban", parseLISTBAN},
  {0}
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
          functionTable[i].function (origin, st, ch);
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
          functionTable[i].function (origin, st, ch);
          return;
        }
    }
  Services::serviceNotice ("\002[\002Unrecognised Command\002]\002", "Chan", requestor);
}

void
CHAN_FUNC (Chan::parseLISTBAN)
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
		Services::serviceNotice("\002[\002Incorrect Command Usage\002]\002 Usage: listban #channel","chan",origin);
		std::cout << "BOO BOO" << std::endl;
		return;		
	}
	if(!Channel::isChanRegistered(channel))
	{
		Services::serviceNotice("\002[\002Fatal Error\002]\002 Channel not registered","chan",origin);
		std::cout << "its not registered!" << std::endl;
		return;
	}
	int tid = Channel::getChanID(channel);
	Services::serviceNotice("\002[\002Ban List\002]\002 for \002"+channel,"chan",origin);
	String tquery = "SELECT * from chanbans where chan='" + String::convert(tid) + "'";
	MysqlRes res = Sql::query(tquery);
	MysqlRow row;
	int j=0;
	while ((row = res.fetch_row()))
	{
		j++;
		String mask = ((std::string) row[2]).c_str();
		String setby = ((std::string) row[3]).c_str();
		String seton = ((std::string) row[4]).c_str();
		String expireon = ((std::string) row[5]).c_str();
		String reason = ((std::string) row[6]).c_str();
		String tosend = "\002[\002"+String::convert(j)+"\002]\002 Mask \002"+mask+"\002 SetBy \002"+setby+"\002 Date \002"+seton+"\002 Expires \002"+expireon+"\002 Reason \002"+reason+"\002";
		Services::serviceNotice(tosend,"chan",origin);
	}
	Services::serviceNotice("\002[\002Ban List\002]\002 Finished","chan",origin);
}
void
CHAN_FUNC (Chan::parseINFO)
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
		Services::serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: info #channel","Chan",origin);
		return;
	}
	if(!Channel::isChanRegistered(channel))
	{
		Services::serviceNotice("\002[\002Fatal Error\002]\002 Channel not registered","Chan",origin);
		return;
	}
	int cid = Channel::getChanID(channel);
	String nowner = Channel::getChanOwner(cid);
	String ntotal = Channel::getChanCount();
	String nuniq = String::convert(Channel::getChanID(channel));
	String nnuniq = String::convert(Channel::getOnlineChanID(channel));
	String toa = "\002[\002Channel Information\002]\002 for \002"+channel;
	String tob = "Owner : "+nowner;
	String toc = "Unique IDS: "+nuniq+"/"+nnuniq;
	Services::serviceNotice(toa,"Chan",origin);
	Services::serviceNotice(tob,"Chan",origin);
	Services::serviceNotice(toc,"Chan",origin);	
}
void
CHAN_FUNC (Chan::parseADDUSER)
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

	String nickname = tokens.nextToken();
	String level = tokens.nextToken();
	if(channel=="" | nickname=="" | level=="")
	{
		Services::serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: adduser #channel nickname level","Chan",origin);
		return;
	}
	if(!Nickname::isNickRegistered(nickname))
	{
		Services::serviceNotice("\002[\002Fatal Error\002]\002 Nickname not registered","Chan",origin);
		return;
	}
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick  = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		int waccess = level.toInt();
		int taccess = Channel::getChanAccess(channel,nickname);
		if(waccess==access || waccess>access || waccess<1 || waccess>499)
		{
			Services::serviceNotice("Error: You cannot add someone with higher, or equal access to your own","Chan",origin);
			return;
		}
		if(taccess>0)
		{
			Services::serviceNotice("Error: That person already has access","Chan",origin);
			return;
		}
		Channel::chanAddAccess(channel,nickname,level);
		Services::log(origin,"Chan","Added "+nickname+" with level "+level,channel);
		Services::serviceNotice("Command completed successfully","Chan",origin);
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
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>150)
		{
			Channel::updateTopic(channel,topic);
			Channel::setTopic(channel,topic);
			Services::log(origin,"Chan","Set topic to "+topic,channel);
			return;
		}
		more = st.hasMoreTokens();
		
	}
Services::serviceNotice("Sorry, you do not have the required access in that channel","Chan",origin);
return;
}

void
CHAN_FUNC (Chan::parseHELP)
{
	String word = tokens.nextToken();
	String parm = tokens.nextToken();
	Services::doHelp(origin,"chan",word,parm);
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
		Services::serviceNotice("Usage:  ban #channel nickname your optional reason goes here","Chan",origin);
		return;
	}
	if(reason=="")
	{
		reason = "You are banned";
	}
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>150)
		{
			StringTokens st (who);
			String temp1 = st.nextToken('@');
			String temp2 = st.rest();
			if(temp2.length()<2)
			{
				String tban = Nickname::getHost(temp1);
				String toban = "*!*@"+tban;
				who = toban;
			}
			int cid = Channel::getChanID(channel);
			Services::serverMode(channel,"+b",who);
			long newt = Services::currentTime + 120;
			Channel::addChanBan(cid,who,origin,newt,reason);
			String rs = "("+origin+"/"+currnick+") "+reason;
			Channel::banChan(channel,who,rs);
			return;
		}
	}
	Services::serviceNotice("\002[\002No Access\002]\002","Chan",origin);

}

void
CHAN_FUNC (Chan::parseREGISTER)
{
	String channel = tokens.nextToken();
	if (channel=="")
	{
		Services::serviceNotice("Usage: register #channel","Chan",origin);
		return;
	}
	if ( channel[0] != '#' )
	{
		Services::serviceNotice("Error: Channel names must begin with the '#' symbol","Chan",origin);
		return;
	}
	if(!Nickname::isNickRegistered(origin))
	{
		Services::serviceNotice("Error: Your nickname is not registered","Chan",origin);
		return;
	}
	if(!Nickname::isIdentified(origin,origin))
	{
		Services::serviceNotice("Error: You must be identified, and using the nickname you wish to own the channel","Chan",origin);
		return;
	}
	int owned = Channel::ownedChannels(origin);
	if(owned>0)
	{
		Services::serviceNotice("Error: You are only permitted own one channel per nickname on IRCDome","Chan",origin);
		return;
	}
	if(Channel::isChanRegistered(channel))
	{
		Services::serviceNotice("Error: That channel is already registered","Chan",origin);
		return;
	}
	Channel::registerChannel(channel,origin);
	Services::serviceNotice("Registration Successful","Chan",origin);
	Services::log(origin,"Chan","Registered the channel",channel);
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
		Services::serviceNotice("Usage: op #channel optional list of nicknames to op","Chan",origin);
		return;
	}
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>100)
		{
				String foo = tokens.nextToken();
				if(foo=="")
				{
					if(!Services::isOp(origin,channel))
					{
						Services::mode("Chan",channel,"+o",origin);
						Channel::internalOp(origin,channel);
						Services::log(origin,"Chan","Opped themselves",channel);
						return;
					}
				return;
				}
				if(!Services::isOp(foo,channel))
				{
					Services::mode("Chan",channel,"+o",foo);
					Channel::internalOp(foo,channel);
					Services::log(origin,"Chan","Opped "+foo,channel);
				}
				bool more = false;
				more = tokens.hasMoreTokens();
				while(more==true)
				{
					String foo = tokens.nextToken();
					if(!Services::isOp(foo,channel))
					{
						Services::mode("Chan",channel,"+o",foo);
						Channel::internalOp(foo,channel);
						Services::log(origin,"Chan","Opped " +foo,channel);
					}
					more = tokens.hasMoreTokens();
				}
		more = st.hasMoreTokens();
		}
		
	return;
	}
Services::serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
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
		Services::serviceNotice("Usage: deop #channel optional list of nicknames to op","Chan",origin);
		return;
	}
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>100)
		{
				String foo = tokens.nextToken();
				if(foo=="")
				{
					if(Services::isOp(origin,channel))
					{
						Services::mode("Chan",channel,"-o",origin);
						Channel::internalDeOp(origin,channel);
						Services::log(origin,"Chan","Deopped themselves",channel);
						return;
					}
				return;
				}
				if(Services::isOp(foo,channel))
				{
					Services::mode("Chan",channel,"-o",foo);
					Channel::internalDeOp(foo,channel);
					Services::log(origin,"Chan","Deopped "+foo,channel);
				}
				bool more = false;
				more = tokens.hasMoreTokens();
				while(more==true)
				{
					String foo = tokens.nextToken();
					if(Services::isOp(foo,channel))
					{
						Services::mode("Chan",channel,"-o",foo);
						Channel::internalDeOp(foo,channel);
						Services::log(origin,"Chan","Deopped " +foo,channel);
					}
					more = tokens.hasMoreTokens();
				}
		more = st.hasMoreTokens();
		}
		
	return;
	}
Services::serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
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
		Services::serviceNotice("Usage: voice #channel optional list of nicknames to op","Chan",origin);
		return;
	}
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>50)
		{
				String foo = tokens.nextToken();
				if(foo=="")
				{
					if(!Services::isVoice(origin,channel))
					{
						Services::mode("Chan",channel,"+v",origin);
						Channel::internalVoice(origin,channel);
						Services::log(origin,"Chan","Voiced themselves",channel);
						return;
					}
				return;
				}
				if(!Services::isVoice(foo,channel))
				{
					Services::mode("Chan",channel,"+v",foo);
					Channel::internalVoice(foo,channel);
					Services::log(origin,"Chan","Voiced "+foo,channel);
				}
				bool more = false;
				more = tokens.hasMoreTokens();
				while(more==true)
				{
					String foo = tokens.nextToken();
					if(!Services::isVoice(foo,channel))
					{
						Services::mode("Chan",channel,"+v",foo);
						Channel::internalVoice(foo,channel);
						Services::log(origin,"Chan","Voiced " +foo,channel);
					}
					more = tokens.hasMoreTokens();
				}
		more = st.hasMoreTokens();
		}
		
	return;
	}
Services::serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
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
		Services::serviceNotice("Usage: devoice #channel optional list of nicknames to op","Chan",origin);
		return;
	}
	String la = Nickname::getIDList(origin);
	StringTokens st (la);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>50)
		{
				String foo = tokens.nextToken();
				if(foo=="")
				{
					if(Services::isVoice(origin,channel))
					{
						Services::mode("Chan",channel,"-v",origin);
						Channel::internalDeVoice(origin,channel);
						Services::log(origin,"Chan","DeVoiced themselves",channel);
						return;
					}
				return;
				}
				if(Services::isVoice(foo,channel))
				{
					Services::mode("Chan",channel,"-v",foo);
					Channel::internalDeVoice(foo,channel);
					Services::log(origin,"Chan","DeVoiced "+foo,channel);
				}
				bool more = false;
				more = tokens.hasMoreTokens();
				while(more==true)
				{
					String foo = tokens.nextToken();
					if(Services::isVoice(foo,channel))
					{
						Services::mode("Chan",channel,"-v",foo);
						Channel::internalDeVoice(foo,channel);
						Services::log(origin,"Chan","DeVoiced " +foo,channel);
					}
					more = tokens.hasMoreTokens();
				}
		more = st.hasMoreTokens();
		}
		
	return;
	}
Services::serviceNotice("Sorry, you do not have enough access in that channel","Chan",origin);
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
		Services::serviceNotice("Usage: kick #channel nickname Your reason here","Chan",origin);
		return;
	}
	String thelist = Nickname::getIDList(origin);
	StringTokens st (thelist);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
		String currnick = st.nextToken();
		int access = Channel::getChanAccess(channel,currnick);
		if(access>100)
		{
			if(who.toLower()=="chan")
			{
				String rs = "And why would you want to kick me? :-(";
				Services::serviceKick(channel,origin,rs);
				return;
			}
			String rs = "("+origin+"/"+currnick+") "+reason;
			Services::serviceKick(channel,who,rs);
			Channel::internalDel(who,channel);
			return;
		}
	more = st.hasMoreTokens();
	}
Services::serviceNotice("You do not have the required access to perform that command","Chan",origin);
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
		Services::serviceNotice("Usage: /msg Chan access #channel","Chan",origin);
		return;
	}
	bool foo = false;
	foo = Channel::isChanRegistered(channel);
	if(foo==true)
	{
	}
	if(foo==false)
	{
		Services::serviceNotice("That channel is not registered","Chan",origin);
		return;
	}
	Services::serviceNotice("Channel access list for "+channel,"Chan",origin);
	int chanid = Channel::getChanID(channel);
	String query = "SELECT nickid,access from chanaccess where chanid=" + String::convert(chanid);
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	{
		String tnickid = ((std::string) row[0]).c_str();
		String taccess = ((std::string) row[1]).c_str();
		String tosend = "Nickname \002"+Nickname::getNick(tnickid.toInt())+"\002 Access \002"+taccess+"\002";
		Services::serviceNotice(tosend,"Chan",origin);
	}
	Services::log(origin,"Chan","Did a channel access",channel);
	res.free_result();
}

EXORDIUM_SERVICE_INIT_FUNCTION {
	Services::registerService(name,name,"ircdome.org","+dz",
			"Channel Registration Services");
	Services::serviceJoin(name,"#Debug");
	Channel::synchChannels();
        return new Module("chan", new Chan(name));
}





