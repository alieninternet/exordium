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

#include "exordium/services.h"
#include "exordium/channel.h"
#include "exordium/user.h"
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <fstream>
#include "exordium/log.h"
#include "exordium/parser.h"
#include "exordium/service.h"
#include <kineircd/utils.h>
#include <kineircd/kineircdconf.h>
#include <kineircd/signals.h>

extern "C"
{
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dlfcn.h>
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
   String reason = "\002[\002Rehash\002]\002 Services has received the REHASH signal";
   ((Services *)foo)->helpme(reason,"Serv");
}

KINE_SIGNAL_HANDLER_FUNC(Death)
{
   String reason = "\002[\002Fatal Error\002]\002 Services received \002"+String(sys_siglist[signal]) + "\002 - Initiating shutdown";
   //((Services *)foo)->helpme(reason, "Serv");
   ((Services *)foo)->shutdown(reason);
   exit(1);
}

namespace Exordium
{

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
	  logger.logLine ("Cleaning out (any) stale entries from the DB");
	  database.dbDelete("onlineclients");
	  database.dbDelete("chanstatus");
	  database.dbDelete("identified");
	  database.dbDelete("kills");
	  database.dbDelete("onlineservers");
	  database.dbDelete("onlinechan");
	  database.dbDelete("onlineopers");
 
