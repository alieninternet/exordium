/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/parser.h"
#include "exordium/services.h"
#include "exordium/nickname.h"
#include "exordium/channel.h"
#include "exordium/log.h"
#include <kineircd/str.h>
#include "exordium/sql.h"
#include "exordium/ircdome.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

struct Parser::functionTableStruct const
  Parser::functionTable[] = {
  {"PART", parsePART},
  {"MODE", parseM},
  {"PASS", parsePASS},
  {"PRIVMSG", parsePRIVMSG},
  {"SERVER", parseS},
  {"NICK", parseN},
  {"PING", parsePING},
  {"QUIT", parseQUIT},
  {"SQUIT", parseSQUIT},
  {"SJOIN", parseSJOIN},
  {"AWAY", parseAWAY},
  {0}
};

void
Parser::parseLine (String & line)
{
  StringTokens st (line);
  String origin = ""; 
  
  if (line[0] == ':')
    {
      // Skip the first parameter, we do not care about it anyway
      origin = st.nextToken ().substr (1);
        origin = origin.IRCtoLower();
    }
  String command = st.nextToken ();
  for (int i = 0; functionTable[i].command != 0; i++)
    {
      // Does this match?
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st);
          return;
        }
    }
Log::logLine(String("Unparsed Input!: ")+line);
};
void
PARSER_FUNC (Parser::parseAWAY)
{
String reason = tokens.rest();
Services::Debug("\002[\002Away\002]\002 "+origin+" has set away with reason "+reason);
if(Nickname::deopAway(origin))
	{
		if(reason=="")
			{
				Services::Debug("User is coming back");
				return;
			}
			else
			{
				Services::Debug("User is going away");
				int nid = Nickname::getOnlineNickID(origin);
				String query = "SELECT chanid from chanstatus where nickid="+String::convert(nid)+" AND status=2";
				MysqlRes res = Sql::query(query);
				MysqlRow row;
				while ((row = res.fetch_row()))
				{
				String foo = ((std::string) row[0]).c_str();
				String cname = Channel::getChanIDName(foo.toInt());
				String cstr = origin+" "+origin;
				Services::serverMode(cname,"-o+v",cstr);
				}
				
			}
	}
}
void
PARSER_FUNC (Parser::parseS)
{
//Yay for hard-coded server line, honest :-)
  String serverName = tokens.nextToken ();
  String hops = tokens.nextToken ();
  String description = tokens.rest();
  if (!Nickname::isAuthorised (serverName))
    {
      String tosend =
        String (":services.ircdome.org SQUIT ") + serverName +
        " :Unauthorised Links are not permitted on IRCDome - The network administration has been notified";
      Services::queueAdd (String (tosend));
      return;
    }
  Services::AddOnlineServer (serverName, hops, description);
}

void
PARSER_FUNC (Parser::parseM)
{
bool add = false;
bool take = false;
int i;
String dest = tokens.nextToken();
if(dest[0]=='#')
{ 
	if(!Channel::isChanRegistered(dest))
	{
		return;
	}
	String modes = tokens.nextColonToken();
	int length = modes.length();
	for (i = 0; i!=length; i++)
	{
		if (modes[i] == '+')
        	{
        		add = true;
        		take = false;
        	}
		if (modes[i] == '-')
        	{
        		add = false;
        		take = true;
        	}
		if(modes[i]=='b')   
		{
			if(add)
			{
				String target = tokens.nextToken();
				return;
			}
			if(take)
			{
				String target = tokens.nextToken();
				return;
			}
		}
		if(modes[i] == 'o')
		{
			if(add)
			{
				String target = tokens.nextToken();
				Channel::internalOp(target,dest);
			}
			if(take)
			{
				String target = tokens.nextToken();
				Channel::internalDeOp(target,dest);
			}
		}
		if(modes[i] == 'v')
		{
			if(add)
			{
				String target = tokens.nextToken();
				Channel::internalVoice(target,dest);
			}
			if(take)
			{
				String target = tokens.nextToken();
				Channel::internalDeVoice(target,dest);
			}
		}
	}
return;
}
String modes = tokens.nextColonToken();
int length = modes.length();
for (i = 0; i!=length; i++)
	{
		if (modes[i] == '+')
		{
			add = true;
			take = false;
		}
		if (modes[i] == '-')
		{
			add = false;
			take = true;
		}
		if(modes[i]=='o')
		{
			if(add)
			{
			//Active Oper? (hah :-)
			int axs = Nickname::getAccess("Oper",origin);
			if(axs==0)
			{
				//Non-Authorised.
				String tosend = origin+" just tried to become an IRC Operator - \002No Access\002";
				Services::globop(tosend,"Oper");
				String reason = "You have no permission to become an IRC Operator";
				Services::killnick(origin, "Oper", reason);
				return;
			}
			if(axs==-1)
			{
				String tosend = origin+" just tried to become an IRC Operator - \002Suspended\002";
				Services::globop(tosend,"Oper");
				String reason = "You are suspended - Do not try to become an Operator";
				Services::killnick(origin, "Oper", reason);
				return;
			}	
			if(axs>0)
			{
				String tosend = origin+" just became an IRC Operator - level "+String::convert(axs);
				Services::globop(tosend,"Oper");
				return;
				//Need to add to an online tables of opers..
			}
			}

		}
	}
}
/* Incoming Pass - Denotes we have traversed the ident check etc*/
void
PARSER_FUNC (Parser::parsePASS)
{
  long unsigned rx = Services::getCountRx();
  long unsigned tx = Services::getCountTx();
  String togo = String("Completed Network Synch: ")+String::convert(rx)+" Bytes received. "+String::convert(tx)+" Bytes transmitted";
  Services::helpme(String(togo),"Serv");
  return;
}

