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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "exordium/services.h"
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <csignal>
#include <cstdlib>
#include "exordium/service.h"
#include <kineircd/utils.h>
#include <kineircd/kineircdconf.h>
#include <kineircd/password.h>
#include <kineircd/signals.h>
#include <kineircd/config.h>
#include <exordium/database/database.h>
#include <exordium/config.h>
#include <exordium/reguser.h>
#include <exordium/regchannel.h>
#include <exordium/user.h>
#include <exordium/channel.h>

extern "C"
{
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_NETDB_H
# include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif
};

#include <aisutil/socket/sockets.h>

using AISutil::String;
using AISutil::StringTokens;
using Kine::Password;
using AISutil::StringMask;
using Kine::Signals;
using namespace Exordium;

KINE_SIGNAL_HANDLER_FUNC(Rehash)
{
   ((Services*)foo)->logLine("\002[\002Rehash\002]\002 Services has received "
			     "the REHASH signal");
   // something here? :)
}

KINE_SIGNAL_HANDLER_FUNC(Death)
{
   String reason = 
     "\002[\002Fatal Error\002]\002 Services received \002" + 
     String(sys_siglist[signal]) + "\002 - Initiating shutdown";
   ((Services *)foo)->logLine(reason, Log::Warning);
   ((Services *)foo)->shutdown(reason);
   exit(1); // eek
}

/* Services run
 *
 * This is called from the module (loaded into kine) to begin
 * the operation of services.
 *
 * original: James Wilkins
 *
 */

   void
     Services::run(void)
       {
	  fd_set inputSet, outputSet;
	  struct timeval timer;
	  disconnectTime = 0;
	  connected = false;
	  srand(time(NULL));
	  logLine ("Cleaning out (any) stale entries from the DB");
	  databaseFwd.dbDelete("chanstatus");
	  databaseFwd.dbDelete("nicksidentified");
	  databaseFwd.dbDelete("kills");
	  databaseFwd.dbDelete("onlineservers");
	  databaseFwd.dbDelete("onlineopers");
 
	  logLine ("Entering main loop...");
	  for (;;)
	    {
	       time (&currentTime);
	       timer.tv_sec = 1;
	       timer.tv_usec = 0;
	       FD_ZERO (&inputSet);
	       FD_ZERO (&outputSet);

	       if (connected)
		 {
		    if (!stopping)
		      {
			 FD_SET (socky.getFD(), &inputSet);
		      }
		    if (queueReady ())
		      {
			 FD_SET (socky.getFD(), &outputSet);
		      }
		 }
#ifdef DEBUG		 
	       else
		 {
		    logLine("not connected...", Log::Debug);
		 }
#endif

	       switch (select (maxSock, &inputSet, &outputSet, NULL, &timer))
		 {
		  case 0:         // Select timed out
		    break;
		  case -1:                // Select broke
		    break;
		  default:                // Select says there is something to process
		    // Something from the socket?
		    if (FD_ISSET (socky.getFD(), &inputSet))
		      {
			 if (!handleInput ())
			   {
			      logLine ("Error handling server input. Reconnecting.");
			      connected = false;
			      disconnectTime = currentTime;
			      disconnect();
			   }
		      }
		    if (FD_ISSET (socky.getFD(), &outputSet))
		      {
			 if (!queueFlush ())
			   {
			      if(!stopping)
				//Ok, technically nasty, but if we're in a shutdown
				//state, do we really care if the connection closes?
				{
				   logLine("Disconnecting... (Queue flushing error)");
				   connected = false;
				   disconnectTime = currentTime;
				   disconnect ();
				}
			   }

			 if(stopping)
			   {
			      if(stopTime < currentTime)
				{
#ifdef DEBUG
				   logLine("Disconnecting, QueueFlushed and in stop state",
					   Log::Debug);
#endif
				   connected = false;
				   exit(0);
				}
			   }

		      }
		 }
	       if (currentTime > (time_t) (lastCheckPoint + 10))
		 {
		    lastCheckPoint = currentTime;
		    checkpoint ();
		 }
	       if (currentTime > (time_t) (lastExpireRun + 120))
		 {
		    lastExpireRun = currentTime;
		    SynchTime ();

		 }

#ifdef DEBUG
	       if(!connected)
		 {
		    logLine("Not connected", Log::Debug);
		 }
#endif
	       if (!connected && (currentTime >= (time_t) (disconnectTime + 10)))
		 {
#ifdef DEBUG
		    logLine("Beginning Connect Attempt", Log::Debug);
#endif
		    connect ();
		 }
	    }
       }

   Services::Services()
     : parser(),
       console(),
       sock(-1),
       maxSock(-1),
       inputBufferPosition(0),
       startTime(time(NULL)),
       lastPing(time(NULL)),
       disconnectTime(time(NULL)),
       stopTime(0),
       serverLastSpoke(time(NULL)),
       lastCheckPoint(time(NULL)),
       lastExpireRun(time(NULL)),
       connected(false),
       stopping(false),
       burstOk(false),
       countTx(0),
       countRx(0)
     {
	currentTime = time(NULL);
	SecurePrivmsg = false;
	
#ifdef DEBUG
	logLine("Setting up signal handlers",
		Log::Debug);
#endif
	Kine::signals().addHandler(&Rehash,
				   Signals::REHASH,
				   (void *)this);
	Kine::signals().addHandler(&Death,
				   Signals::VIOLENT_DEATH |
				   Signals::PEACEFUL_DEATH,
				   (void *)this);


	struct hostent *host;
	queueKill ();
	if (!(inputBuffer = (char *) malloc (inputBufferSize)))
	  {
	     logLine ("Fatal Error: Could not allocate input buffer",
		      Log::Fatality);
	     perror ("malloc");
	     exit (1);
	  }
	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
#ifdef DEBUG
	std::ostringstream debugging;
	debugging << "UplinkHost = " << configFwd.getUplinkHost() << 
	  "; UplinkPort = " << configFwd.getUplinkPort();
	logLine(debugging.str(), Log::Debug);
#endif
	if ((host = gethostbyname (configFwd.getUplinkHost().c_str())) == NULL)
	  {
	     logLine ("Fatal Error: Error resolving uplinkhost",
		      Log::Fatality);
	     exit (1);
	  }
	memcpy (&addr.sin_addr, host->h_addr_list[0], host->h_length);
	addr.sin_port = htons (configFwd.getUplinkPort());
     }

