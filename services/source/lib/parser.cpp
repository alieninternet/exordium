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

#include "exordium/parser.h"
#include "exordium/services.h"
#include "exordium/channel.h"
#include "exordium/log.h"
#include <kineircd/str.h>
#include "exordium/sql.h"
#include "exordium/ircdome.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

const struct Parser::functionTableStruct Parser::functionTable[] =
{
     { "PART", &Parser::parsePART },
     { "MODE", &Parser::parseM },
     { "PASS", &Parser::parsePASS },
     { "PRIVMSG", &Parser::parsePRIVMSG },
     { "SERVER", &Parser::parseS },
     { "NICK", &Parser::parseN },
     { "PING", &Parser::parsePING },
     { "QUIT", &Parser::parseQUIT },
     { "SQUIT", &Parser::parseSQUIT },
     { "SJOIN", &Parser::parseSJOIN },
     { "AWAY", &Parser::parseAWAY },
     { 0, 0 }
};

void Parser::parseLine(const String& line)
{
   StringTokens st (line);
   String origin = "";

   if (line[0] == ':')
     {
	// Skip the first parameter, we do not care about it anyway
	origin = st.nextToken ().substr (1);
	origin = origin.IRCtoLower();
     }
   //Find the associated user for this origin...
//   User *ptr = services.findUser(origin);
   String command = st.nextToken ();
//   if (ptr != 0) {
      for (int i = 0; functionTable[i].command != 0; i++)
	{
	   // Does this match?
	   if (command == functionTable[i].command)
	     {
	     // Run the command and leave *dodgey*
		(this->*(functionTable[i].function))(origin, st);
		return;
	     }
	}
  // }

   services.getLogger().logLine(String("Unparsed Input!: ")+line);
};