void
PARSER_FUNC (Parser::parsePART)
{
String channel = tokens.nextToken();
Channel::internalDel(origin,channel);
}

void
PARSER_FUNC (Parser::parseN)
{
if(tokens.countTokens() < 11)
{
/* Client Nickname Change */
	String newnick = tokens.nextToken();
	newnick = newnick.IRCtoLower();
	String query = "UPDATE onlineclients set nickname='" + newnick + "' where nickname='"+origin+"'";
	Sql::query(query);
	query = "DELETE from kills WHERE nick='"+origin+"'";
	Sql::query(query);
	if(Nickname::isNickRegistered(newnick))
	{
		if(!Nickname::isIdentified(newnick))
		{
			if(!Nickname::isPending(newnick))
			{
			/* Not identified as new nickname */
			Nickname::addCheckidentify(newnick);
			}
		}
	}
	return;
}
String nick = tokens.nextToken();
if(Nickname::isNickRegistered(nick))
{
	if(!Nickname::isPending(nick))
		Nickname::addCheckidentify(nick);
}

String hops = tokens.nextToken();
String timestamp = tokens.nextToken();
String modes = tokens.nextToken();
String username = tokens.nextToken();
String host = tokens.nextToken();
String vwhost = tokens.nextToken();
String server = tokens.nextToken();
//Don't want the next two.
tokens.nextToken();
tokens.nextToken();
String realname = tokens.rest();
Nickname::addClient(nick,hops,timestamp,username,host,vwhost,server,modes,realname);
int num = Nickname::countHost(host);
String query = "SELECT txt from news where level=0 AND expires<"+String::convert(Services::currentTime);
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
	String foo = ((std::string) row[0]).c_str();
	Services::serviceNotice("\002[\002IRCDome Global News\002]\002"+foo,"IRCDome",nick);
}
Services::queueAdd(":IRCDome WALLOPS :\002[\002Sign On\002]\002 "+nick+" ("+username+"@"+host+") ["+server+"]");
if(num>2)
{
	String alert = "\002Alert\002 excess connections from "+host+" - Latest client is "+nick+"!"+username+"@"+host+" - ("+String::convert(num)+")";
	Services::globop(alert,"Oper");
	//Add gline.
}

}
void
PARSER_FUNC (Parser::parsePRIVMSG)
{
  String originl = origin.IRCtoLower();
  String target = tokens.nextToken ();
  String message = tokens.nextColonToken();
  
  if ((message[0] == '\001') && (message[message.length () - 1] == '\001'))
    {
      // Break the message down
      StringTokens CTCPtokens (message.substr (1, message.length () - 2));
      String CTCPcommand = CTCPtokens.nextToken ().toUpper ();
      // Check what kind of CTCP this is
      if (CTCPcommand == "PING")
        {
          String data = CTCPtokens.rest ();
          Services::sendCTCPpingReply (target, origin, data);
          return;
        }
      if (CTCPcommand == "VERSION")
        {
          Services::sendCTCPversionReply (target, origin);
          return;
        }
      return;
    }
  if ((target.toLower ()) == "ircdome")
    {
	IRCDome::parseLine(message,originl);
	return;
    }
std::cout << target << std::endl;
  if(target[0]=='#')
	{
		//Oke channel msg :>
		if(!Services::serviceM.exists("chan"))
		{
			String togo = "Sorry - Channel services are currently disabled for maintenance";
			Services::serviceNotice(String(togo),target,originl);
			return;
		}
		Services::serviceM.throwLine("chan",message,originl,target);
		Services::serviceM.throwLine("game",message,originl,target);

	}
   //Hard check for nick if its @ircdome.org ......
   if(target.toLower()=="nick@ircdome.org")
	{
		Services::SecurePrivmsg = true;
		//Safety check for the module.. :-)
		if(!Services::serviceM.exists("nick"))
		{
			String togo = "Sorry - This part of services is currently undergoing construction";
			Services::serviceNotice(String(togo),target,originl);
		}
		Services::serviceM.throwLine("nick",message,originl);
		return;
	}
   Services::SecurePrivmsg = false;
   if(!Services::serviceM.exists(target.toLower()))
	{
		String togo = "Sorry - This part of Services is currently offline for maintenance - please try again later";
		Services::serviceNotice(String(togo),target,originl);
		return;
	}
   Services::serviceM.throwLine(target.toLower(),message,originl);
  

}