/* HandleInput()
 *
 * This handles the incoming data from our uplink, and hands it over
 * to our parser.
 *
 * Original: James Wilkins
 *
 */

bool Services::handleInput (void)
{
   std::stringstream bufferin;
   socky.read(bufferin);
   String line;
   
   while(bufferin.peek() != -1) {
      std::getline(bufferin,line);
#ifdef DEBUG
      logLine("RX: " + line, Log::Debug);
#endif
      countRx += line.length();
      parser.parseLine(line);
   }
   
   return true;
}

/* disconnect()
 *
 * (Uncleanly?) say bye bye to our uplink..
 *
 */

   void
     Services::disconnect (void)
       {
#ifdef DEBUG
	  logLine("Closing socket.", Log::Debug);
#endif
	  socky.close();
	  connected = false;
       }

/* connect()
 *
 * Connect to our uplink! Yeah!
 *
 */

   bool Services::connect (void)
     {
	logLine ("Attempting Connection to Uplink");
	if (sock >= 0)
	  {
#ifdef DEBUG
	     logLine("Closing stale network socket", Log::Debug);
#endif
	     socky.close();
	     sock = -1;
	  }

	socky.setRemoteAddress(configFwd.getUplinkHost());

        // Note: Maybe add an option for port?
	//
	socky.setRemotePort(configFwd.getUplinkPort());

	if(!socky.connect())
	  {
#ifdef DEBUG
	     logLine(String("Socky.connect() returned an error: ") +
		     socky.getErrorMessage(), 
		     Log::Debug);
#endif

	  }
/* I'm not particulary happy with how this is coded.
 * In all honesty, we should await some verification from the uplink
 * that it is ready to receive data, as opposed to just blinding throwing
 * everything at our uplink.. and possibly (at a later stage) filling
 * up our sendQ on the server
 */
	connected = true;
	logLine ("Beginning handshake with uplink");
	maxSock = socky.getFD() + 1;

/* *Whistles* Config option */
	queueAdd ("PASS pass :TS");
	queueAdd ("CAPAB TS3 BURST UNCONNECT NICKIP");
/* Jesus, so many hard coded stuff :( */
/* It's okay, all the server connection stuff has a very very very limited
 * lifespan in anycase.. - pickle
 */
	queueAdd ("SERVER " + Kine::config().getOptionsServerName() + " 1 :" +
		  Kine::config().getOptionsDescription());
	
	// Do we have an underling?
	if (!configFwd.getUnderlingHostname().empty()) {
	   queueAdd ("SERVER " + configFwd.getUnderlingHostname() + " 2 :" + configFwd.getUnderlingDescription());
	}
	
	queueAdd ("SVINFO 3 1 0 :"+String::convert(currentTime));
	queueAdd (":" + Kine::config().getOptionsServerName() + " EOB");
	queueAdd ("BURST");
	//	queueFlush();

	// Start all the modules
	configFwd.getModules().startAll();
	
	// Is the console actually wanted?
	if (configFwd.getConsoleEnabled()) {
	   registerService(configFwd.getConsoleName(), configFwd.getConsoleName(),
			   configFwd.getConsoleHostname(),
			   configFwd.getConsoleDescription());
	   // I smell a configuration variable.. *sniff sniff* can you?
	   serviceJoin(configFwd.getConsoleName(), "#Exordium");
	}
	
	connected = true;
	
	queueAdd ("BURST 0");
	return true;
     };

