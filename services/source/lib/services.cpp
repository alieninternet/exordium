/* $Id$
 *
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "services.h"
#include "exordium/channel.h"
#include "exordium/user.h"
#include "exordium/utils.h"
#include "core_language.h"
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
#include "exordium/service.h"
#include <kineircd/kineircdconf.h>
#include <kineircd/signals.h>

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

/** Our rehash handler.  Contains things we do upon receiving a REHASH */
KINE_SIGNAL_HANDLER_FUNC(Rehash)
{
   ((Services*)foo)->logLine("\002[\002Rehash\002]\002 Services has received "
			     "the REHASH signal");
}

/** Our death handler.  Contains things we do upon receiving DEATH */
KINE_SIGNAL_HANDLER_FUNC(Death)
{
   String reason =
     "\002[\002Fatal Error\002]\002 Services received \002" +
     String(sys_siglist[signal]) + "\002 - Initiating shutdown";
   ((Services *)foo)->logLine(reason, Log::Warning);
   ((Services *)foo)->shutdown(reason);
   exit(1); /* Until kine's signal thingies are fixed :C */
}
/** Run() Called from the module to begin operation.
 * This begins the main 'loop' for services, initiates our connection
 * etc.
 */

void
  ServicesInternal::run(void)
{
   fd_set inputSet, outputSet;
   struct timeval timer;
   disconnectTime = 0;
   connected = false;
   lastExpireRun = 0;
   buildNumber = 1;
   srand(time(NULL));
   logLine ("Cleaning out (any) stale entries from the DB");
   database.dbDelete("onlineclients");
   database.dbDelete("chanstatus");
   database.dbDelete("nicksidentified");
   database.dbDelete("kills");
   database.dbDelete("onlineservers");
   database.dbDelete("onlinechan");
   database.dbDelete("onlineopers");

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
		  handleInput();
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
	if (currentTime > (time_t) (lastExpireRun + 3600))
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

ServicesInternal::ServicesInternal(ConfigInternal& c, CDatabase& db)
  : Services(db),
parser(*this),
console(*this),
config(c),
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

   database.dbConnect();

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
   if ((host = gethostbyname (config.getUplinkHost().c_str())) == NULL)
     {
	logLine ("Fatal Error: Error resolving uplinkhost",
		 Log::Fatality);
	exit (1);
     }
   memcpy (&addr.sin_addr, host->h_addr_list[0], host->h_length);
   addr.sin_port = htons (config.getUplinkPort());
}

/** HandleInput().
 *
 * This handles the incoming data from our uplink, and hands it over
 * to our parser.
 *
 * Original: James Wilkins
 *
 */

bool ServicesInternal::handleInput (void)
{
   std::stringstream bufferin;
   char c;
   if(!socky.read(bufferin))
     {
	std::cout << "Read failed!" << std::endl;
	throw(1); /* Fatal error!*/
	//	return false; /* Fatal error - should d/c */
     }

   if(!bufferin.str().empty())
     {

	for(;;)
	  {
	     if(bufferin.peek()==-1)
	       {
	          return false; /* Not fatal, just not ready */
	       }

	     if(bufferin.peek() == '\0')
	       {
		  (void)bufferin.ignore();
	       }
	     else if ((bufferin.peek()=='\r') || (bufferin.peek() =='\n'))
	       {
		  (void)bufferin.ignore();

		  if((bufferin.peek() == '\r') || (bufferin.peek() == '\n'))
		    {
		       (void)bufferin.ignore();
		    }
		  if(!inputQueue.empty())
		    {
		       countRx += inputQueue.length();
		       parser.parseLine(inputQueue);
		       inputQueue.clear();
		    }
	       }
	     c = bufferin.get();
	     if((int)c!=-1)
	       inputQueue += (char)c;
	  }
	return true;
     }
}

/* disconnect()
 *
 * (Uncleanly?) say bye bye to our uplink..
 *
 */

void
  ServicesInternal::disconnect (void)
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