void PARSER_FUNC (Parser::parseAWAY)
{
   String reason = tokens.rest();
   User *origin = services.findUser(OLDorigin);
   if(origin == 0)
     {
	std::cout << "Our pointer is null." << std::endl;
//	exit(0);
	return;
     }
 
   if(origin->deopAway())
     {
	if(reason=="")
	  {
	     //Don't do anything user is coming back
	     return;
	  }
	else
	  {
	     String query = "SELECT chanid from chanstatus where nickid="+origin->getOnlineIDString()+" AND status=2";
	     MysqlRes res = services.getDatabase().query(query);
	     MysqlRow row;
	     while ((row = res.fetch_row()))
	       {
		  String foo = ((std::string) row[0]).c_str();
		  String cname = services.getChannel().getChanIDName(foo.toInt());
		  String cstr = OLDorigin+" "+OLDorigin;
		  services.serverMode(cname,"-o+v",cstr);
		  services.getChannel().internalVoice(OLDorigin,cname);
		  services.getChannel().internalDeOp(OLDorigin,cname);
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
   if (!services.isAuthorised (serverName))
     {
	String tosend =
	  String (":services.ircdome.org SQUIT ") + serverName +
	  " :Unauthorised Links are not permitted on IRCDome - The network administration has been notified";
	services.queueAdd (String (tosend));
	return;
     }
   services.AddOnlineServer (serverName, hops, description);
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
//	if(!services.getChannel().isChanRegistered(dest))
//	  {
//	     return;
//	  }
// I decided that chanstatus should have info on ALL channels....
// 
// 
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
		       if(target.toLower()=="chan")
			 return;
		       services.getChannel().internalOp(target.toLower(),dest);
		    }
		  if(take)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       services.getChannel().internalDeOp(target.toLower(),dest);
		    }
	       }
	     if(modes[i] == 'v')
	       {
		  if(add)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       services.getChannel().internalVoice(target.toLower(),dest);
		    }
		  if(take)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       services.getChannel().internalDeVoice(target.toLower(),dest);
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
		  User *ptr = services.findUser(OLDorigin);
		  int axs = ptr->getAccess("Oper");
		  if(axs==0)
		    {
		       //Non-Authorised.
		       String tosend = OLDorigin+" just tried to become an IRC Operator - \002No Access\002";
		       services.globop(tosend,"Oper");
		       String reason = "You have no permission to become an IRC Operator";
		       services.killnick(OLDorigin, "Oper", reason);
		       return;
		    }
		  if(axs==-1)
		    {
		       String tosend = OLDorigin+" just tried to become an IRC Operator - \002Suspended\002";
		       services.globop(tosend,"Oper");
		       String reason = "You are suspended - Do not try to become an Operator";
		       services.killnick(OLDorigin, "Oper", reason);
		       return;
		    }
		  if(axs>0)
		    {
		       String tosend = OLDorigin+" just became an IRC Operator - level "+String::convert(axs);
		       services.globop(tosend,"Oper");
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
   long unsigned rx = services.getCountRx();
   long unsigned tx = services.getCountTx();
   String togo = String("Completed Network Synch: ")+String::convert(rx)+" Bytes received. "+String::convert(tx)+" Bytes transmitted";
   services.helpme(String(togo),"Serv");
   return;
}

void
  PARSER_FUNC (Parser::parsePART)
{
   String channel = tokens.nextToken();
   services.getChannel().internalDel(OLDorigin,channel);
}

void PARSER_FUNC (Parser::parseN)
{
   User *origin = services.findUser(OLDorigin);
   
   if(tokens.countTokens() < 11)
     {
/* Client Nickname Change */
	origin->setNick(tokens.nextToken());
	String query = "DELETE from kills WHERE nick='"+OLDorigin+"'";
	services.getDatabase().query(query);
	if(services.isNickRegistered(origin->getNickname()))
	  {
	     if(!origin->isIdentified(origin->getNickname()))
	       {
		  if(!origin->isPending())
		    {
			/* Not identified as new nickname */
		       /* Added this for raff. */
		       /* He's an annoying little pratt isn't he?
			* But somewhat loveable ;) - simon
			*/
		       if(origin->modNick())
			 {
			    
		       origin->addCheckIdentify();
			 }
		       
		    }
	       }
	  }
	return;
     }
   String nick = tokens.nextToken();
   std::cout << "NEW CLIENT:" << nick << std::endl;

   String hops = tokens.nextToken();
   String timestamp = tokens.nextToken();
   String modes = tokens.nextToken();
   String username = tokens.nextToken();
   String host = tokens.nextToken();
   String vwhost = tokens.nextToken();
   String server = tokens.nextToken();

  //Don't want the next two.
   (void)tokens.nextToken();
   (void)tokens.nextToken();
   
   String realname = tokens.rest();
   
//   services.getNickname().addClient(nick,hops,timestamp,username,host,vwhost,server,modes,realname);
   
   User *newNick = services.addClient(nick, hops, timestamp, username, host,
				      vwhost, server, modes, realname);

   if (newNick == 0) {
      std::cout << "That client wasn't such a nice fellow afterall :(" << 
	std::endl;
      return;
   }
   
   if(services.isNickRegistered(nick))
     {
	if(!newNick->isPending())
	  {
	     if(newNick->modNick())
	       {
		  
		  newNick->addCheckIdentify();
	       }
	     
	  }
	
     }

   
   int num = newNick->countHost();
   String query = "SELECT txt from news where level=0 AND expires<"+String::convert(services.currentTime);
   MysqlRes res = services.getDatabase().query(query);
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	String foo = ((std::string) row[0]).c_str();
	services.serviceNotice("\002[\002IRCDome Global News\002]\002"+foo,"IRCDome",nick);
     }
   services.queueAdd(":IRCDome WALLOPS :\002[\002Sign On\002]\002 "+nick+" ("+username+"@"+host+") ["+server+"]");
   if(num>2)
     {
	String alert = "\002Alert\002 excess connections from "+host+" - Latest client is "+nick+"!"+username+"@"+host+" - ("+String::convert(num)+")";
	services.globop(alert,"Oper");
	//Add gline.
     }

}
void
  PARSER_FUNC (Parser::parsePRIVMSG)
{
   User *origin = services.findUser(OLDorigin);

   // preserve sanity
   if (origin == 0) {
      std::cout << "privmsg from non-user or something: " << OLDorigin << 
	std::endl;
      return;
   }
   
   String OLDoriginl = OLDorigin.IRCtoLower();
   String target = tokens.nextToken ();
   
   // fix me. urgh.
   String message(tokens.nextColonToken());

   if ((message[0] == '\001') && (message[message.length () - 1] == '\001'))
     {
	// Break the message down
	StringTokens CTCPtokens (message.substr (1, message.length () - 2));
	String CTCPcommand = CTCPtokens.nextToken ().toUpper ();
	// Check what kind of CTCP this is
	if (CTCPcommand == "PING")
	  {
	     String data = CTCPtokens.rest ();
	     services.sendCTCPpingReply (target, OLDorigin, data);
	     return;
	  }
	if (CTCPcommand == "VERSION")
	  {
	     services.sendCTCPversionReply (target, OLDorigin);
	     return;
	  }
	return;
     }
   if ((target.toLower ()) == "ircdome")
     {
	services.getIRCDome().parseLine(message,OLDoriginl);
	return;
     }
   std::cout << target << std::endl;
   if(target[0]=='#')
     {
	//Oke channel msg :>
	if(!services.serviceM.exists("chan"))
	  {
	     String togo = "Sorry - Channel services are currently disabled for maintenance";
	     services.serviceNotice(String(togo),target,OLDoriginl);
	     return;
	  }
	
	// dodgey? YES.. YES IT IS.. :(
//	services.serviceM.throwLine("chan",message,OLDoriginl,target);
//	services.serviceM.throwLine("game",message,OLDoriginl,target);
	StringTokens dodgeydodgeydodgey(message);
	services.serviceM.throwLine("chan", dodgeydodgeydodgey, *origin,
				    target);
	StringTokens dodgeydodgeydodgeyAGAIN(message);
	services.serviceM.throwLine("game", dodgeydodgeydodgeyAGAIN, *origin,
				    target);

     }
   //Hard check for nick if its @ircdome.org ......
   if(target.toLower()=="nick@ircdome.org")
     {
	services.SecurePrivmsg = true;
	//Safety check for the module.. :-)
	if(!services.serviceM.exists("nick"))
	  {
	     String togo = "Sorry - This part of services is currently undergoing construction";
	     services.serviceNotice(String(togo),target,OLDoriginl);
	  }
	StringTokens bloodydodgeytoo(message);
	services.serviceM.throwLine("nick", bloodydodgeytoo, *origin);
	return;
     }
   services.SecurePrivmsg = false;
   if(!services.serviceM.exists(target.toLower()))
     {
	String togo = "Sorry - This part of Services is currently offline for maintenance - please try again later";
	services.serviceNotice(String(togo),target,OLDoriginl);
	return;
     }
   StringTokens dodgeybutnotanymoredodgeythanthelastonewas(message);
   services.serviceM.throwLine(target.toLower(),
			       dodgeybutnotanymoredodgeythanthelastonewas,
			       *origin);

}