/* doBurst()
 *
 * Load our modules... (move to a config option thingie)
 * and send our burst stuff to the uplink
 *
 */

   void
     Services::doBurst (void)
       {
	  // Start all the modules
	  configFwd.getModules().startAll();
	  
	  // Is the console actually wanted?
	  if (configFwd.getConsoleEnabled()) {
	     registerService(configFwd.getConsoleName(), configFwd.getConsoleName(),
			     configFwd.getConsoleHostname(),
			     configFwd.getConsoleDescription());
	     // I smell a configuration variable.. *sniff sniff* can you?
	     serviceJoin(configFwd.getConsoleName(), "#Exordium");
	  }

	  connected = true;
	  // this is dodgey.
	  configFwd.getModules().dumpModules();
	  return;
       }
/* getQuote(int)
 *
 * This proberly doesn't belong here...
 *
 * It fetches a quote from the database for the Game:: module
 *
 */

   String Services::getQuote(int const &number)
     {
        databaseFwd.dbSelect("body", "fortunes", "id='"+String::convert(number)+"'");

        if(databaseFwd.eof())
           return "";
        else
           return databaseFwd.dbGetValue();
     }

/* getLogCount()
 *
 * Count the total number of log entries in our database..
 *
 */

   String Services::getLogCount(void)
     {
        databaseFwd.dbCount("log");
        return databaseFwd.dbGetValue();
     }

/* getNoteCount()
 *
 * Count and return the total number of notes in our database
 *
 */

   String Services::getNoteCount(void)
     {
        databaseFwd.dbCount("notes");
        return databaseFwd.dbGetValue();
     }
/* getGlineCount()
 *
 * Count and return the total number of glines in our database
 *
 */

   String Services::getGlineCount(void)
     {
        databaseFwd.dbCount("glines");
        return databaseFwd.dbGetValue();
     }

/* shutdown(String)
 *
 * Initiate a services shutdown with the given reason being
 * propagated accross the network
 *
 */

   void Services::shutdown(String const &reason)
     {
	logLine("Services is shutting down " + reason,
		Log::Warning);

	// Stop and unload all the modules..
	configFwd.getModules().unloadAll(reason);
	
	// I do not like this.. oh well..  - pickle
	if (configFwd.getConsoleEnabled()) {
	   queueAdd(":IRCDome QUIT :"+reason);
	}
	
	queueAdd(":" + Kine::config().getOptionsServerName() +
		 " SQUIT " + Kine::config().getOptionsServerName() + " :" +
		 reason);
	stopping = true;
	stopTime = currentTime + 5;

        // Clean up before we die
        databaseFwd.dbDelete("chanstatus");
        databaseFwd.dbDelete("nicksidentified");
        databaseFwd.dbDelete("kills");
        databaseFwd.dbDelete("onlineservers");
        databaseFwd.dbDelete("onlineopers");
     }