void
PARSER_FUNC (Parser::parsePING)
{
	String data = tokens.rest();
	Services::doPong(data);
}
void
PARSER_FUNC (Parser::parseSQUIT)
{
String server = tokens.nextToken();
String query = "SELECT id from onlineclients where server='"+server+"'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
	String idt = ((std::string) row[0]).c_str();
	res.free_result();
	int foo = idt.toInt();
	String qt = "DELETE from identified where nick='"+String::convert(foo)+"'";
	Sql::query(qt);
}
query = "DELETE from onlineclients where server='"+server+"'";
Sql::query(query);
}
void
PARSER_FUNC (Parser::parseQUIT)
{
String reason = tokens.nextColonToken();
int oid = Nickname::getOnlineNickID(origin);
String query = "DELETE from onlineclients where nickname='"+origin+"'";
Sql::query(query);
query = "DELETE from identified where nick='"+String::convert(oid)+"'";
Sql::query(query);
//Store the quit reason here
}

void
PARSER_FUNC (Parser::parseSJOIN)
{
	bool op = false;
	bool voice = false;
	bool normal = false;
	String ts1 = tokens.nextToken();
	String ts2 = tokens.nextToken();
	String chan = tokens.nextToken();
	String modes = tokens.nextToken();
	bool more;
	more = tokens.hasMoreTokens();
	/* Hrmmm..... :-) */
	if(!Channel::ifChanExists(chan.toLower()))
	{
		/* Doesn't exist.. add it to the table so it has an ID :-) */
		String query = "INSERT into onlinechan values ('','"+chan+"')";
		Sql::query(query);
		
	}
	while(more)
	{
		String user = tokens.nextToken();
		StringTokens luser (user);
		String foo = luser.nextColonToken();
		if(foo[0]=='@')
		{
			op = true;
			voice = false;
			normal = false;
		}
		if(foo[0]=='+')
		{
			op = false;
			voice = true;
			normal = false;
		}
		if(foo[0]!='@' && foo[0]!='+')
		{
			op = false;
			voice = false;
			normal = true;
		}
		if(op)
		{
			Channel::internalOp(foo.trim(),chan);
		}
		if(voice)
		{
			Channel::internalVoice(foo.trim(),chan);
		}
		if(normal)
		{
			if(Nickname::isIdentified(foo.trim(),foo.trim()))
			{
				int access = Channel::getChanAccess(chan,foo.trim());
				String togo = "Client: \002:"+foo.trim()+":<\002 Target:002:"+chan+":\002 Access :\002"+String::convert(access)+":\002";
				if(access>99)
				{
					Services::mode("Chan",chan,"+o",foo.trim());
					Channel::internalOp(foo.trim(),chan);
					return;
				}
				if(access>49)
				{
					Services::mode("Chan",chan,"+v",foo.trim());
					Channel::internalVoice(foo.trim(),chan);
					return;
				}
			}
		Channel::internalAdd(foo.trim(),chan);

		}
	more = tokens.hasMoreTokens();
	}
}
