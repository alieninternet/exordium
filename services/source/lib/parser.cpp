/* $Id$
 *
 * Exordium Network Services
 * Copyright (C) 2002 Exordium Development Team
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
/*
 *                   THIS  FILE  IS  TEMPORARY !!
 *
 *   The ultimate goal of linking with KineIRCd is to eliminate the
 *   focus on protocols. In fact, not even KineIRCd alone will know
 *   how to talk to other IRC servers - that is handled by protocol
 *   modules. Anything within this file should be considered a 'hack'
 *   until such time as KineIRCd has a suitable core and protocol
 *   module to carry on the work done here.
 *
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "parser.h"
#include <exordium/channel.h>
#include <exordium/server.h>
#include <aisutil/string.h>
#include <kineircd/config.h>

using AIS::Util::String;
using AIS::Util::StringTokens;
using namespace Exordium;

const struct Parser::functionTableStruct Parser::functionTable[] =
{
     { "301", &Parser::parseUSERAWAY },
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
     { "KICK", &Parser::parseKICK },
     { "KILL", &Parser::parseKILL },
     { "MOTD", &Parser::parseMOTD },
     { "VERSION", &Parser::parseMOTD },
     { "TOPIC", &Parser::parseTOPIC },
     { "GNOTICE", &Parser::parseGNOTICE },
     { 0, 0 }
};

void Parser::parseLine(const String& line)
{
   std::cout << "line() -> : " << line << std::endl;
   StringTokens st (line);
   Kine::Name origin = "";
#ifdef DEBUG
   std::cout << "DEBUG RX: " << line << std::endl;
#endif
   char c;
   c = line[0];
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
	     // Run the command and leave *dodgey*
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }

   services.logLine("Unparsed Input!: " + line,
		    Log::Debug);
};

void PARSER_FUNC (Parser::parseGNOTICE)
{
   // OLDorigin = server
   // txt = the rest.
   String txt = tokens.rest();

}

void PARSER_FUNC (Parser::parseAWAY)
{
   String reason = tokens.rest();
   User *origin = services.findUser(OLDorigin);
   if(origin == 0)
     {
	services.logLine("Error in Parser::parseAWAY - Null pointer for user " + OLDorigin);
	return;
     }
   services.getConfigInternal().getModules().handleAway(*origin,reason);
}
void
  PARSER_FUNC (Parser::parseS)
{
   String serverName = tokens.nextToken ();
   String hops = tokens.nextToken ();
   String description = tokens.rest();
   Server *ptr = services.addServer(serverName,hops.toInt(),description);
   if(ptr==0)
     {
	services.logLine("Error: Could not allocate record for new server : "+serverName);
	return;
     }
   if(!ptr->isAuthorised())
     {
	services.queueAdd(":" + Kine::config().getOptionsServerName() + 
			  " SQUIT " + serverName + 
			  " :Unauthorised links are not permitted here "
			  " - The network administration has been notified");
	return;
     }
   ptr->addOnlineServer();
   
}

void
  PARSER_FUNC (Parser::parseUSERAWAY)
{

/* THIS DOESNT DO ANYTHING, BUT NEEDED TO STOP MY CONSOLE FILLING UP WITH
   ANNOYING AWAY MESSAGES FROM CLIENTS *STARES AT PRAETORIAN*
 */
}

void
  PARSER_FUNC (Parser::parseM)
{
   bool add = false;
   bool take = false;
   int i;
   Kine::Name dest = tokens.nextToken();
   Kine::Name nick = OLDorigin;
   String currentmodes="";
   if(dest[0]=='#')
     {
	String modes = tokens.nextColonToken();
	dChan *dptr = services.findChan(dest);

        services.getConfigInternal().getModules().handleChannelMode( *dptr, modes, tokens.rest(), OLDorigin );

	return;
     }

   User *origin=services.findUser(OLDorigin);

   currentmodes=origin->getModes();

   String modes = tokens.nextColonToken();
   int length = modes.length();
   int pos;

   for (i = 0; i!=length; i++)
     {
	if (modes[i] == '+')
	  {
	     add = true;
	     take = false;
             continue;
	  }
	if (modes[i] == '-')
	  {
	     add = false;
	     take = true;
             continue;
	  }

        if(add)
	  {
	     pos = currentmodes.find(modes[i]);

	     // If mode is not already set
	     if(pos == -1)
	       currentmodes.push_back(modes[i]);
	  }
        else
	  if(take)
	    {
	       pos = currentmodes.find(modes[i]);

	       // If mode is already set
	       if(pos != -1)
		 currentmodes.erase(pos, 1);
	    }

	if(modes[i]=='o')
	  {
	     if(add)
	       {
		  services.validateOper(nick);
	       }
	     else if(take)
               {
		  if(services.isOper(OLDorigin))
		    services.delOper(OLDorigin);
               }

	  }
     }

   // Update modes in table onlineusers
   services.getDatabase().dbUpdate("onlineclients", "modes='"+currentmodes+"'", "id="+origin->getOnlineIDString());
}
/* Incoming Pass - Denotes we have traversed the ident check etc*/
void
  PARSER_FUNC (Parser::parsePASS)
{
   long unsigned rx = services.getCountRx();
   long unsigned tx = services.getCountTx();
   String togo = String("Completed Network Synch: ")+String::convert(rx)+" Bytes received. "+String::convert(tx)+" Bytes transmitted";
   services.sendGOper(Kine::config().getOptionsServerName(),togo);
   services.logLine(String(togo));
   return;
}

