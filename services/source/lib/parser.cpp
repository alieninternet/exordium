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

#include "exordium/config.h"
#include "exordium/parser.h"
#include "exordium/services.h"
//#include "exordium/dchan.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include "exordium/ircdome.h"

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

   services.logLine("Unparsed Input!: " + line);
};

void PARSER_FUNC (Parser::parseAWAY)
{
   String reason = tokens.rest();
   User *origin = services.findUser(OLDorigin);
   if(origin == 0)
     {
#ifdef DEBUG
	std::cout << "Our pointer is null." << std::endl;
#endif
	return;
     }
 
   /* From here on i'm fiddling with away */
   services.getConfig().getModules().handleAway(*origin,reason);

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
//	if(!services.getChannel().isChanRegistered(dest))
//	  {
//	     return;
//	  }
// I decided that chanstatus should have info on ALL channels....
// 
// 
//        currentmodes = channel.getModes();

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
#ifdef DEBUG
                   else
                     std::cout << "Warning: inconsistency in parsem: oper is not in onlineopers!" << std::endl;
#endif

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
   services.helpme(String(togo),"Serv");
   return;
}

void
  PARSER_FUNC (Parser::parsePART)
{
   String channel = tokens.nextToken();
   services.getChannel().internalDel(OLDorigin,channel);

   services.getDatabase().dbDelete("onlinechan", "name='"+channel+"'");
}

void PARSER_FUNC (Parser::parseN)
{
   
   if(tokens.countTokens() < 11)
     {
/* Client Nickname Change */
	String tempString = OLDorigin.trim().IRCtoLower();	
        User *origin = services.findUser(tempString);
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
        services.setNick(*origin, newnick); 
        services.getDatabase().dbDelete("kills", "nick='"+OLDorigin+"'");
	if(services.isNickRegistered(origin->getNickname()))
	  {
#ifdef DEBUG
	     std::cout << "Nick is registered" << std::endl;
#endif
	     if(!origin->isIdentified(origin->getNickname()))
	       {
#ifdef DEBUG
		  std::cout << "it isn't identified" << std::endl;
#endif
		  if(!origin->isPending())
		    {
#ifdef DEBUG
		       std::cout << "its not pending" << std::endl;
#endif
			/* Not identified as new nickname */
		       /* Added this for raff. */
		       /* He's an annoying little pratt isn't he?
			* But somewhat loveable ;) - simon
			*/
		       if(origin->modNick())
			 {
#ifdef DEBUG
			    std::cout << "they want modnick :-)" << std::endl;
#endif
		       	   origin->addCheckIdentify();
			 }
#ifdef DEBUG
		       else
			 {
			    std::cout << "they dont want modnick" << std::endl;
			 }
#endif
		       
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
   User *newNick = services.addClient(nick, hops, timestamp, username, host,
				      vwhost, server, modes, realname);
   if (newNick == 0) {
#ifdef DEBUG
      std::cout << "That client wasn't such a nice fellow afterall :(" << 
	std::endl;
#endif
      return;
   }

   services.getConfig().getModules().handleClientSignon(*newNick);
/*   if (modes.find("o"))
     services.validateOper(nick);
*/

/*   
*   int num = newNick->countHost();
*
*   int nbRes = services.getDatabase().dbSelect("txt", "news", "level=0 AND expires<"+String::convert(services.currentTime));
*
*   // NOTE: hardcoded bot nick?
*   for (int i=0; i<nbRes; i++)
*   {
*      newNick->sendMessage("\002[\002IRCDome Global News\002]\002 "+ services.getDatabase().dbGetValue(), services.getConfig().getConsoleName());
*      services.getDatabase().dbGetRow();
*   }
*
*   services.queueAdd(":IRCDome WALLOPS :\002[\002Sign On\002]\002 "+nick+" ("+username+"@"+host+") ["+server+"]");
*   if(num>2)
*     {
*	String alert = "\002Alert\002 excess connections from "+host+" - Latest client is "+nick+"!"+username+"@"+host+" - ("+String::convert(num)+")";
*	services.globop(alert,"Oper");
*	//Add gline.
*     }
*/
}
void
  PARSER_FUNC (Parser::parsePRIVMSG)
{
   User *origin = services.findUser(OLDorigin);

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
   if ((target.IRCtoLower ()) == 
       services.getConfig().getConsoleName().IRCtoLower())
     {
	services.getIRCDome().parseLine(message,OLDoriginl);
	return;
     }
#ifdef DEBUG
   std::cout << '\'' << target << '\'' << std::endl;
#endif
   if(target[0]=='#')
     {
	//Oke channel msg :>
	if(!services.getConfig().getModules().exists("chan"))
	  {
	     String togo = "Sorry - Channel services are currently disabled for maintenance";
	     services.serviceNotice(String(togo),target,OLDoriginl);
	     return;
	  }
	
	// dodgey? YES.. YES IT IS.. :(
//	services.getConfig().getModules().throwLine("chan",message,OLDoriginl,target);
//	services.getConfig().getModules().throwLine("game",message,OLDoriginl,target);
	StringTokens dodgeydodgeydodgey(message);
	services.getConfig().getModules().throwLine("chan", dodgeydodgeydodgey, *origin,
				    target);
	StringTokens dodgeydodgeydodgeyAGAIN(message);
	services.getConfig().getModules().throwLine("game", dodgeydodgeydodgeyAGAIN, *origin,
				    target);
	return;
     }
   //Hard check for nick if its @ircdome.org ......
   if(target.toLower()=="nick@ircdome.org")
     {
	services.SecurePrivmsg = true;
	//Safety check for the module.. :-)
	if(!services.getConfig().getModules().exists("nick"))
	  {
	     String togo = "Sorry - This part of services is currently undergoing construction";
	     services.serviceNotice(String(togo),target,OLDoriginl);
	  }
	StringTokens bloodydodgeytoo(message);
	services.getConfig().getModules().throwLine("nick", bloodydodgeytoo, *origin);
	return;
     }
   services.SecurePrivmsg = false;
   if(!services.getConfig().getModules().exists(target))
     {
	services.serviceNotice("Sorry - This part of Services is currently "
			       "offline for maintenance - please try again "
			       "later",
			       services.getConfig().getServicesHostname(),
			       OLDoriginl);
	return;
     }
   StringTokens dodgeybutnotanymoredodgeythanthelastonewas(message);
   services.getConfig().getModules().throwLine(target.toLower(),
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
 
   services.getDatabase().dbDelete("identified", "identified.nick=onlineclients.id AND onlineclients.server='"+server+"'");
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
 
   
   services.getDatabase().dbDelete("identified", "nick='"+String::convert(oid)+"'");

   //Store the quit reason here
   services.getDatabase().dbUpdate("nicks", "quitmsg='"+reason+"'", "nickname='"+OLDorigin+"'");

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
        services.getDatabase().dbInsert("onlinechan", "'','"+chan+"'");
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



        /******* NOTE
         Bug fixed: leading @ or + was causing user not found errors.
         Solution: foo is only used to toggle the booleans, username 
                   contains the nick without leading @ or +.
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
	     
	     if(ptr->isIdentified(username))
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
   User *origin = services.findUser(OLDorigin);

   origin->sendMessage("AddANiceMessageHereWithVersionEtc", services.getConfig().getServicesHostname(), false);
}