/* SynchTime()
 *
 * Perform various tasks that need doing on an ongoing basis
 * those being;
 * 	Expire any channel bans that need expiring
 *
 */

   void Services::SynchTime(void)
     {
	//Undo any expired glines

	//Undo any expired channel bans
	String ctime = String::convert(currentTime);

        databaseFwd.dbSelect("id,chan,mask", "chanbans", "expireon<"+ctime);

        RegisteredChannel *chan;

        while(!databaseFwd.eof())
	{   
             chan=regChannelListFwd.getRegisteredChannelByID(databaseFwd.dbGetValue(1).toInt());

             if(chan!=NULL)
               chan->RemoveBan(databaseFwd.dbGetValue(2));
           
             databaseFwd.dbGetRow();
	}

	//Lastly commit any outstanding db changes.
     }

/* expireRun()
 *
 * Again, perform any tasks that need doing at a regular interval
 *
 * These being expiring glines, and statistical counts for information
 * purposes
 *
 */

   void Services::expireRun(void)
     {
	String nc = String::convert(regUserListFwd.nbRegUsers());
	String cc = String::convert(regChannelListFwd.nbRegChans());
	String oc = String::convert(userListFwd.nbUsers());
	String lc = getLogCount();
	String gc = getGlineCount();
	String noc = getNoteCount();
	unsigned long rx = getCountRx();
	unsigned long tx = getCountTx();
	String togo = String("NC [\002")+nc+"\002] CC [\002"+cc+"\002] OC [\002"+oc+"\002] LC [\002"+lc+"\002] GC [\002"+gc+"\002] NOC [\002"+noc+"\002] RX [\002"+String::convert(rx)+"\002] TX [\002"+String::convert(tx)+"\002]";
	servicePrivmsg(String(togo),"Oper","#Debug");
     }

   /* AddOnlineServer(ServerName,Hops,Description)
    *
    * Add a server into our database thingie oo :((((
    *
    */

   void
     Services::AddOnlineServer (String const &servername, String const &hops, String const &description)
       {
          databaseFwd.dbInsert("onlineservers", "'','"+servername+"','"+hops+"','"+description+"'");
       }

   /* DelOnlineServer(ServerName)
    *
    * Del a server from our database
    *
    */

   // NOTE: gotta recheck this up when more than 1 server is up, should compare ID instead.. maybe make a server map?
   void
     Services::DelOnlineServer (String const &name)
       {
          databaseFwd.dbDelete("onlineservers", "servername='" + name + "'");
       }

/* doPong(line)
 *
 * Ping... Pong!
 *
 */

   void
     Services::doPong (String const &line)
       {
	  queueAdd (String (":services.ircdome.org PONG ") + line);
       }

/* mode(String,String,String,String)
 *
 * A messy way of sending a mode
 *
 */

   void
     Services::mode (String const &who, String const &chan, String const &mode,
		     String const &target)
       {
	  queueAdd (":"+who+" MODE "+chan+ " " + mode + " " + target);
       }
/* doHelp(User,String,String,String)
 *
 * Generate a help page from our dynamic help system
 *
 */
   void
     Services::doHelp(String const& origin, String const &service,
			      String const &topic, String const &parm)
       {
          RegisteredUser *user=regUserListFwd.getRegisteredNick(origin);

          if(user==NULL)
             return;

	  if(topic == "")
	    {
	       //No topic, no parm.
               databaseFwd.dbSelect("txt", "help", 
                    "service='"+service+"' AND word='' AND parm='' AND lang='"+user->getLang()+"' ORDER by id");

               String line="";

               while(!databaseFwd.eof())
		 {    
		    line = parseHelp(databaseFwd.dbGetValue());
		    serviceNotice(line,service,origin);
                    databaseFwd.dbGetRow();
		 }
	       return;
	    }
	  // End
	  if(parm == "")
	    {
	       //No topic, no parm.
               
               databaseFwd.dbSelect("txt", "help",
                    "service='"+service+"' AND word='"+topic+"' AND parm='' AND lang='"+user->getLang()+"' ORDER by id");

               String line="";

               while(!databaseFwd.eof())
                 {
                    line = parseHelp(databaseFwd.dbGetValue());
                    serviceNotice(line,service,origin);
                    databaseFwd.dbGetRow();
                 }

	       return;
	    }
	  // End

               databaseFwd.dbSelect("txt", "help",
                    "service='"+service+"' AND word='"+topic+"' AND parm='"+parm+"' AND lang='"+user->getLang()+"' ORDER by id");

               String line="";

               while(!databaseFwd.eof())
                 {
                    line = parseHelp(databaseFwd.dbGetValue());
                    serviceNotice(line,service,origin);
                    databaseFwd.dbGetRow();
                 }
               return;
            }

