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

#include "exordium/parser.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include "exordium/services.h"
#include "exordium/config.h"
#include "exordium/user.h"
#include "exordium/regchannel.h"
#include "exordium/reguser.h"
#include "exordium/database/database.h"

#include "kineircd/config.h"


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

   servicesFwd.logLine("Unparsed Input!: " + line,
		    Log::Debug);
};

void PARSER_FUNC (Parser::parseAWAY)
{
   String reason = tokens.rest();
   User *origin = userListFwd.getUser(OLDorigin);
   if(origin == 0)
     {
#ifdef DEBUG
	std::cout << "Our pointer is null." << std::endl;
#endif
	return;
     }
 
   /* From here on i'm fiddling with away */
   configFwd.getModules().handleAway(*origin,reason);

}
void
  PARSER_FUNC (Parser::parseS)
{
   // this should all be in the 'serv' module :(   - pickle
   String serverName = tokens.nextToken ();
   String hops = tokens.nextToken ();
   String description = tokens.rest();
   if (!servicesFwd.isAuthorised (serverName)) {
      servicesFwd.queueAdd(":" + Kine::config().getOptionsServerName() + 
			" SQUIT " + serverName + 
			" :Unauthorised Links are not permitted on IRCDome "
			"- The network administration has been notified");
      return;
   }
   servicesFwd.AddOnlineServer (serverName, hops, description);
}

/* NOTE: dont forget to uncomment after Channel is finished */

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
		       Channel *chan=channelListFwd.getChan(dest);

                       if(chan!=NULL)
                       {
                         if(chan->isInChannel(dest))
                            chan->setUserStatusOp(dest, true);
                         else
                         {
                            chan->addUser(dest);
                            chan->setUserStatusOp(dest, true);
                         }
                       }
		    }
		  if(take)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;
		       
                       Channel *chan=channelListFwd.getChan(dest);

                       if(chan!=NULL)
                       {
                         if(chan->isInChannel(dest))
                           chan->setUserStatusOp(dest, false);
                       }
		    }
	       }
	     if(modes[i] == 'v')
	       {
		  if(add)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;

                       Channel *chan=channelListFwd.getChan(dest);

                       if(chan!=NULL)
                       {
                         if(chan->isInChannel(dest))
                            chan->setUserStatusVoice(dest, true);
                         else
                         {
                            chan->addUser(dest);
                            chan->setUserStatusVoice(dest, true);
                         }
                       }
		    }
		  if(take)
		    {
		       String target = tokens.nextToken();
		       if(target.toLower()=="chan")
			 return;

                       Channel *chan=channelListFwd.getChan(dest);

                       if(chan!=NULL)
                       {
                         if(chan->isInChannel(dest))
                            chan->setUserStatusVoice(dest, false);
                       }
		    }
	       }
	  }
       
        // Update modes in table chans
	return;
     }

   User *origin=userListFwd.getUser(OLDorigin);

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


// Mode o not functionnal yet        
/*
	if(modes[i]=='o')
	  {
	     if(add)
	       {
                   servicesFwd.validateOper(OLDorigin);
	       }
               else if(take)
               {
                   if(servicesFwd.isOper(OLDorigin))
                     servicesFwd.delOper(OLDorigin);
#ifdef DEBUG
                   else
                     std::cout << "Warning: inconsistency in parsem: oper is not in onlineopers!" << std::endl;
#endif

               }

	  }
*/
     }


   origin->setModes(currentmodes);
}
/* Incoming Pass - Denotes we have traversed the ident check etc*/
void
  PARSER_FUNC (Parser::parsePASS)
{
   long unsigned rx = servicesFwd.getCountRx();
   long unsigned tx = servicesFwd.getCountTx();
   String togo = String("Completed Network Synch: ")+String::convert(rx)+" Bytes received. "+String::convert(tx)+" Bytes transmitted";
   servicesFwd.logLine(String(togo));
   return;
}

void
  PARSER_FUNC (Parser::parsePART)
{
   String channel = tokens.nextToken();

   Channel *chan=channelListFwd.getChan(channel);

   if(chan!=NULL)
   {
      chan->delUser(OLDorigin);

      if(chan->getNbUsers()<1)
        channelListFwd.delChan(channel);
   }

}