bool ServicesInternal::connect (void)
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

   socky.setRemoteAddress(config.getUplinkHost());
   socky.setRemotePort(config.getUplinkPort());

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
   queueAdd ("PASS "+config.getUplinkPass()+" :TS");
   queueAdd ("SVINFO 3 3 0 :"+String::convert(currentTime));
   queueAdd ("CAPAB TS3 SSJOIN NICKIP NOQUIT");
   queueAdd ("SERVER " + Kine::config().getOptionsServerName() + " 1 :" +
	     Kine::config().getOptionsDescription());
   if (!config.getUnderlingHostname().empty())
     {
	queueAdd ("SERVER " + config.getUnderlingHostname() + " 2 :" + config.getUnderlingDescription());
     }
   queueAdd (":" + Kine::config().getOptionsServerName() + " EOB");
   queueAdd ("BURST");

   // Start all the modules
   config.getModules().startAll(*this);

   // Is the console actually wanted?
   if (config.getConsoleEnabled())
     {
	registerService(config.getConsoleName(), config.getConsoleName(), /* Hack for now */
			config.getConsoleHostname(),
			config.getConsoleDescription());
     }
   Kine::langs().registerMap(Language::tagMap);
   int foofoo = 0;
   for (;;)
     {
	if (Language::tagMap[++foofoo].tagName == 0)
	  {
	     break;
	  }

     }

   connected = true;
   queueAdd ("BURST 0");
   return true;
};

/* shutdown(String)
 *
 * Initiate a services shutdown with the given reason being
 * propagated accross the network
 *
 */