/* sendEmail(String,String,String)
 *
 * post an email into the database, which is polled later by a third party
 * utilite to send any pending emails
 *
 */

   void
     Services::sendEmail (String const &to, String const &subject, String const &text)
       {
          databaseFwd.dbInsert("emails", "'','"+to+"','"+subject+"','"+text+"'");
       }

/* parseHelp(In)
 *
 * This parses our special codes in the help files
 * allowing for items such as bold, italics etc
 *
 */

   String
     Services::parseHelp (String const &instr)
       {
	  String retstr;
	  for (unsigned int i = 0; i != instr.length(); i++)
	    {
	       if (instr[i] == '%' && ((i != 0) && (instr[i - 1] != '\\')))
		 {
		    switch (instr[++i])
		      {
		       case 'u':
			 retstr += '\037';
			 break;
		       case 'r':
			 retstr += '\026';
			 break;
		       case 'b':
			 retstr += '\002';
			 break;
		       default:
			 retstr += instr[i];
		      }
		 }
	       else
		 {
		    retstr += instr[i];
		 }
	    }
	  return retstr;
       }

/* log(String,String,String)
 *
 * Logs the given information into the database
 * which can later be accessed by staff and/or
 * the web interface
 *
 * There are two versions, one for normal logs, and the other
 * for channel based access.
 */

   void
     Services::log (User& origin, String const &service, String const &text, String const &cname)
       {
	  String nicks = origin.getIDList();
	  String ident = origin.getUsername();
	  String host = origin.getHostname();
          databaseFwd.dbInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','"+cname+"'");
       }

   void
     Services::log (User& origin, String const &service, String const &text)
       {
	  String nicks = origin.getIDList();
	  String ident = origin.getUsername();
	  String host = origin.getHostname();
          databaseFwd.dbInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"',''");
       }

/* Overload kine's trim to add some stuff of our own! :-) */
   String String::trim(void) const
     {
	size_type s = 0;
	size_type e = length();

	while ((s < e) &&
	       ((c_str()[s] == ' ') ||
		(c_str()[s] == '\t') ||
		(c_str()[s] == '\r') ||
		(c_str()[s] == ':') ||
		(c_str()[s] == '@') ||
		(c_str()[s] == '+') ||
		(c_str()[s] == '\n')))
	  {
	     s++;
	  }

	while ((e > s) &&
	       ((c_str()[e - 1] == ' ') ||
		(c_str()[e - 1] == '\t') ||
		(c_str()[e - 1] == '\r') ||
		(c_str()[e - 1] == ':') ||
		(c_str()[e - 1] == '@') ||
		(c_str()[e - 1] == '+') ||
		(c_str()[e - 1] == '\n')))
	  {
	     e--;
	  }

	return substr(s, e - s);
     }

   /* servicePart(String,String)
    *
    * Make a given service part a channel.
    *
    * Perhaps this should be handed over to each individual module
    * to control
    *
    */

   void
     Services::servicePart(String const &service, String const &target)
       {
	  queueAdd (String (":") + service + " PART " + target);
       }

   void Services::serviceJoin(AISutil::String const &service,
			      AISutil::String const &target)
     {
	queueAdd(":" + Kine::config().getOptionsServerName() + " SJOIN " +
		 AISutil::String::convert(currentTime) + " " + target +
		 " + :" + service);
     };

   /* usePrivmsg(nick)
    *
    * Figure out whether we should use the privmsg or
    * the notice interface to talk to a client.
    *
    */

   bool
     Services::usePrivmsg (String const &nick)
       {
          RegisteredUser *user=regUserListFwd.getRegisteredNick(nick);

          if(user!=NULL)
          {
             if(user->getPrivmsg()==1)
                return true;
             else 
                return false;
          }

          return false;
       }

   void
     Services::serviceKick(String const &chan, String const &nick, String const &reason)
       {
	  queueAdd (String (":Chan KICK ")+chan+" "+nick+" :"+reason);
       }


   void
     Services::sendNote(String const &from, String const &to, String const &text)
       {
	  String thenick = to.IRCtoLower();
          databaseFwd.dbInsert("notes", "'','"+from+"','"+to+"',NOW(),'"+text+"'");

          User *u=userListFwd.getUser(thenick);

          if(u!=NULL)
	    {
	       if(u->isIdentified(thenick))
		 {
		    String togo = String("\002[\002New Note\002]\002 From \002")+from+"\002";
		    serviceNotice(togo,"Note",to);
		 }
	    }

       }