void
  PARSER_FUNC (Parser::parsePING)
{
   String data = tokens.rest();
   services.doPong(data);
}
void
  PARSER_FUNC (Parser::parseSQUIT)
{
   String server = tokens.nextToken();
   String query = "SELECT id from onlineclients where server='"+server+"'";
   MysqlRes res = services.getDatabase().query(query);
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	String idt = ((std::string) row[0]).c_str();
	res.free_result();
	int foo = idt.toInt();
	String qt = "DELETE from identified where nick='"+String::convert(foo)+"'";
	services.getDatabase().query(qt);
     }
   query = "DELETE from onlineclients where server='"+server+"'";
   services.getDatabase().query(query);
}
void
  PARSER_FUNC (Parser::parseQUIT)
{
   String reason = tokens.nextColonToken();
   int oid = services.locateID(OLDorigin);
   String query = "DELETE from onlineclients where nickname='"+OLDorigin+"'";
   services.getDatabase().query(query);
   query = "DELETE from identified where nick='"+String::convert(oid)+"'";
   services.getDatabase().query(query);
   //Store the quit reason here
   query = "UPDATE nicks set quitmsg='" + reason + "' where nickname='"+OLDorigin+"'";
   services.getDatabase().query(query);
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
   if(!services.getChannel().ifChanExists(chan.toLower()))
     {
		/* Doesn't exist.. add it to the table so it has an ID :-) */
	String query = "INSERT into onlinechan values ('','"+chan+"')";
	services.getDatabase().query(query);

     }
   while(more)
     {
	String user = tokens.nextToken();
	StringTokens luser (user);
	String foo = luser.nextColonToken();
	String username = foo.trim();

        // Strip leading @ if there is one
        if (username[0]=='@')
           username=username.substr(1, username.length()-1);



        /******* NOTE
         Bug fixed: leading @ was causing user not found errors.
         Solution: foo is only used to toggle the booleans, username 
                   contains the nick without leading # or +.
                   Thus when we need to pass the nick we use username.
         -- PLV
        *******/


        User *ptr = services.findUser(username);

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
	     services.getChannel().internalOp(username,chan);
	  }
	if(voice)
	  {
	     services.getChannel().internalVoice(username,chan);
	  }
	if(normal)
	  {
	     if(ptr == 0)
	       {
		  return;
	       }
	     
	     if(ptr->isIdentified(foo))
	       {
		  int access = services.getChannel().getChanAccess(chan,username);
		  String togo = "Client: \002:"+username+":<\002 Target:002:"+chan+":\002 Access :\002"+String::convert(access)+":\002";
		  if(access>99)
		    {
		       services.mode("Chan",chan,"+o",username);
		       services.getChannel().internalOp(username,chan);
		       return;
		    }
		  if(access>49)
		    {
		       services.mode("Chan",chan,"+v",username);
		       services.getChannel().internalVoice(username,chan);
		       return;
		    }
	       }
	     services.getChannel().internalAdd(username,chan);

	  }
	more = tokens.hasMoreTokens();
     }
}
