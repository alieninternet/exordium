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
#include "kineircd/str.h"
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;


namespace Exordium {

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
	Services::Debug(line);
	Services::Debug(requestor);
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
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st, ch);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "Chan", requestor);
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
		Services::serviceNotice("Usage: info #channel","Chan",origin);
		return;
	}
	if(!Channel::isChanRegistered(channel))
	{
		Services::serviceNotice("Error: Target channel is not registered","Chan",origin);
		return;
	}
	int cid = Channel::getChanID(channel);
	String nowner = Channel::getChanOwner(cid);
	
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
		Services::serviceNotice("Usage: adduser #channel nickname level","Chan",origin);
		return;
	}
	if(!Nickname::isNickRegistered(nickname))
	{
		Services::serviceNotice("Error: That nickname is not registered","Chan",origin);
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
		if(waccess==access || waccess>access)
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
			int cid = Channel::getChanID(channel);
			Services::serverMode(channel,"+b",who);
			Channel::addChanBan(cid,who,origin,1,reason);
			String rs = "("+origin+"/"+currnick+") "+reason;
			Channel::banChan(channel,who,rs);
			return;
		}
	}
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
		Services::Debug(currnick);
		int access = Channel::getChanAccess(channel,currnick);
		Services::Debug(String::convert(access));
		if(access>100)
		{
				String foo = tokens.nextToken();
				if(foo=="")
				{
					Services::Debug("Is already op?");
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
		Services::Debug(currnick);
		int access = Channel::getChanAccess(channel,currnick);
		Services::Debug(String::convert(access));
		if(access>100)
		{
				String foo = tokens.nextToken();
				if(foo=="")
				{
					Services::Debug("Is already op?");
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
		Services::Debug(currnick);
		int access = Channel::getChanAccess(channel,currnick);
		Services::Debug(String::convert(access));
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
		Services::Debug(currnick);
		int access = Channel::getChanAccess(channel,currnick);
		Services::Debug(String::convert(access));
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
		Services::Debug("Its registered damn it!!!");
	}
	if(foo==false)
	{
		Services::serviceNotice("That channel is not registered","Chan",origin);
		return;
	}
	Services::serviceNotice("Channel access list for "+channel,"Chan",origin);
	int chanid = Channel::getChanID(channel);
	String query = "SELECT nickid,access from chanaccess where chanid='" + String::convert(chanid) + "'";
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
}

extern "C" Module *service_init(void) {
   return new Module("chan", new Chan());
}



};