// TODO: check why the 2 lines are commented
void Services::checkpoint(void)
{
	  //Any nick mods to be done? :-)

          databaseFwd.dbSelect("kills");

          while(!databaseFwd.eof())
	    {
	       String id = databaseFwd.dbGetValue(0);
	       String killt = databaseFwd.dbGetValue(2);
	       int nowt = currentTime;
	       String tomod = databaseFwd.dbGetValue(1);
               databaseFwd.dbGetRow();

	       if(killt.toInt() < nowt)
		 {
                    databaseFwd.dbDelete("kills", "id='"+id+"'");
		    int foo = 0;
		    bool running = true;
		    String newnick = "";
		    while(running)
		      {
			 foo++;
			 newnick = tomod+String::convert(foo);
			 if(!regUserListFwd.isRegistered(newnick))
			   {
			      if(!userListFwd.userExists(newnick))
				{
				   //Oke we can use this one :)
				   running = false;
				}
			   }

		      }
		    User *ptr = userListFwd.getUser(tomod);
		    if(ptr==NULL)
		      {
#ifdef DEBUG
			 logLine("I could not find the users record.. hence i"
				 " can't change their nick " + tomod,
				 Log::Debug);
#endif
			 return;
		      }

		    String msg = "Non-Identification: Your nickname is now being changed";
		    serviceNotice(msg,"Nick",tomod);
		    queueAdd(":" + Kine::config().getOptionsServerName() +
			     " MODNICK " + tomod + " " + newnick + " :0");
                    ptr->setNick(newnick);
		 }
	       if((killt.toInt()-nowt)<60)
		 {
		    if((killt.toInt()-nowt)>50)
		      {
			 String msg = "\002[\002Identification Warning\002]\002 Less than 60 seconds left to identify";
			 serviceNotice(msg,"Nick",tomod);
		      }
		 }
	    }

}


bool Services::queueFlush(void)
{
   if(connected)
     {
	if (socky.isOkay())
	  {
	     if (socky.write (outputQueue.front ()))
	       {
#ifdef DEBUG
		  logLine("DEBUG TX:" + outputQueue.front(),
			  Log::Debug);
#endif
		  outputQueue.pop ();
		  return true;
	       }
	     else
	       {
		  if(!stopping)
		    {
		       connected = false;
		       return false;
		    }
		  else
		    {
		       exit(0);
		    }
	       }
	  }
	else
	  {
#ifdef DEBUG
	     logLine("Socky is dead :(", Log::Debug);
#endif
	     connected = false;
	     return false;
	  }
	return false;

     }
   else
     {
#ifdef DEBUG
	logLine("Trying to queueflush when disconnected ?!",
		Log::Debug);
#endif
	return false;
     }
};


/* generatePassword(String,String)
 *
 * Generate a new password for the given user.
 *
 */

String
  Services::generatePassword(String const &nickname, String const &password)
{
   String bob((char *)Kine::Password::makePassword(nickname,password).s_char,(String::size_type)20);
   return bob;
}

/* isAuthorised(String)
 *
 * Return true if the specificed server is allowed to connect
 * to the network ... otherwise false
 *
 */