void ServicesInternal::shutdown(String const &reason)
{
   logLine("Services is shutting down " + reason,
	   Log::Warning);

   // Stop and unload all the modules..
   config.getModules().unloadAll(reason);

   // I do not like this.. oh well..  - pickle
   if (config.getConsoleEnabled())
     {
	queueAdd(config.getConsoleName()+" QUIT :"+reason);
     }

   // Nasty hack for now.
   String tofo = "\002Services shutting down\002 : " + reason;
   queueAdd(":" + Kine::config().getOptionsServerName() +
	    " SQUIT " + Kine::config().getOptionsServerName() + " :" +
	    reason);

   stopping = true;
   stopTime = currentTime + 15;

   // Clean up before we die
   database.dbDelete("onlineclients");
   database.dbDelete("chanstatus");
   database.dbDelete("nicksidentified");
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
 *	Update ServerWide Time.
 */

void ServicesInternal::SynchTime(void)
{
   //Undo any expired glines

   //Undo any expired channel bans
   //time_t ctime = currentTime;
   //int nbRes = database.dbSelect("id,chan,mask", "chanbans", "expireon<"+ctime);
   //for(int i=0; i<nbRes; i++)
   //  {
   //	channel.RemoveBan(database.dbGetValue(0), database.dbGetValue(1), database.dbGetValue(2));
   //	database.dbGetRow();
   //   }
   //
}

void
  ServicesInternal::sendGOper(String const &from, String const &text)
{
   queueAdd(":"+from+" GLOBOPS :"+text);

}

void
  ServicesInternal::sendHelpme(String const &from, String const &text)
{
   queueAdd(":"+from+" HELPME :"+text);
}

/* mode(String,String,String,String)
 *
 * A messy way of sending a mode
 *
 */

void
  ServicesInternal::mode (String const &who, String const &chan, String const &mode,
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
  ServicesInternal::doHelp(User& origin, String const &service,
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
   //
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

/* sendEmail(String,String,String)
 *
 * post an email into the database, which is polled later by a third party
 * utilite to send any pending emails
 *
 */

void
  ServicesInternal::sendEmail (String const &to, String const &subject, String const &text)
{
   database.dbInsert("emails", "'','"+to+"','"+subject+"','"+text+"'");
}

void
  ServicesInternal::setMode(String const &who, String const &mode)
{
   queueAdd(":"+Kine::config().getOptionsServerName()+" MODE "+who+" "+mode);

}
/* parseHelp(In)
 *
 * This parses our special codes in the help files
 * allowing for items such as bold, italics etc
 *
 */

String
  ServicesInternal::parseHelp (String const &instr)
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

unsigned long
  ServicesInternal::getCountTx(void)
{
   return countTx;
}

unsigned long
  ServicesInternal::getCountRx(void)
{
   return countRx;
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
  ServicesInternal::log (User& origin, String const &service, String const &text, String const &cname)
{
   //	  String thenick = origin.IRCtoLower();
   //	  User *ptr = findUser(thenick);
   String nicks = origin.getIDList();
   String ident = origin.getIdent();
   String host = origin.getHost();
   database.dbDelayedInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','"+cname+"'");
}

void
  ServicesInternal::log (User& origin, String const &service, String const &text)
{
   //	  String thenick = nick.IRCtoLower();
   //	  User *ptr = findUser(thenick);
   String nicks = origin.getIDList();
   String ident = origin.getIdent();
   String host = origin.getHost();
   database.dbDelayedInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"',''");
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
  ServicesInternal::servicePart(String const &service, String const &target)
{
   queueAdd (String (":") + service + " PART " + target);
}

void ServicesInternal::serviceJoin(AISutil::String const &service,
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
    */

bool
  ServicesInternal::usePrivmsg (String const &nick)
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
  ServicesInternal::serviceKick(String const &chan, String const &nick, String const &reason)
{
   queueAdd (String (":Chan KICK ")+chan+" "+nick+" :"+reason);
}

bool
  ServicesInternal::isOp(String const &nick, String const &chan)
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
  ServicesInternal::isVoice(String const &nick, String const &chan)
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

void
  ServicesInternal::sendNote(Kine::Name const &from, Kine::Name const &to, String const &text)
{
   Kine::Name thenick = to.IRCtoLower();
   database.dbInsert("notes", "'','"+from+"','"+to+"',NOW(),'"+text+"'");
   int foo = locateID(thenick);
   if(foo>0)
     {
	User *ptr = findUser(thenick);
	if(ptr==0)
	  return;
	//Client is online.. But are they identified HUHUHUH?!!?
	if(ptr->isIdentified(to))
	  {
	     String togo = String("\002[\002New Note\002]\002 From \002")+from+"\002";
	     serviceNotice(togo,"Note",to);
	  }
     }

}

// TODO: check why the 2 lines are commented
void ServicesInternal::checkpoint(void)
{
   /* Decrease network wide flood counters */
   user_map::iterator it;
   for (it = users.begin(); it != users.end(); it++)
     {
	String tmp = (*it).first;
	if(((*it).second)!=0)
	  (*it).second->decFloodCount();
     }

   //Any nick mods to be done? :-)

   int nbRes = database.dbSelect("kills");

   for(int i=0; i<nbRes; i++)
     {
	String id = database.dbGetValue(0);
	String killt = database.dbGetValue(2);
	int nowt = currentTime;
	Kine::Name tomod = database.dbGetValue(1);
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
		      " SVSNICK " + tomod + " " + newnick + " :0");
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

bool ServicesInternal::queueFlush(void)
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

/* addServer(String,int,String)
 * 
 * Add a server to our server map
 * 
 */
Server* const ServicesInternal::addServer(const String &name, const int &hops, const String &desc)
{
   return servers[name] = new Server(name,hops,desc,*this);
}

/* addChan(String,Int)
 *
 * Add a channel to our channel map
 *
 */
dChan* const ServicesInternal::addChan(const Kine::Name& name, const int oid)
{
   return chans[name] = new dChan(name.IRCtoLower(),oid,*this);
}

/* addUser(String,Int)
 *
 * Add the user to our users map
 *
 */
User* const ServicesInternal::addUser(const String& name, const int oid)
{
   return users[name] = new User(name,oid,*this);
}

/* findUser(String)
 *
 * Find and return a pointer to a user.
 *
 */
User* ServicesInternal::findUser(Kine::Name &name)
{
   User *ptr = users[name.IRCtoLower().trim()];
   if(ptr == 0)
     {
	return 0;
     }
   return ptr;
}

/* findChan(String)
 *
 * Find and return a pointer to that channel.
 *
 */
dChan* ServicesInternal::findChan(Kine::Name &name)
{
   dChan *ptr = chans[name.IRCtoLower().trim()];
   if(ptr==0)
     {
	return 0;
     }
   return ptr;
}

/* findServer(String)
 * 
 * Find and return a pointer to that server.
 * 
 */
Server* ServicesInternal::findServer(String &name)
{
   Server *ptr = servers[name];
   if(ptr==0)
     {
	return 0;
     }
   return ptr;
}

/* delUser(String)
 *
 * Find.. and delete the given user.
 *
 */
bool ServicesInternal::delUser(Kine::Name &name)
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
bool ServicesInternal::delChan(Kine::Name &name)
{
   chans.erase(name.IRCtoLower());
   database.dbDelete("onlinechan","name='"+name.IRCtoLower()+"'");
   return true;
}

/* delServer(String)
 * 
 * Delete the given server.
 * 
 */
bool ServicesInternal::delServer(String &name)
{
   servers.erase(name);
   return true;

}

/* setNick(User,String)
 *
 * Find and update the given user record as having
 * a new nickname.
 *
 */
void ServicesInternal::setNick(User &who, Kine::Name &newnick)
{
#ifdef DEBUG
   logLine("setNick: " + newnick, Log::Debug);
#endif
   String fixedNewNick = newnick.IRCtoLower();
   user_map::iterator user = users.find(who.getNickname().IRCtoLower());
   users.erase(user);
   users[fixedNewNick] = &who;
   who.setNick(fixedNewNick);

   // Update nickname in table onlineclients
   database.dbUpdate("onlineclients", "nickname='"+fixedNewNick+"'", "id="+who.getOnlineIDString());
};

/* addClient(...)
 *
 * Add a user and setup a new map entry for them...
 * ..
 */

User* ServicesInternal::addClient(Kine::Name const &nick, String const &hops,
				  String const &timestamp,
				  String const &username, String const &host,
				  String const &server,
				  String const &modes, String const &realname)
{
   database.dbInsert("onlineclients", "'','"+nick.toLower()+"','"
		     +hops + "','" + timestamp + "','" + username + "','"
		     +host + "','"+host+"','" + server + "','"
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
int ServicesInternal::locateID(Kine::Name const &nick)
{

   // Saftey Check - Remove any special chars.
   String newnick = nick.IRCtoLower();

   if( database.dbSelect("id", "onlineclients", "nickname='"+newnick+"'") < 1 )
     {
#ifdef DEBUG
	logLine("ERROR: Returning 0 (NOT KNOWN) from getOnlineNickID",
		Log::Debug);
#endif
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

int ServicesInternal::getRequiredAccess(String const &service,
					String const &command)
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

bool ServicesInternal::isNickRegistered(String const &nick)
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

/* modeIdentify(String)
 *
 * Causes services to set a user client +r
 * meaning they have identified to the nickname
 * they are currently using.
 *
 * as an after thought , maybe this should be an inline function? thoughts?
 */

void ServicesInternal::modeIdentify(String const &nick)
{
   queueAdd(":" + Kine::config().getOptionsServerName() + " SVSMODE " + nick +
	    " +r");
   return;
}

/*
 * updateLastID(String)
 *
 * Updates the last identified time of a client
 * to now.
 *
 */

void ServicesInternal::updateLastID(String const &nick)
{
   database.dbUpdate("nicks", "lastid=NOW()", "nickname='"+nick+"'");
}

/*
 * getNick(int)
 *
 * Return the nickname for the given registered nickname ID
 *
 */
String ServicesInternal::getNick(int const &id)
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
String ServicesInternal::getOnlineNick(int const &id)
{
   if(id==0)
     return "";
   if( database.dbSelect("nickname", "onlineclients", "id='"+String::convert(id)+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/*
 * getOnlineChan(int)
 *
 * Return the unique ID for any given channel
 *
 */
int ServicesInternal::getOnlineChanID(String const &id)
{
   if( database.dbSelect("id", "onlinechan", "name='"+id+"'") < 1 )
     return 0;
   else
     return database.dbGetValue().toInt();
}

void ServicesInternal::addOper(String const &nick, int access)
{
   database.dbInsert("onlineopers", "''," + String::convert(gstatic.getRegisteredNickID(nick)) + "," +String::convert(access));
}

void ServicesInternal::delOper(String const &nick)
{
   database.dbDelete("onlineopers", "nickid="+String::convert(gstatic.getRegisteredNickID(nick)));
}

bool ServicesInternal::isOper(String const &nick)
{
   if( database.dbSelect("id", "onlineopers", "nickid="+ String::convert(gstatic.getRegisteredNickID(nick))) < 1 )
     return false;
   else
     return true;
}

void ServicesInternal::validateOper(Kine::Name &origin)
{
   //Active Oper? (hah :-)
   User *ptr = findUser(origin);
   int axs = ptr->getAccess("Oper");

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
	 */
	killnick(origin, "Oper", reason);
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
	sendGOper("Oper",tosend);

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

time_t ServicesInternal::getStartTime()
{
   return startTime;
}

time_t ServicesInternal::getCurrentTime()
{
   return currentTime;
}

void ServicesInternal::queueAdd(const String& line)
{
   if(connected)
     {
	outputQueue.push(line+"\r\n");
	countTx += line.length();

     }
   else
     {

	// logLine("Tried to TX " + line + " but not connected",
	//         //            Log::Debug);
     }

};
