/*
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/services.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <fstream>
#include "exordium/sql.h"
#include "exordium/log.h"
#include "exordium/parser.h"
#include "exordium/service.h"
#include "exordium/utils.h"
#include "exordium/module.h"
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

#include "socket/sockets.h"

using Kine::String;
using Kine::StringTokens;
using Kine::Password;
using Kine::StringMask;
using Kine::Signals;
using namespace Exordium;

#define CONFIG "services.conf"
#define UPLINK "services-hub.ircdome.org"

KINE_SIGNAL_HANDLER_FUNC(Rehash)
{
   String reason = "\002[\002Rehash\002]\002 Services has received the REHASH signal - commiting database";
   ((Services *)foo)->helpme(reason,"Serv");
}

KINE_SIGNAL_HANDLER_FUNC(Death)
{
   String reason = "\002[\002Fatal Error\002]\002 Services received \002"+String(sys_siglist[signal]) + "\002 - Terminating";
   ((Services *)foo)->helpme(reason, "Serv");
   ((Services *)foo)->shutdown(reason);
}

namespace Exordium
{

   //namespace Services {

   //  int sock = -1;
   //  int maxSock = -1;
   //  char *inputBuffer = 0;
   //  unsigned int inputBufferSize = 512;
   //  unsigned int inputBufferPosition = 0;
   //  struct sockaddr_in addr;
   //  String UplinkHost = "";
   //  time_t startTime;
   //  time_t lastPing;
   //  time_t currentTime;
   //  time_t serverLastSpoke;
   //  time_t disconnectTime = 0;
   //  time_t stopTime = 0;
   //  time_t lastCheckPoint;
   //  time_t lastExpireRun;
   //  time_t lastModeRun;
   //  bool connected = false;
   //  bool stopping = false;
   //  bool sentPing = false;
   //  bool burstOk = false;
   //  unsigned long countTx = 0;
   //  unsigned long countRx = 0;
   //  unsigned long remoteAddress;
   //    std::queue < String > outputQueue;
   //    std::queue < String > ModeoutputQueue;
   //  bool SecurePrivmsg = false;
   //  Core serviceM;
   //  Kine::SocketIPv4TCP socky;
   //};

/* Services run */
   void
     Services::run(void)
       {
	  fd_set inputSet, outputSet;
	  struct timeval timer;
	  disconnectTime = 0;
	  connected = false;
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

   Services::Services(Kine::Daemon& d, Log& l, Sql& db, const Config& c)
     : daemon(d),
   logger(l),
   database(db),
   config(c),
   parser(*this),
   nickname(*this),
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
	logger.logLine("Setting up signal handlers");
	getDaemon().getSignals().addHandler(&Rehash, Signals::REHASH, (void *)this);
	getDaemon().getSignals().addHandler(&Death,
					    Signals::VIOLENT_DEATH | Signals::PEACEFUL_DEATH,
					    (void *)this);

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
	if ((host = gethostbyname ("chrome.tx.us.ircdome.org")) == NULL)
	  {
	     logger.logLine ("Fatal Error: Error resolving uplinkhost");
	     exit (1);
	  }
	memcpy (&addr.sin_addr, host->h_addr_list[0], host->h_length);
	addr.sin_port = htons (6667);
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

/* Handle Input */
   bool Services::handleInput (void)
     {
	std::stringstream bufferin;
	socky.read(bufferin);
	String line;
	while(bufferin.peek()!=-1)
	  {
	     std::getline(bufferin,line);
	     logger.logLine("RX: "+line);
	     parser.parseLine(line);
	  }

	return true;
     };

/* Disconnect from Server */
   void
     Services::disconnect (void)
       {
	  logger.logLine ("Closing socket.");
	  socky.close();
	  connected = false;
       }

/* Connect to Server */
   bool Services::connect (void)
     {
	logger.logLine ("Attempting Connection to Uplink");
	if (sock >= 0)
	  {
	     logger.logLine ("Closing stale network socket");
	     socky.close();
	     sock = -1;
	  }
	socky.setRemoteAddress("209.51.139.254");
	socky.setRemotePort(6667);
	if(!socky.connect())
	  {
	     std::cout << "Socky.connect() returned an error" << std::endl;

	  }
	connected = true;
	std::cout << "CONNECTED. CONNECTED" << std::endl;
	logger.logLine ("Beginning handshake with uplink");
	maxSock = socky.getFD() + 1;
	queueAdd ("PASS pass :TS");
	queueAdd ("CAPAB TS3 BURST UNCONNECT NICKIP");
	queueAdd ("SERVER services.ircdome.org 1 :IRCDome Network Services");
	queueAdd ("SERVER ircdome.org 2 :IRCDome Console");
	queueAdd ("SVINFO 3 1 0 :"+String::convert(currentTime));
	queueAdd (":services.ircdome.org EOB");
	queueAdd ("BURST");
	doBurst ();
	queueAdd ("BURST 0");
	return true;
     };

/* Send Services Connection Burst */
   void
     Services::doBurst (void)
       {
	  loadModule("nick","./modules/nick.so");
	  loadModule("chan","./modules/chan.so");
	  loadModule("serv","./modules/serv.so");
	  loadModule("note","./modules/note.so");
	  loadModule("bot","./modules/bot.so");
	  loadModule("game","./modules/game.so");
	  loadModule("love","./modules/love.so");
	  loadModule("oper","./modules/oper.so");
	  loadModule("stats","./modules/stats.so");
	  loadModule("vote","./modules/vote.so");
	  registerService("IRCDome", "ircdome", "ircdome.org", "+dz",
			  "The service James forgot :(");
	  serviceJoin ("IRCDome", "#debug");
	  serviceJoin ("IRCDome", "#Exordium");
	  connected = true;
	  serviceM.dumpModules();
	  return;
       }

   String Services::getQuote(int const &number)
     {
	String query = "SELECT body from fortunes where id='" + String::convert(number) + "'";
	MysqlRes res = database.query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String foo = ((std::string) row[0]);
	     res.free_result();
	     return foo;
	  }
	res.free_result();
	return String("");
     }

   String Services::getLogCount(void)
     {
	String query = "SELECT count(*) from log";
	MysqlRes res = database.query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String foo = ((std::string) row[0]);
	     res.free_result();
	     return foo;
	  }
	res.free_result();
	return String("0");
     }

   String Services::getNoteCount(void)
     {
	String query = "SELECT count(*) from notes";
	MysqlRes res = database.query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String foo = ((std::string) row[0]);
	     res.free_result();
	     return foo;
	  }
	res.free_result();
	return String("0");
     }

   String Services::getGlineCount(void)
     {
	String query = "SELECT count(*) from glines";
	MysqlRes res = database.query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	  {
	     String foo = ((std::string) row[0]);
	     res.free_result();
	     return foo;
	  }
	res.free_result();
	return String("0");
     }

   void Services::shutdown(String const &reason)
     {
	helpme("Services is shutting down "+reason,"IRCDome");
	queueAdd(":Chan QUIT :"+reason);
	queueAdd(":Nick QUIT :"+reason);
	queueAdd(":Love QUIT :"+reason);
	queueAdd(":Note QUIT :"+reason);
	queueAdd(":Serv QUIT :"+reason);
	queueAdd(":IRCDome QUIT :"+reason);
	queueAdd(":Oper QUIT :"+reason);
	queueAdd(":services.ircdome.org SQUIT chrome.tx.us.ircdome.org :"+reason);
	stopping = true;
	stopTime = currentTime + 10;
     }

   void Services::SynchTime(void)
     {
	//Undo any expired glines

	//Undo any expired channel bans
	String ctime = String::convert(currentTime);
	String query = "SELECT id,chan,mask from chanbans where expireon<" + ctime;
	MysqlRes res = database.query(query);
	MysqlRow row;
	while (( row = res.fetch_row()))
	  {
	     String id = ((std::string) row[0]);
	     String chan = ((std::string) row[1]);
	     String mask = ((std::string) row[2]);
	     channel.RemoveBan(id,chan,mask);
	  }

	//Lastly commit any outstanding db changes.
     }

   void Services::expireRun(void)
     {
	String nc = nickname.getRegNickCount();
	String cc = channel.getChanCount();
	String oc = nickname.getOnlineCount();
	String lc = getLogCount();
	String gc = getGlineCount();
	String noc = getNoteCount();
	unsigned long rx = getCountRx();
	unsigned long tx = getCountTx();
	String togo = String("NC [\002")+nc+"\002] CC [\002"+cc+"\002] OC [\002"+oc+"\002] LC [\002"+lc+"\002] GC [\002"+gc+"\002] NOC [\002"+noc+"\002] RX [\002"+String::convert(rx)+"\002] TX [\002"+String::convert(tx)+"\002]";
	servicePrivmsg(String(togo),"Oper","#Debug");

     }

   void
     Services::AddOnlineServer (String const &servername, String const &hops, String const &description)
       {
	  String query = "INSERT into onlineservers values ('','" +
	    servername + "','" + hops + "','" + description + "')";
	  database.query(query);
       }

   void
     Services::doPong (String const &line)
       {
	  queueAdd (String (":services.ircdome.org PONG ") + line);
       }

   void
     Services::mode (String const &who, String const &chan, String const &mode,
		     String const &target)
       {
	  queueAdd (":"+who+" MODE "+chan+ " " + mode + " " + target);
       }

   void
     Services::doHelp(String const &nick, String const &service,
		      String const &topic, String const &parm)
       {
	  String lang = nickname.getLanguage(nick);
	  if(topic == "")
	    {
	       //No topic, no parm.
	       String query = "SELECT txt from help where service='"+service+"' AND word='' AND parm='' AND lang='"+lang+"' ORDER by id";
	       MysqlRes res = database.query(query);
	       MysqlRow row;
	       while ((row = res.fetch_row()))
		 {
		    String line = ((std::string) row[0]);
		    line = parseHelp(line);
		    serviceNotice(line,service,nick);
		 }
	       res.free_result();
	       return;
	    }
	  // End
	  if(parm == "")
	    {
	       //No topic, no parm.
	       String query = "SELECT txt from help where service='"+service+"' AND word='"+topic+"' AND parm='' AND lang='"+lang+"' ORDER by id";
	       MysqlRes res = database.query(query);
	       MysqlRow row;
	       while ((row = res.fetch_row()))
		 {
		    String line = ((std::string) row[0]);
		    line = parseHelp(line);
		    serviceNotice(line,service,nick);
		 }
	       res.free_result();
	       return;
	    }
	  // End
	  String query = "SELECT txt from help where service='"+service+"' AND word='"+topic+"' AND parm='"+parm+"' AND lang='"+lang+"' ORDER by id";
	  MysqlRes res = database.query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String line = ((std::string) row[0]);
	       line = parseHelp(line);
	       serviceNotice(line,service,nick);
	    }
	  res.free_result();
	  return;

       }
   void
     Services::sendEmail (String const &to, String const &subject, String const &text)
       {
	  String query = "INSERT into emails values ('','"+to+"','"+subject+"','"+text+"')";
	  database.query(query);
       }

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

   void
     Services::log (String const &nick, String const &service, String const &text, String const &cname)
       {
	  String nicks = nickname.getIDList(nick);
	  String ident = nickname.getIdent(nick);
	  String host = nickname.getHost(nick);
	  String query = "INSERT DELAYED into log values('','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','"+cname+"')";
	  database.query(query);
       }

   void
     Services::log (String const &nick, String const &service, String const &text)
       {
	  String nicks = nickname.getIDList(nick);
	  String ident = nickname.getIdent(nick);
	  String host = nickname.getHost(nick);
	  String query = "INSERT DELAYED into log values('','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','')";
	  database.query(query);
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

   void
     Services::servicePart(String const &service, String const &target)
       {
	  queueAdd (String (":") + service + " PART " + target);
       }

   bool
     Services::usePrivmsg (String const &nick)
       {
	  if(!nickname.isNickRegistered(nick))
	    {
	       return false;
	    }
	  String query = String("SELECT privmsg from nicks where nickname='") + nick + "'";
	  MysqlRes res = database.query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       if(foo=="1")
		 return true;
	    }
	  return false;
       }

   bool
     Services::unloadModule(String const &name)
       {
	  serviceM.delModule(name);
	  serviceM.delModule(name);
	  return true;
       }
   bool
     Services::loadModule (String const &name, String const &fileName)
       {
	  void *handle;
	  handle = dlopen(fileName.c_str(), RTLD_NOW);
	  if(!handle)
	    {
	       String togo = "\002[\002Module Error\002]\002 Could not load "+fileName;
	       Debug(String(togo));
	       String foo = dlerror();
	       Debug("\002[\002Module Error\002]\002 dlError() returned: "+foo);
	       return false;
	    }
	  EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN((*initfunc)) =
	    ((EXORDIUM_SERVICE_INIT_FUNCTION_NO_EXTERN((*)))
	     (dlsym(handle, "service_init")));

	  if (initfunc == 0)
	    {
	       String togo = "\002[\002Module Error\002]\002 Module does not contain an init function";
	       Debug(togo);
	       return false;
	    }

	  Module *modInfo = (*initfunc)(*this, name);
	  serviceM.addModule(modInfo->modName, *modInfo->modService, handle);
	  return true;
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
	  int nickid = nickname.getOnlineNickID(nick);
	  String query = "SELECT status from chanstatus where chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'";
	  MysqlRes res = database.query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String status = ((std::string) row[0]);
	       res.free_result();
	       if(status.toInt() == 2)
		 {
		    return true;

		 }
	    }
	  return false;
       }

   bool
     Services::isVoice(String const &nick, String const &chan)
       {
	  int chanid = channel.getOnlineChanID(chan);
	  int nickid = nickname.getOnlineNickID(nick);
	  String query = "SELECT status from chanstatus where chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'";
	  MysqlRes res = database.query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String status = ((std::string) row[0]);
	       res.free_result();
	       if(status.toInt() == 1)
		 {
		    return true;

		 }
	    }
	  return false;
       }

   int
     Services::countNotes(String const &who)
       {
	  String query = "select count(*) from notes where nto='" + who + "'";
	  MysqlRes res = database.query(query);
	  MysqlRow row;
	  int j = 0;
	  while (( row = res.fetch_row()))
	    {
	       j++;
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       return foo.toInt();
	    }
	  //No notes.
	  return 0;
       }

   void
     Services::sendNote(String const &from, String const &to, String const &text)
       {
	  String query = String("insert into notes values('','")+from+"','"+to+"',NOW(),'"+text+"')";
	  database.query(query);
	  int foo = nickname.getOnlineNickID(to);
	  if(foo>0)
	    {
	       //Client is online.. But are they identified HUHUHUH?!!?
	       if(nickname.isIdentified(to,to))
		 {
		    String togo = String("\002[\002New Note\002]\002 From \002")+from+"\002";
		    serviceNotice(togo,"Note",to);
		 }
	    }

       }

   void
     Services::checkpoint(void)
       {
	  //Any nick mods to be done? :-)
	  String query = "SELECT * from kills";
	  MysqlRes res = database.query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String id = ((std::string) row[0]);
	       String killt = ((std::string) row[2]);
	       int nowt = currentTime;
	       String tomod = ((std::string) row[1]);
	       res.free_result();
	       if(killt.toInt() < nowt)
		 {
		    String query = "DELETE from kills where id='" + id + "'";
		    int foo = 0;
		    bool running = true;
		    String newnick = "";
		    while(running)
		      {
			 foo++;
			 newnick = tomod+String::convert(foo);
			 if(!nickname.isNickRegistered(newnick))
			   {
			      if(nickname.getOnlineNickID(newnick)==0)
				{
				   //Oke we can use this one :)
				   running = false;
				}
			   }

		      }
		    String msg = "\002[\002Non-Identification\002]\002 Your nickname is now being changed";
		    serviceNotice(msg,"Nick",tomod);
		    String togo = String(":services.ircdome.org MODNICK ")+tomod+" "+newnick+" :0";
		    queueAdd(String(togo));

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