void
  PARSER_FUNC (Parser::parsePART)
{
   Kine::Name channel = tokens.nextToken();
   dChan *ptr = services.findChan(channel);
   User  *uptr = services.findUser(OLDorigin);

   services.getConfigInternal().getModules().handleChannelPart( *uptr, *ptr );

   ptr->delUser(*uptr);
   if(ptr->getCount()==0)
     {
        services.logLine(channel + " has 0 members.  Deleting");
	services.delChan(channel);
     }
}

void PARSER_FUNC (Parser::parseN)
{

   if(tokens.countTokens() < 9)
     {
/* Client Nickname Change */
	Kine::Name tempString = OLDorigin.IRCtoLower().trim();
        User *origin = services.findUser(tempString);
	if(origin==0)
	  {
	     services.logLine("Parser::parseN Something fucked going on.. can't find a struct for the user " + tempString);
	     services.logLine("Parser::parseN This is a bad error, should be able to find any user on a parm<11 count");
	     return;
	  }

        Kine::Name newnick=tokens.nextToken().trim();
        services.setNick(*origin, newnick);
        services.getDatabase().dbDelete("kills", "nick='"+OLDorigin+"'");
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
   Kine::Name nick = tokens.nextToken();
   String hops = tokens.nextToken();
   String timestamp = tokens.nextToken();
   String modes = tokens.nextToken();
   String username = tokens.nextToken();
   String host = tokens.nextToken();
   String server = tokens.nextToken();
   String blah1 = tokens.nextToken();
   String blah2 = tokens.nextToken();
   String realname = tokens.nextColonToken();
   std::cout << "\nNickname: " << nick 
     << "\nHops: " << hops 
     << "\nTimeStamp: " << timestamp
     << "\nModes: " << modes
     << "\nUserName: " << username
     << "\nHost: " << host
     << "\nServer: " << server
     << "\nBlah1: " << blah1
     << "\nBlah2: " << blah2
     << "\nRealName: " << realname
     << std::endl;
   User *newNick = services.addClient(nick, hops, timestamp, username, host,
				      server, modes, realname);
   if (newNick == 0)
     {
	services.logLine("Parser::ParseN couldn't create a new user record for ");
	services.logLine("Parser::ParseN "+nick+" "+hops+" "+timestamp+" "+username+" "+host+" "+server+" "+modes+" "+realname);
	return;
     }

   services.getConfigInternal().getModules().handleClientSignon(*newNick);

}
void
  PARSER_FUNC (Parser::parsePRIVMSG)
{
   User *origin = services.findUser(OLDorigin);

   // preserve sanity
   if (origin == 0)
     {
#ifdef DEBUG
	std::cout << "privmsg from non-user or something: " << OLDorigin <<
	  std::endl;
#endif
	return;
     }

   /* This user has sent a privmsg, increment flood counter. */
   std::cout << "Increasing " << origin->getNickname() << " floodcount" << std::endl;
   origin->incFloodCount();
   
   /* Now check the flood counter, and ignore if too high */
   if(origin->getFloodCount()>=70 && origin->getFloodCount()<80)
     {
	origin->sendMessage("You are being ignored by services for flooding, wait a while then try again",services.getConfigInternal().getConsoleName());
	return;
     }
   
   if(origin->getFloodCount()>80)
     {
	return;
     }
   
   String OLDoriginl = OLDorigin.IRCtoLower();
   Kine::Name target = tokens.nextToken ();
   String message(tokens.nextColonToken());

   if ((message[0] == '\001') && (message[message.length () - 1] == '\001'))
     {
	// Break the message down
	StringTokens CTCPtokens (message.substr (1, message.length () - 2));
	String CTCPcommand = CTCPtokens.nextToken ().toUpper ();
	// Check what kind of CTCP this is
	if (CTCPcommand == "PING")
	  {
	     services.queueAdd(":" + target + " NOTICE " + OLDorigin +
			       " :\001PING " + CTCPtokens.rest() + "\001");
	     return;
	  }
	if (CTCPcommand == "VERSION")
	  {
	     services.queueAdd(":" + target + " NOTICE " + OLDorigin +
			       " :\001VERSION Exordium Network Services "
			       "http://exordium.sourceforge.net/\001");
	     return;
	  }
	return;
     }
   if ((target.IRCtoLower ()) ==
       services.getConfigInternal().getConsoleName().IRCtoLower())
     {
	services.getConsole().parseLine(message,OLDoriginl);
	return;
     }
#ifdef DEBUG
   std::cout << '\'' << target << '\'' << std::endl;
#endif

   if(target.toLower()=="chan@" + Kine::config().getOptionsServerName())
     {
	if(!services.getConfigInternal().getModules().exists("chan"))
	  {
	     String togo = "Sorry - This part of services is currently undergoing maintenance";
	     services.serviceNotice(togo,target,OLDoriginl);
	  }
	std::cout << "Throwing this to CHAN: " << message << std::endl;
	StringTokens too(message);
	services.getConfigInternal().getModules().throwLine("chan",too,*origin,true);
	return;
     }

   if(target.toLower()=="nick@" + Kine::config().getOptionsServerName())
     {
	//Safety check for the module.. :-)
	if(!services.getConfigInternal().getModules().exists("nick"))
	  {
	     String togo = "Sorry - This part of services is currently undergoing construction";
	     services.serviceNotice(String(togo),target,OLDoriginl);
	  }
	StringTokens bloodydodgeytoo(message);
	services.getConfigInternal().getModules().throwLine("nick",
							    bloodydodgeytoo,
							    *origin,
							    true);
	return;
     }
   if(!services.getConfigInternal().getModules().exists(target))
     {
	services.serviceNotice("Sorry - This part of Services is currently "
			       "offline for maintenance - please try again "
			       "later",
			       Kine::config().getOptionsServerName(),
			       OLDoriginl);
	return;
     }
   StringTokens dodgeybutnotanymoredodgeythanthelastonewas(message);
   services.getConfigInternal().getModules().throwLine(target.toLower(),
						       dodgeybutnotanymoredodgeythanthelastonewas,
						       *origin,
						       false);

}

void
  PARSER_FUNC (Parser::parsePING)
{
   services.queueAdd(":" + Kine::config().getOptionsServerName() + " PONG " +
		     tokens.rest());
}

void
  PARSER_FUNC (Parser::parseSQUIT)
{
   String server = tokens.nextToken();
   services.getDatabase().dbDelete("nicksidentified", "USING nicksidentified,onlineclients nicksidentified.nick=onlineclients.id AND onlineclients.server='"+server+"'");
   services.getDatabase().dbDelete("onlineclients", "server='"+server+"'");
   services.delServer(server);
}
void
  PARSER_FUNC (Parser::parseQUIT)
{
   String reason = tokens.nextColonToken();
   int oid = services.locateID(OLDorigin);

   // Delete quitting user from oper list
   if(services.isOper(OLDorigin))
     services.delOper(OLDorigin);

   services.delUser(OLDorigin);

   services.getDatabase().dbDelete("nicksidentified", "nick='"+String::convert(oid)+"'");

   //Store the quit reason here...
   // but only if its registered!
   if(services.isNickRegistered(OLDorigin))
     {
	services.getDatabase().dbUpdate("nicks", "quitmsg='"+reason+"'", "nickname='"+OLDorigin+"'");
     }
}

void
  PARSER_FUNC (Parser::parseSJOIN)
{
   bool op = false;
   bool voice = false;
   bool normal = false;
   int  status = 0;
   String ts1 = tokens.nextToken();
   //String ts2 = tokens.nextToken();
   Kine::Name chan = tokens.nextToken();
   String modes = tokens.nextToken();
   bool more;
   more = tokens.hasMoreTokens();

   if(!services.getChannel().ifChanExists(chan.toLower()))
     {
        services.getDatabase().dbInsert("onlinechan", "'','"+chan+"'");
     }
   dChan *dptr = services.findChan(chan);
   if(dptr==0)
     {
	/* We have no record for this channel, make it. */
	std::cout << "Channel record being made for " << chan << std::endl;
	dptr = services.addChan(chan.IRCtoLower(),services.getOnlineChanID(chan));
     }

   std::cout << "parseSJOIN() - " << ts1 << " : " << chan << " : " << modes <<std::endl;
   std::cout << "parseSJOIN() - " << OLDorigin << std::endl;
   User *musr = services.findUser(OLDorigin);

   if(musr==0)
     {
	// Do nothing
	//
     }
   else
     {

        if(!more)
	  services.getConfigInternal().getModules().handleChannelJoin( *musr, *dptr, 0 );

     }

   while(more)
     {
	String user = tokens.nextToken();
	StringTokens luser (user);
	String foo = luser.nextColonToken();
	Kine::Name username = foo.trim();
	status = 0;
	std::cout << "parseSjoin() " << user << " : " << foo << std::endl;
        // First check if both @ and + are there
        if ((username[1]=='@') || (username[1]=='+'))
	  username=username.substr(2, username.length()-2);
        else
	  {
	     // Strip leading @ or + if there is one
	     if ((username[0]=='@') || (username[0]=='+'))
	       username=username.substr(1, username.length()-1);
	  }

        User *ptr = services.findUser(username);
	if(ptr==0)
	  {
	     std::cout << "User doesn't exist (" << username << ") aborting" << std::endl;
	     return;
	  }
	std::cout << "Ok! found the user" << std::endl;
        if(foo[0]=='@' && foo[1]=='+')
	  {
	     op = true;
	     voice = true;
	     normal = false;
	  }
	else if(foo[0]=='@' && foo[1]!='+')
	  {
	     op = true;
	     voice = false;
	     normal = false;
	  }
	else if(foo[0]=='+' && foo[1]=='@')
	  {
	     op = true;
	     voice = true;
	     normal = false;
	  }
	else if(foo[0]=='+' && foo[1]!='@')
	  {
	     op = false;
	     voice = true;
             normal = false;
          }
	else if(foo[0]!='@' && foo[0]!='+')
	  {
	     op = false;
	     voice = false;
	     normal = true;
	  }
	else
	  {
	     op = false;
	     voice = false;
	     normal = true;
	  }

	/* Voice = 1 | Op = 2 */
	if(op)
	  {
	     status = status +2;
                /* Safety Check......... */
	     dptr->addUser(*ptr,status);

	  }
	if(voice)
	  {
	     status++;
                /* Safety Check......... */
	     dptr->addUser(*ptr,status);

	  }
	if(normal)
	  {
                /* Safety Check......... */
	     dptr->addUser(*ptr,status);
          }

	services.getConfigInternal().getModules().handleChannelJoin( *ptr, *dptr, status );

     }

   more = tokens.hasMoreTokens();

}

void
  PARSER_FUNC (Parser::parseKICK)
{

}

void
  PARSER_FUNC (Parser::parseKILL)
{
   Kine::Name who = tokens.nextToken();
   String reason = tokens.rest();
   int oid = services.locateID(who);
   if(services.isOper(who))
     services.delOper(who);

   services.delUser(who);

   services.getDatabase().dbDelete("nicksidentified","nick='"+String::convert(oid)+"'");
   if(services.isNickRegistered(who))
     services.getDatabase().dbUpdate("nicks","quitmsg='"+reason+"'", "nickname='"+who+"'");

}

// Return something to the user who does VERSION or MOTD on us.
void
  PARSER_FUNC (Parser::parseMOTD)
{
   User *origin = services.findUser(OLDorigin);

   origin->sendMessage("Exordium Network Services - (c)2002,2003 Exordium Development Team",
		       Kine::config().getOptionsServerName(),
		       false);
   origin->sendMessage("Please see http://sf.net/projects/exordium for details on the project",Kine::config().getOptionsServerName(),false);
}

void
  PARSER_FUNC (Parser::parseTOPIC)
{
   // OLDorigin can be either a user or a server so we dont consider it,
   // the user is given later
   //
   Kine::Name channel = tokens.nextToken();
   String source = tokens.nextToken();

   // Skip timestamp
   tokens.nextToken();

   String newTopic = tokens.nextColonToken();

   dChan *chan = services.findChan( channel );

   if( chan == 0 )
     {
	services.logLine("Error in Parser::parseTOPIC - Null pointer for channel " + channel);
	return;
     }

   // We only care if the channel is registered
   if( chan->isRegistered() )
     services.getConfigInternal().getModules().handleTopic( source, *chan, newTopic );
}