	  logger.logLine ("Entering main loop...");
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
	       else
		 {
		    std::cout << "not connected..." << std::endl;
		 }

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
			      logger.logLine ("Error handling server input. Reconnecting.");
			      helpme("Error in handleInput() - Jumping server","Serv");
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
				   logger.logLine("Disconnecting... (Queue flushing error)");
				   connected = false;
				   disconnectTime = currentTime;
				   disconnect ();
				}
			   }

			 if(stopping)
			   {
			      if(stopTime < currentTime)
				{
				   logger.logLine("Disconnecting, QueueFlushed and in stop state");
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
		    expireRun ();
		    SynchTime ();

		 }
	       if (currentTime > (time_t) (lastModeRun + 1))
		 {
		    lastModeRun = currentTime;
		    if(ModequeueReady())
		      {
			 ModequeueFlush();
		      }
		 }
	       if(!connected)
		 {
		    std::cout << "Not connected" << std::endl;
		 }
	       if (!connected && (currentTime >= (time_t) (disconnectTime + 10)))
		 {
		    std::cout << "Beginning Connect Attempt" << std::endl;
		    connect ();
		 }
	    }
       }

   Services::Services(Kine::Daemon& d, Log& l, Config& c, CDatabase& db)
     : daemon(d),
   logger(l),
   database(db),
   config(c),
   parser(*this),
   channel(*this),
   ircdome(*this)
     {
	sock = -1;
	maxSock = -1;
	inputBufferPosition = 0;
	stopTime = 0;
	connected = false;
	stopping = false;
	burstOk = false;
	SecurePrivmsg = false;
	countTx = 0;
	countRx = 0;
	logger.logLine("Setting up signal handlers");
	getDaemon().getSignals().addHandler(&Rehash, Signals::REHASH, (void *)this);
	getDaemon().getSignals().addHandler(&Death,
					    Signals::VIOLENT_DEATH | Signals::PEACEFUL_DEATH,
					    (void *)this);


        database.dbConnect();

	struct hostent *host;
	queueKill ();
	startTime = currentTime = lastPing = lastExpireRun = lastCheckPoint = serverLastSpoke = time (NULL);
	if (!(inputBuffer = (char *) malloc (inputBufferSize)))
	  {
	     logger.logLine ("Fatal Error: Could not allocate input buffer");
	     perror ("malloc");
	     exit (1);
	  }
	memset (&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	std::cout << config.getUplinkHost() << std::endl;
	std::cout << config.getUplinkPort() << std::endl;
	if ((host = gethostbyname (config.getUplinkHost().c_str())) == NULL)
	  {
	     logger.logLine ("Fatal Error: Error resolving uplinkhost");
	     exit (1);
	  }
	memcpy (&addr.sin_addr, host->h_addr_list[0], host->h_length);
	addr.sin_port = htons (config.getUplinkPort());
     }

   bool Services::writeData (String & line)
     {
	if (!sock)
	  {
	     return false;
	  }
	countTx += line.length ();
	if (::write (sock, line.c_str(), (int)line.length ()) +::
	    write (sock, "\n", 1) != (int)line.length () + 1)
	  {
	     return false;
	  }
	return true;
     }

Services::~Services()
{
  database.dbDisconnect();
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
	while(bufferin.peek()!=-1)
	  {
	     std::getline(bufferin,line);
	     logger.logLine("RX: "+line);
	     countRx += line.length();
	     parser.parseLine(line);
	  }

	return true;
     };

/* disconnect()
 *
 * (Uncleanly?) say bye bye to our uplink..
 *
 */

   void
     Services::disconnect (void)
       {
	  logger.logLine ("Closing socket.");
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
	logger.logLine ("Attempting Connection to Uplink");
	if (sock >= 0)
	  {
	     logger.logLine ("Closing stale network socket");
	     socky.close();
	     sock = -1;
	  }

	socky.setRemoteAddress(config.getUplinkHost());

        // Note: Maybe add an option for port?
	//
	socky.setRemotePort(config.getUplinkPort());

	if(!socky.connect())
	  {
	     std::cout << "Socky.connect() returned an error: " <<
	       socky.getErrorMessage() << std::endl;

	  }
/* I'm not particulary happy with how this is coded.
 * In all honesty, we should await some verification from the uplink
 * that it is ready to receive data, as opposed to just blinding throwing
 * everything at our uplink.. and possibly (at a later stage) filling
 * up our sendQ on the server
 */
	connected = true;
	logger.logLine ("Beginning handshake with uplink");
	maxSock = socky.getFD() + 1;

/* *Whistles* Config option */
	queueAdd ("PASS pass :TS");
	queueAdd ("CAPAB TS3 BURST UNCONNECT NICKIP");
/* Jesus, so many hard coded stuff :( */
/* It's okay, all the server connection stuff has a very very very limited
 * lifespan in anycase.. - pickle
 */
	queueAdd ("SERVER " + config.getServicesHostname() + " 1 :" + config.getServicesDescription());
	queueAdd ("SERVER " + config.getConsoleHostname() + " 2 :" + config.getConsoleDescription());
	queueAdd ("SVINFO 3 1 0 :"+String::convert(currentTime));
	queueAdd (":" + config.getServicesHostname()  + " EOB");
	queueAdd ("BURST");
	//	queueFlush();
	doBurst ();
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
	  config.getModules().startAll(*this);
	  
	  // Is the console actually wanted?
	  if (config.getConsoleEnabled()) {
	     registerService(config.getConsoleName(), config.getConsoleName(),
			     config.getConsoleHostname(), "+dz",
			     config.getConsoleDescription());
	     serviceJoin (config.getConsoleName(), "#debug");
	     serviceJoin (config.getConsoleName(), "#Exordium");
	  }

	  connected = true;
	  // this is dodgey.
	  config.getModules().dumpModules();
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
        if( database.dbSelect("body", "fortunes", "id='"+String::convert(number)+"'") < 1 )
           return "";
        else
           return database.dbGetValue();
     }

/* getLogCount()
 *
 * Count the total number of log entries in our database..
 *
 */

   String Services::getLogCount(void)
     {
        return String::convert(database.dbCount("log"));
     }

/* getNoteCount()
 *
 * Count and return the total number of notes in our database
 *
 */

   String Services::getNoteCount(void)
     {
        return String::convert(database.dbCount("notes"));
     }
/* getGlineCount()
 *
 * Count and return the total number of glines in our database
 *
 */

   String Services::getGlineCount(void)
     {
        return String::convert(database.dbCount("glines"));
     }

/* shutdown(String)
 *
 * Initiate a services shutdown with the given reason being
 * propagated accross the network
 *
 */

   void Services::shutdown(String const &reason)
     {
	helpme("Services is shutting down " + reason, config.getConsoleName());

	// Stop and unload all the modules..
	config.getModules().unloadAll(reason);
	
	// I do not like this.. oh well..  - pickle
	if (config.getConsoleEnabled()) {
	   queueAdd(":IRCDome QUIT :"+reason);
	}
	
	queueAdd(":services.ircdome.org SQUIT chrome.tx.us.ircdome.org :"+reason);
	stopping = true;
	stopTime = currentTime + 5;

        // Clean up before we die
        database.dbDelete("onlineclients");
        database.dbDelete("chanstatus");
        database.dbDelete("identified");
        database.dbDelete("kills");
        database.dbDelete("onlineservers");
        database.dbDelete("onlinechan");
        database.dbDelete("onlineopers");
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

        int nbRes = database.dbSelect("id,chan,mask", "chanbans", "expireon<"+ctime);

        for(int i=0; i<nbRes; i++)
	  {
	     channel.RemoveBan(database.dbGetValue(0), database.dbGetValue(1), database.dbGetValue(2));
             database.dbGetRow();
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
	String nc = getRegNickCount();
	String cc = channel.getChanCount();
	String oc = getOnlineCount();
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
          database.dbInsert("onlineservers", "'','"+servername+"','"+hops+"','"+description+"'");
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
          database.dbDelete("onlineservers", "servername='" + name + "'");
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
     Services::doHelp(User& origin, String const &service,
		      String const &topic, String const &parm)
       {
	  if(topic == "")
	    {
	       //No topic, no parm.
               int nbRes = database.dbSelect("txt", "help", 
                    "service='"+service+"' AND word='' AND parm='' AND lang='"+origin.getLanguage()+"' ORDER by id");

               String line="";

               for(int i=0; i<nbRes; i++)
		 {    
		    line = parseHelp(database.dbGetValue());
		    serviceNotice(line,service,origin.getNickname());
                    database.dbGetRow();
		 }
	       return;
	    }
	  // End
	  if(parm == "")
	    {
	       //No topic, no parm.
               
               int nbRes = database.dbSelect("txt", "help",
                    "service='"+service+"' AND word='"+topic+"' AND parm='' AND lang='"+origin.getLanguage()+"' ORDER by id");

               String line="";

               for(int i=0; i<nbRes; i++)
                 {
                    line = parseHelp(database.dbGetValue());
                    serviceNotice(line,service,origin.getNickname());
                    database.dbGetRow();
                 }

	       return;
	    }
	  // End

               int nbRes = database.dbSelect("txt", "help",
                    "service='"+service+"' AND word='"+topic+"' AND parm='"+parm+"' AND lang='"+origin.getLanguage()+"' ORDER by id");

               String line="";

               for(int i=0; i<nbRes; i++)
                 {
                    line = parseHelp(database.dbGetValue());
                    serviceNotice(line,service,origin.getNickname());
                    database.dbGetRow();
                 }
               return;
            }

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
          database.dbInsert("emails", "'','"+to+"','"+subject+"','"+text+"'");
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
	  //	  String thenick = origin.IRCtoLower();
	  //	  User *ptr = findUser(thenick);
	  String nicks = origin.getIDList();
	  String ident = origin.getIdent();
	  String host = origin.getHost();
          database.dbInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','"+cname+"'");
       }

   void
     Services::log (User& origin, String const &service, String const &text)
       {
	  //	  String thenick = nick.IRCtoLower();
	  //	  User *ptr = findUser(thenick);
	  String nicks = origin.getIDList();
	  String ident = origin.getIdent();
	  String host = origin.getHost();
          database.dbInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"',''");
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
	queueAdd(":" + config.getServicesHostname() + " SJOIN " +
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
	  if(!isNickRegistered(nick))
	    {
	       return false;
	    }

          if( database.dbSelect("privmsg", "nicks", "nickname='"+nick+"'") < 1 )
            return false;

          if( database.dbGetValue() == "1")
            return true;
          else
            return false;
       }

   void
     Services::serviceKick(String const &chan, String const &nick, String const &reason)
       {
	  queueAdd (String (":Chan KICK ")+chan+" "+nick+" :"+reason);
       }

   bool
     Services::isOp(String const &nick, String const &chan)
       {
	  int chanid = channel.getOnlineChanID(chan);
	  int nickid = locateID(nick);
          if( database.dbSelect("status", "chanstatus", "chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'") < 1)
            return false;

          if( database.dbGetValue().toInt() == 2 )
            return true;
          else
            return false;
       }

   bool
     Services::isVoice(String const &nick, String const &chan)
       {
          int chanid = channel.getOnlineChanID(chan);
          int nickid = locateID(nick);
          if( database.dbSelect("status", "chanstatus", "chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'") < 1 )
            return false;

          if( database.dbGetValue().toInt() == 1 )
            return true;
          else
            return false;
       }

   int
     Services::countNotes(String const &who)
       {
          return database.dbCount("notes", "nto='"+who+"'");
       }

   void
     Services::sendNote(String const &from, String const &to, String const &text)
       {
	  String thenick = to.IRCtoLower();
          database.dbInsert("notes", "'','"+from+"','"+to+"',NOW(),'"+text+"'");
	  int foo = locateID(thenick);
	  if(foo>0)
	    {
	       User *ptr = findUser(thenick);
	       //Client is online.. But are they identified HUHUHUH?!!?
	       if(ptr->isIdentified(to))
		 {
		    String togo = String("\002[\002New Note\002]\002 From \002")+from+"\002";
		    serviceNotice(togo,"Note",to);
		 }
	    }

       }

// TODO: check why the 2 lines are commented
   void
Services::checkpoint(void)
{
	  //Any nick mods to be done? :-)

          int nbRes = database.dbSelect("kills");

          for(int i=0; i<nbRes; i++)
	    {
	       String id = database.dbGetValue(0);
	       String killt = database.dbGetValue(2);
	       int nowt = currentTime;
	       String tomod = database.dbGetValue(1);
               database.dbGetRow();

	       if(killt.toInt() < nowt)
		 {
                    database.dbDelete("kills", "id='"+id+"'");
		    int foo = 0;
		    bool running = true;
		    String newnick = "";
		    while(running)
		      {
			 foo++;
			 newnick = tomod+String::convert(foo);
			 if(!isNickRegistered(newnick))
			   {
			      if(locateID(newnick)==0)
				{
				   //Oke we can use this one :)
				   running = false;
				}
			   }

		      }
		    User *ptr = findUser(tomod);
		    if(ptr==0)
		      {
			 std::cout << "I could not find the users record.. hence i can't change their nick " << tomod << std::endl;
			 return;
		      }

		    String msg = "Non-Identification: Your nickname is now being changed";
		    serviceNotice(msg,"Nick",tomod);
		    String togo = String(":services.ircdome.org MODNICK ")+tomod+" "+newnick+" :0";
		    queueAdd(String(togo));
		    //		    setNick(*ptr,newnick);
		    //		    database.query("UPDATE onlineclients set nickname='"+newnick+"' WHERE nickname='"+tomod+"'");
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


unsigned long Services::random(unsigned long max)
{
   return (unsigned long)(((max+1.0) * rand()) / RAND_MAX);
}

bool
  Services::queueFlush(void)
{
   if(connected)
     {
	if (socky.isOkay())
	  {
	     if (socky.write (outputQueue.front ()))
	       {
		  std::cout << "DEBUG TX:" << outputQueue.front() << std::endl;
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
	     std::cout << "Socky is dead" << std::endl;
	     connected = false;
	     return false;
	  }
	return false;

     }
   else
     {
	std::cout << "Trying to queueflush when disconnected ?!" << std::endl;
	return false;
     }
};
/* addChan(String,Int)
 * 
 * Add a channel to our channel map
 * 
 */
dChan* const Services::addChan(const String& name, const int oid)
{
   return chans[name] = new dChan(name,oid,*this);
}

/* addUser(String,Int)
 *
 * Add the user to our users map
 *
 */
User* const Services::addUser(const String& name, const int oid)
{
   return users[name] = new User(name,oid,*this);
}

/* findUser(String)
 *
 * Find and return a pointer to a user.
 *
 */
User*
  Services::findUser(String &name)
{
   std::cout << "findUser() - Looking for " << name.IRCtoLower().trim() << std::endl;
   User *ptr = users[name.IRCtoLower().trim()];
   if(ptr == 0)
     {
	std::cout << "findUser() - I could not find the user named :" << name << std::endl;
	return 0;
     }

   return ptr;
}

/* findChan(String)
 * 
 * Find and return a pointer to that channel.
 * 
 */
dChan*
  Services::findChan(String &name)
{
   std::cout << "findChan() - Looking for " << name.IRCtoLower().trim() << std::endl;
   dChan *ptr = chans[name.IRCtoLower().trim()];
   if(ptr==0)
     {
	std::cout << "findChan() - I could not find the channel" << std::endl;
	return 0;
     }
   return ptr;
}

/* delUser(String)
 *
 * Find.. and delete the given user.
 *
 */
bool
  Services::delUser(String &name)
{
   users.erase(name.IRCtoLower());

   database.dbDelete("onlineclients", "nickname='"+name+"'");

   return true;
};

/* delChan(String)
 * 
 * Delete the given channel.
 * 
 */
bool
  Services::delChan(String &name)
{
   chans.erase(name.IRCtoLower());
   database.dbDelete("onlinechan","name='"+name.IRCtoLower()+"'");
   return true;
}

/* setNick(User,String)
 *
 * Find and update the given user record as having
 * a new nickname.
 *
 */
void
  Services::setNick(User &who, String &newnick)
{
   std::cout << "setNick: " << newnick << std::endl;
   user_map::iterator user = users.find(who.getNickname().IRCtoLower());
   users.erase(user);
   users[newnick] = &who;
   who.setNick(newnick);

   // Update nickname in table onlineclients
   database.dbUpdate("onlineclients", "nickname='"+newnick+"'", "id="+who.getOnlineIDString());
};

/* getRegNickCount()
 *
 * Return the total number of registered nicknames (as a String)
 *
 */

String
  Services::getRegNickCount(void)
{
   return String::convert(database.dbCount("nicks"));
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
bool
  Services::isAuthorised(String const &server)
{
/*
   if( database.dbSelect("id", "serverlist", "name='"+server+"'") < 1 )
      return false;
   else
   {
     if( database.dbGetValue().toInt() > 0 )
       return true;
     else
       return false;
   }
*/
return true;
};

/* addClient(...)
 *
 * Add a user and setup a new map entry for them...
 * ..
 */

User*
  Services::addClient(String const &nick, String const &hops,
		      String const &timestamp, String const &username,
		      String const &host, String const &vwhost,
		      String const &server, String const &modes,
		      String const &realname)
{
   database.dbInsert("onlineclients", "'','"+nick.toLower()+"','"
                  +hops + "','" + timestamp + "','" + username + "','"
                  +host + "','" + vwhost + "','" + server + "','"
                  +modes+ "','" + realname + "'");
   int foo = locateID(nick);
   String client = nick.IRCtoLower();
   User *ptr = addUser(client,foo);
   return ptr;
};

/* locateID....
 *
 * shouldn't be here.. really .. just a stop gap for now
 *
 */
int
  Services::locateID(String const &nick)
{

   // Saftey Check - Remove any special chars.
   String newnick = nick.IRCtoLower();

   if( database.dbSelect("id", "onlineclients", "nickname='"+newnick+"'") < 1 )
   {
     Debug("ERROR: Returning 0 (NOT KNOWN) from getOnlineNickID");
     return 0;
   }
   else
     return database.dbGetValue().toInt();
}

/* getRequiredAccess(String,String)
 *
 * Find ( and return ) the required access to run a command on
 * with the given service.
 *
 */

int
  Services::getRequiredAccess(String const &service, String const &command)
{
   if( database.dbSelect("access", "commands", "service='"+service+"' AND command='"+command+"'") < 1 )

     /* If no match return 999 - to ensure no-one can use the command
      * since its obvious a command entry hasn't been set up for it */
      return 999;

   else
      return database.dbGetValue().toInt();
};

/* isNickRegistered(String)
 *
 * Return TRUE if the given nickname is registered, otherwise false
 *
 */

bool
  Services::isNickRegistered(String const &nick)
{
   if( database.dbSelect("id", "nicks", "nickname='"+nick+"'") < 1 )
     return false;
   else
   {
     if( database.dbGetValue().toInt() > 0 )
        return true;
     else
        return false;
   }
}

/*
 * getPass(String)
 *
 * Return the password for the given nickname
 *
 */

String
  Services::getPass(String const &nick)
{
   if( database.dbSelect("password", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
   {
     String ret(database.dbGetValue().data(),(String::size_type)20);
      std::cout << "Pass DEBUG: Size of return is" << ret.length() << std::endl;
     return ret;
   }

//	String password((char *)row[0],(String::size_type)20);
}

/*
 * stripModes (String)
 *
 * Strip any mode based characters from a nickname @,+ etc.
 *
 */

String
  Services::stripModes(String const &nick)
{
   char *temp = new char[nick.length() + 1];
   for (register unsigned int i = (nick.length() + 1); i--;)
     {

	char ch = nick.c_str()[i];
	switch(ch)
	  {

	   case '@': /* Opped.. */
	     temp[i] = ' ';
	     continue;
	   case '+': /* Voiced */
	     temp[i] = ' ';
	     continue;
	   default:
	     temp[i] = ch;
	  }

     }
   String result(temp);
   delete(temp);
   return result.trim();

}

/* getRegisteredNickID(String)
 *
 * Return the unique identifier for a nickname if it is registered
 *
 */

int
  Services::getRegisteredNickID(String const &nick)
{
   if( database.dbSelect("id", "nicks", "nickname='"+nick+"'") < 1 )
     return 0;
   else
     return database.dbGetValue().toInt();
}

/* modeIdentify(String)
 *
 * Causes services to set a user client +r
 * meaning they have identified to the nickname
 * they are currently using.
 *
 * as an after thought , maybe this should be an inline function? thoughts?
 */

void
  Services::modeIdentify(String const &nick)
{
   queueAdd(":services.ircdome.org SVSMODE "+nick+" +r");
   return;
}

/*
 * updateLastID(String)
 *
 * Updates the last identified time of a client
 * to now.
 *
 */

void
  Services::updateLastID(String const &nick)
{
   database.dbUpdate("nicks", "lastid=NOW()", "nickname='"+nick+"'");
}

/*
 * getNick(int)
 *
 * Return the nickname for the given registered nickname ID
 *
 */
String
  Services::getNick(int const &id)
{
   if( database.dbSelect("nickname", "nicks", "id='"+String::convert(id)+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/*
 * getOnlineNick(int)
 *
 * Same as above except for Online ID's
 *
 */
String
  Services::getOnlineNick(int const &id)
{
   if( database.dbSelect("nickname", "onlineclients", "id='"+String::convert(id)+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/*
 * getpendingCode(String)
 *
 * return the pending authorisation code for a nickname.
 *
 */

String
  Services::getpendingCode(String const &nick)
{
   if( database.dbSelect("auth", "pendingnicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
};

/*
 * registerNick(String,String,String)
 *
 * register the given nickname :-)
 *
 */

void
  Services::registerNick(String const &nick, String const &password, String const &email)
{
   String gpass = generatePassword(nick.IRCtoLower(),password);
   database.dbInsert("nicks", "'','"+nick.IRCtoLower()+"','"+gpass+"','" + email + "',NOW(),NOW(),'',0,'english','0','None','http://www.ircdome.org',0,'None Set','None Set','No Quit Message Recorded',1)");
}

/* genAuth(String)
 *
 * Generate a new auth code the given nickname (and return it!)
 *
 */

String
  Services::genAuth(String const &nickname)
{
   String authcode = Kine::Utils::SHA1::digestToStr(Kine::Password::makePassword("VIVA LA FRANCE :)",nickname),PasswordStrBase,PasswordStrBaseLongPad);
   database.dbInsert("pendingnicks", "'','"+nickname+"','"+authcode+"'");
   Debug("New registration: "+nickname);
   return authcode;
}

/* getURL(String)
 *
 * Find and return the URL for the given nickname.
 *
 */

String
  Services::getURL(String const &nick)
{
   if( database.dbSelect("url", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
};

/* getMSN(String)
 *
 * Find and return the MSN for a nick.
 *
 */

String
  Services::getMSN(String const &nick)
{
   if( database.dbSelect("msn", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/* getYAHOO(String)
 *
 * Find and return the YAHOO! for a nick.
 *
 */

String
  Services::getYAHOO(String const &nick)
{
   if( database.dbSelect("yahoo", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
};

/* getAIM(String)
 *
 * Return the AIM setting for a nickname.
 *
 */

String
  Services::getAIM(String const &nick)
{
   if( database.dbSelect("aim", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
};

/* getICQ()
 *
 * Return the ICQ for a nick.
 *
 */

String
  Services::getICQ(String const &nick)
{
   if( database.dbSelect("icq", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/* getLanguage(String)
 *
 * return the language setting for a nick.
 *
 */

/*
String
  Services::getLanguage(String const &nick)
{
   if(!isNickRegistered(nick))
     {
	return "english";
     }

   if( database.dbSelect("lang", "nicks", "nickname='"+nick+"'") < 1 )
     return ""english;
   else
     return database.dbGetValue();
}
*/

/* getEmail
 *
 * Retrieve the nicknames email
 *
 */

String
  Services::getEmail(String const &nick)
{
   if( database.dbSelect("email", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/* getRegDate - return the registration date for a client.
 */

String
  Services::getRegDate(String const &nick)
{
   if( database.dbSelect("registered", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/* getLastID - return the date a client last identified */

String
  Services::getLastID(String const &nick)
{
   if( database.dbSelect("lastid", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/* getLastHost - get last host */
String
  Services::getLastHost(String const &nick)
{
   if( database.dbSelect("lasthost", "nicks", "nickname='"+nick+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

void
  Services::addOper(String const &nick, int access)
{
   database.dbInsert("onlineopers", "''," + String::convert(getRegisteredNickID(nick)) + "," +String::convert(access));
}

void
  Services::delOper(String const &nick)
{
   database.dbDelete("onlineopers", "nickid="+String::convert(getRegisteredNickID(nick)));
}

bool
  Services::isOper(String const &nick)
{
   if( database.dbSelect("id", "onlineopers", "nickid="+ String::convert(getRegisteredNickID(nick))) < 1 )
     return false;
   else
     return true;
}

void
  Services::validateOper(String &origin)
{
   //Active Oper? (hah :-)
   User *ptr = findUser(origin);
   int axs = ptr->getAccess("Oper");

   if(axs==0)
     {
	//Non-Authorised.
	String tosend = origin+" just tried to become an IRC Operator - \002No Access\002";
	globop(tosend,"Oper");
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
	globop(tosend,"Oper");
	String reason = "You are suspended - Do not try to become an Operator";
	killnick(origin, "Oper", reason);
	return;
     }
   if(axs>0)
     {
	String tosend = origin+" just became an IRC Operator - level "+String::convert(axs);
	globop(tosend,"Oper");

	if (!isOper(origin))
	  addOper(origin, axs);
	else
	  {
	     std::cout << "Warning: inconsistency in ValidateOper: new oper already in onlineopers!" << std::endl;
	  }

	return;
     }

}

/* Debug
 *
 * Decide if/where to send our debug messages
 *
 */

void
  Services::Debug(AISutil::String const &line)
{
   String debugline = line;
   queueAdd(":IRCDome PRIVMSG #Exordium :"+debugline);

}

int
  Services::getAccess(String &service, String &nickname)
{
   if( database.dbSelect("access", "access", "nickname='"+nickname+"' AND service='"+service+"'") < 1 )
     return 0;
   else
   {
     std::cout << "AXS:" << database.dbGetValue() << std::endl;
     return database.dbGetValue().toInt();
   }
}
