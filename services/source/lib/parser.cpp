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
//#include "exordium/dchan.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include <kineircd/config.h>

using AISutil::String;
using AISutil::StringTokens;
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
     { "KICK", &Parser::parseKICK },
     { "KILL", &Parser::parseKILL },
     { "MOTD", &Parser::parseMOTD },
     { "VERSION", &Parser::parseMOTD },
     { 0, 0 }
};

void Parser::parseLine(const String& line)
{
   StringTokens st (line);
   String origin = "";
   std::cout << "DEBUG RX: " << line << std::endl;
   char c;
   c = line[0];
   std::cout << "The Ascii Value of " << c << " is " << (int)c << std::endl;
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
   //
   services.logLine("Unparsed Input!: " + line,
		    Log::Debug);
};

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
   if (!services.isAuthorised (serverName))
     {
	services.queueAdd(":" + Kine::config().getOptionsServerName() +
			  " SQUIT " + serverName +
			  " :Unauthorised Links are not permitted on PeopleChat "
			  "- The network administration has been notified");
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
   String currentmodes="";
   if(dest[0]=='#')
     {
	String modes = tokens.nextColonToken();
	int length = modes.length();
	dChan *dptr = services.findChan(dest);
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

/*
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
*/

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
		       User *ptr = services.findUser(target);
		       dptr->addUser(*ptr,2);
		    }
		  if(take)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       User *ptr = services.findUser(target);
		       dptr->addUser(*ptr,0);
		    }
	       }
	     if(modes[i] == 'v')
	       {
		  if(add)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       User *ptr = services.findUser(target);
		       dptr->addUser(*ptr,1);
		    }
		  if(take)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       User *ptr = services.findUser(target);
		       dptr->addUser(*ptr,0);
		    }
	       }
	  }

        // Update modes in table chans
        //services.getDatabase().dbUpdate("chans", "modes='"+currentmodes+"'", "id="+channel.getChanIDString(dest));

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
		  services.validateOper(OLDorigin);
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
   services.sendGOper("services.peoplechat.org",togo);
   services.logLine(String(togo));
   return;
}

void
  PARSER_FUNC (Parser::parsePART)
{
   String channel = tokens.nextToken();
   dChan *ptr = services.findChan(channel);
   User  *uptr = services.findUser(OLDorigin);
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
	String tempString = OLDorigin.trim().IRCtoLower();
        User *origin = services.findUser(tempString);
	if(origin==0)
	  {
	     services.logLine("Parser::parseN Something fucked going on.. can't find a struct for the user " + tempString);
	     services.logLine("Parser::parseN This is a bad error, should be able to find any user on a parm<11 count");
	     return;
	  }

        String newnick=tokens.nextToken().trim();
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
   String nick = tokens.nextToken();
   String hops = tokens.nextToken();
   String timestamp = tokens.nextToken();
   String modes = tokens.nextToken();
   String username = tokens.nextToken();
   String host = tokens.nextToken();
   String vwhost = tokens.nextToken();
   String server = tokens.nextToken();
   (void)tokens.nextToken();
   (void)tokens.nextToken();
   String realname = tokens.nextColonToken();
   User *newNick = services.addClient(nick, hops, timestamp, username, host,
				      vwhost, server, modes, realname);
   if (newNick == 0)
     {
	services.logLine("Parser::ParseN couldn't create a new user record for ");
	services.logLine("Parser::ParseN "+nick+" "+hops+" "+timestamp+" "+username+" "+host+" "+vwhost+" "+server+" "+modes+" "+realname);
	return;
     }

   services.getConfigInternal().getModules().handleClientSignon(*newNick);
/*   if (modes.find("o"))
     services.validateOper(nick);
*/

   int num = newNick->countHost();

   //int nbRes = services.getDatabase().dbSelect("txt", "news", "level=0 AND expires<"+String::convert(services.currentTime));
   //for (int i=0; i<nbRes; i++)
   //{
   //   newNick->sendMessage("\002[\002PeopleChat Global News\002]\002 "+ services.getDatabase().dbGetValue(), services.getConfigInternal().getConsoleName());
   //   services.getDatabase().dbGetRow();
   //}

   if(num>2)
     {
	String alert = "\002Alert\002 excess connections from "+host+" - Latest client is "+nick+"!"+username+"@"+host+" - ("+String::convert(num)+")";
	services.sendGOper("Oper",alert);
	//Add gline.
     }

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
   if(target[0]=='#')
     {
	//Oke channel msg :>
	if(!services.getConfigInternal().getModules().exists("chan"))
	  {
	     String togo = "Sorry - Channel services are currently disabled for maintenance";
	     services.serviceNotice(String(togo),target,OLDoriginl);
	     return;
	  }

	// dodgey? YES.. YES IT IS.. :(
	//	services.getConfigInternal().getModules().throwLine("chan",message,OLDoriginl,target);
	//	services.getConfigInternal().getModules().throwLine("game",message,OLDoriginl,target);
	StringTokens dodgeydodgeydodgey(message);
	services.getConfigInternal().getModules().throwLine("chan", dodgeydodgeydodgey, *origin,
							    target);
	StringTokens dodgeydodgeydodgeyAGAIN(message);
	services.getConfigInternal().getModules().throwLine("game", dodgeydodgeydodgeyAGAIN, *origin,
							    target);
	return;
     }
   if(target.toLower()=="chan@services.peoplechat.org")
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

   //Hard check for nick if its @ircdome.org ......
   if(target.toLower()=="nick@services.peoplechat.org")
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
   services.getDatabase().dbDelete("nicksidentified", "identified.nick=onlineclients.id AND onlineclients.server='"+server+"'");
   services.getDatabase().dbDelete("onlineclients", "server='"+server+"'");
   services.DelOnlineServer(server);
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
   String chan = tokens.nextToken();
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

	if(musr->isIdentified(musr->getNickname()))
	  {
	     if(dptr->getAccess(OLDorigin)>99)
	       {
		  dptr->mode("Chan","+o",OLDorigin);
		  dptr->addUser(*musr,2);
		  dptr->log(*musr,"Chan","Autoopped on join",chan);
		  return; // break
	       }

	  }
     }

   while(more)
     {
	String user = tokens.nextToken();
	StringTokens luser (user);
	String foo = luser.nextColonToken();
	String username = foo.trim();
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

        /******* NOTE
         Bug fixed: leading @ or + was causing user not found errors.
         Solution: foo is only used to toggle the booleans, username
                   contains the nick without leading @ or +.
                   Thus when we need to pass the nick we use username.
         -- PLV
        *******/

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
	        /* Now lets see if they want opping.... */
	     if(dptr->getAccess(ptr->getNickname())>99)
	       {
		  /* But only if they are identified.. nearly forgot this one  */
		  if(ptr->isIdentified(ptr->getNickname()))
		    {

		       dptr->mode("Chan","+o",ptr->getNickname());
		       dptr->addUser(*ptr,2);
		    }

	       }

	  }
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
   String who = tokens.nextToken();
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