void PARSER_FUNC (Parser::parseN)
{
   
   if(tokens.countTokens() < 11)
     {
/* Client Nickname Change */
	String tempString = OLDorigin.trim().IRCtoLower();	
        User *origin = userListFwd.getUser(tempString);
	if(origin==0)
	  {
#ifdef DEBUG
	     std::cout << "ParseN:  could not find the user named " << OLDorigin << std::endl;
	     std::cout << "ParseN:  this is a fatal error - bailing ;-)" << std::endl;
#endif
	     exit(1);
	  }
	
        String newnick=tokens.nextToken().trim();
#ifdef DEBUG
	std::cout << "ParseN:" << newnick << std::endl;
#endif
        origin->setNick(newnick); 
        databaseFwd.dbDelete("kills", "nick='"+OLDorigin+"'");
	if(regUserListFwd.isRegistered(origin->getNick()))
	  {
#ifdef DEBUG
	     std::cout << "Nick is registered" << std::endl;
#endif
	     if(!origin->isIdentified())
	       {
#ifdef DEBUG
		  std::cout << "it isn't identified" << std::endl;
#endif
		  if(!origin->isPending(newnick))
		    {
#ifdef DEBUG
		       std::cout << "its not pending" << std::endl;
#endif
			/* Not identified as new nickname */
		       /* Added this for raff. */
		       /* He's an annoying little pratt isn't he?
			* But somewhat loveable ;) - simon
			*/
                       RegisteredUser *reggeduser=regUserListFwd.getRegisteredNick(newnick);

                       if(reggeduser!=NULL)
                       {
                         if(reggeduser->getModnick())
			 {
#ifdef DEBUG
			    std::cout << "they want modnick :-)" << std::endl;
#endif
		       	   origin->addPendingNick(newnick);
			 }
#ifdef DEBUG
                         else
			 {
			    std::cout << "they dont want modnick" << std::endl;
			 }
#endif
                       }
		       
		    }
#ifdef DEBUG
		  else
		    {
		       std::cout << "its pending status" << std::endl;
		    }
#endif
		  
	       }
#ifdef DEBUG
	     else
	       {
		  std::cout << "nick already identified" << std::endl;
	       }
#endif
	     
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
   String realname = tokens.rest();

   User *newNick=new User();

   newNick->setNick(nick);
   newNick->setHops(hops.toInt());
   newNick->setHostname(host);
   newNick->setTimestamp(timestamp);
   newNick->setUsername(username);
   newNick->setVwhost(vwhost);
   newNick->setServer(server);
   newNick->setModes(modes);
   newNick->setRealname(realname);

   userListFwd.addUser(newNick);

   configFwd.getModules().handleClientSignon(*newNick);


/*   if (modes.find("o"))
     servicesFwd.validateOper(nick);
*/

/*   
*   int num = newNick->countHost();
*
*   int nbRes = servicesFwd.getDatabase().dbSelect("txt", "news", "level=0 AND expires<"+String::convert(services.currentTime));
*
*   // NOTE: hardcoded bot nick?
*   for (int i=0; i<nbRes; i++)
*   {
*      newNick->sendMessage("\002[\002IRCDome Global News\002]\002 "+ servicesFwd.getDatabase().dbGetValue(), services.getConfigInternal().getConsoleName());
*      servicesFwd.getDatabase().dbGetRow();
*   }
*
*   servicesFwd.queueAdd(":IRCDome WALLOPS :\002[\002Sign On\002]\002 "+nick+" ("+username+"@"+host+") ["+server+"]");
*   if(num>2)
*     {
*	String alert = "\002Alert\002 excess connections from "+host+" - Latest client is "+nick+"!"+username+"@"+host+" - ("+String::convert(num)+")";
*	servicesFwd.globop(alert,"Oper");
*	//Add gline.
*     }
*/
}
void
  PARSER_FUNC (Parser::parsePRIVMSG)
{
   User *origin = userListFwd.getUser(OLDorigin);

   // preserve sanity
   if (origin == 0) {
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
	     String data = CTCPtokens.rest ();
	     servicesFwd.sendCTCPpingReply (target, OLDorigin, data);
	     return;
	  }
	if (CTCPcommand == "VERSION")
	  {
	     servicesFwd.sendCTCPversionReply (target, OLDorigin);
	     return;
	  }
	return;
     }
   if ((target.IRCtoLower ()) == 
       configFwd.getConsoleName().IRCtoLower())
     {
	servicesFwd.getConsole().parseLine(message,OLDoriginl);
	return;
     }
#ifdef DEBUG
   std::cout << '\'' << target << '\'' << std::endl;
#endif
   if(target[0]=='#')
     {
	//Oke channel msg :>
	if(!configFwd.getModules().exists("chan"))
	  {
	     String togo = "Sorry - Channel servicesFwd.are currently disabled for maintenance";
	     servicesFwd.serviceNotice(String(togo),target,OLDoriginl);
	     return;
	  }
	
	// dodgey? YES.. YES IT IS.. :(
//	servicesFwd.getModules().throwLine("chan",message,OLDoriginl,target);
//	servicesFwd.getModules().throwLine("game",message,OLDoriginl,target);
	StringTokens dodgeydodgeydodgey(message);
	configFwd.getModules().throwLine("chan", dodgeydodgeydodgey, *origin,
				    target);
	StringTokens dodgeydodgeydodgeyAGAIN(message);
	configFwd.getModules().throwLine("game", dodgeydodgeydodgeyAGAIN, *origin,
				    target);
	return;
     }
   //Hard check for nick if its @ircdome.org ......
   if(target.toLower()=="nick@ircdome.org")
     {
	servicesFwd.SecurePrivmsg = true;
	//Safety check for the module.. :-)
	if(!configFwd.getModules().exists("nick"))
	  {
	     String togo = "Sorry - This part of servicesFwd.is currently undergoing construction";
	     servicesFwd.serviceNotice(String(togo),target,OLDoriginl);
	  }
	StringTokens bloodydodgeytoo(message);
	configFwd.getModules().throwLine("nick", bloodydodgeytoo, *origin);
	return;
     }
   servicesFwd.SecurePrivmsg = false;
   if(!configFwd.getModules().exists(target))
     {
	servicesFwd.serviceNotice("Sorry - This part of Services is currently "
			       "offline for maintenance - please try again "
			       "later",
			       configFwd.getServicesHostname(),
			       OLDoriginl);
	return;
     }
   StringTokens dodgeybutnotanymoredodgeythanthelastonewas(message);
   configFwd.getModules().throwLine(target.toLower(),
			       dodgeybutnotanymoredodgeythanthelastonewas,
			       *origin);

}

void
  PARSER_FUNC (Parser::parsePING)
{
   servicesFwd.queueAdd(":" + Kine::config().getOptionsServerName() + " PONG " +
		     tokens.rest());
}

void
  PARSER_FUNC (Parser::parseSQUIT)
{
   String server = tokens.nextToken();
 
   userListFwd.delUserByServer(server);

   servicesFwd.DelOnlineServer(server);
}
void
  PARSER_FUNC (Parser::parseQUIT)
{
   String reason = tokens.nextColonToken();

   // Delete quitting user from oper list
   if(servicesFwd.isOper(OLDorigin))
     servicesFwd.delOper(OLDorigin);

 
   RegisteredUser *reguser=regUserListFwd.getRegisteredNick(OLDorigin);
 
   if(reguser!=NULL)
     reguser->setQuitmsg(reason);

   userListFwd.delUser(OLDorigin);
}

void
  PARSER_FUNC (Parser::parseSJOIN)
{
   bool op = false;
   bool voice = false;
   bool normal = false;
   String ts1 = tokens.nextToken();
   String ts2 = tokens.nextToken();
   String channame = tokens.nextToken();
   String modes = tokens.nextToken();
   bool more;
   more = tokens.hasMoreTokens();

   if(!channelListFwd.chanExists(channame))
   {
      Channel *newchan=new Channel(channame);

      channelListFwd.addChan(newchan);
   }

   while(more)
     {
	String user = tokens.nextToken();
	StringTokens luser (user);
	String foo = luser.nextColonToken();
	String username = foo.trim();


        // First check if both @ and + are there
        if ((username[1]=='@') || (username[1]=='+'))
           username=username.substr(2, username.length()-2);
        else
         {
            // Strip leading @ or + if there is one
            if ((username[0]=='@') || (username[0]=='+'))
              username=username.substr(1, username.length()-1);
         }


        User *ptr = userListFwd.getUser(username);
        Channel *chan = channelListFwd.getChan(channame);

        if(ptr==NULL || chan==NULL)
          return;

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

        chan->addUser(username);

	if(normal)
	  {
	     if(ptr == 0)
	       {
		  return;
	       }
	     
	     if(ptr->isIdentified(username))
	       {
                  RegisteredChannel *regchan=regChannelListFwd.getRegisteredChannel(channame);

                  if(regchan!=NULL)
                  {
  		    int access = regchan->getChanAccess(username);
		    String togo = "Client: \002:"+username+":<\002 Target:002:"+chan->getName()+":\002 Access :\002"+String::convert(access)+":\002";
		  
                    if(access>99)
		    {
		       servicesFwd.mode("Chan",chan->getName(),"+o",username);
		       return;
		    }
		    if(access>49)
		    {
		       servicesFwd.mode("Chan",chan->getName(),"+v",username);
		       return;
		    }
	          }

	       }
          }
	more = tokens.hasMoreTokens();
     }
}





void
  PARSER_FUNC (Parser::parseKICK)
{


}



void
  PARSER_FUNC (Parser::parseKILL)
{



}


// Return something to the user who does VERSION or MOTD on us.
void
  PARSER_FUNC (Parser::parseMOTD)
{
   User *origin = userListFwd.getUser(OLDorigin);

   if(origin!=NULL)
     origin->sendMessage("AddANiceMessageHereWithVersionEtc", configFwd.getServicesHostname(), false);
}