// TEMP commenting!
bool Services::isAuthorised(String const &server)
{
/*
   if( databaseFwd.dbSelect("id", "serverlist", "name='"+server+"'") < 1 )
      return false;
   else
   {
     if( databaseFwd.dbGetValue().toInt() > 0 )
       return true;
     else
       return false;
   }
*/
return true;
};

/* getRequiredAccess(String,String)
 *
 * Find ( and return ) the required access to run a command on
 * with the given service.
 *
 */

int Services::getRequiredAccess(String const &service,
					String const &command)
{
   databaseFwd.dbSelect("access", "commands", "service='"+service+"' AND command='"+command+"'");

   if(databaseFwd.eof())

     /* If no match return 999 - to ensure no-one can use the command
      * since its obvious a command entry hasn't been set up for it */
      return 999;

   else
      return databaseFwd.dbGetValue().toInt();
};

/* modeIdentify(String)
 *
 * Causes services to set a user client +r
 * meaning they have identified to the nickname
 * they are currently using.
 *
 * as an after thought , maybe this should be an inline function? thoughts?
 */

void Services::modeIdentify(String const &nick)
{
   queueAdd(":" + Kine::config().getOptionsServerName() + " SVSMODE " + nick + 
	    " +r");
   return;
}

void Services::addOper(String const &nick, int access)
{
   RegisteredUser *u=regUserListFwd.getRegisteredNick(nick);

   if(u!=NULL)
     databaseFwd.dbInsert("onlineopers", "''," + String::convert(u->getID()) + "," +String::convert(access));
}

void Services::delOper(String const &nick)
{
   RegisteredUser *u=regUserListFwd.getRegisteredNick(nick);

   if(u!=NULL)
     databaseFwd.dbDelete("onlineopers", "nickid="+String::convert(u->getID()));
}

bool Services::isOper(String const &nick)
{
   RegisteredUser *u=regUserListFwd.getRegisteredNick(nick);

   if(u!=NULL)
   {
     databaseFwd.dbSelect("id", "onlineopers", "nickid="+ String::convert(u->getID()));

     if(databaseFwd.eof())
       return false;
     else
       return true;
   }

   return false;
}

void Services::validateOper(String &origin)
{
   String oper="Oper";
   int axs=getAccess(oper, origin);

   //Active Oper? (hah :-)

   if(axs==0)
     {
	//Non-Authorised.
	String tosend = origin+" just tried to become an IRC Operator - \002No Access\002";
	logLine(tosend, Log::Warning);
	String reason = "You have no permission to become an IRC Operator";
	/* We have a problem here - for some reason the parser is trying 
	 * to validate every online user - this is what happened when I 
	 * brought Oper online today
	 * 
	 * >09:32:41< *** Quits: Praetorian (Killed (Oper ()))
	 * >09:32:41< *** Quits: Luster (Killed (Oper ()))
	 * >09:32:41< *** Quits: allan1 (Killed (Oper ()))
	 * 
	 * None of these users had usermode +o :-)
	 *
	 * killnick(origin, "Oper", reason);
	 */
	return;
     }
   if(axs==-1)
     {
	String tosend = origin+" just tried to become an IRC Operator - \002Suspended\002";
	logLine(tosend, Log::Warning);
	String reason = "You are suspended - Do not try to become an Operator";
	killnick(origin, "Oper", reason);
	return;
     }
   if(axs>0)
     {
	String tosend = origin+" just became an IRC Operator - level "+String::convert(axs);
	logLine(tosend);

	if (!isOper(origin))
	  addOper(origin, axs);
#ifdef DEBUG
	else
	  {
	     logLine("Warning: inconsistency in ValidateOper: "
		     "new oper already in onlineopers!",
		     Log::Debug);
	  }
#endif

	return;
     }

}

int
  Services::getAccess(String &service, String &nickname)
{
   databaseFwd.dbSelect("access", "access", "nickname='"+nickname+"' AND service='"+service+"'");

   if(databaseFwd.eof())
     return 0;
   else
   {
#ifdef DEBUG
      logLine("AXS:" + databaseFwd.dbGetValue(),
	      Log::Debug);
#endif
     return databaseFwd.dbGetValue().toInt();
   }
}

