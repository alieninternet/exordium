/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/services.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <map>
#include <set>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <iomanip>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include "exordium/sql.h"
#include "exordium/log.h"
#include "exordium/parser.h"
#include "exordium/service.h"
#include "exordium/utils.h"
#include "exordium/love.h"
#include "exordium/module.h" 
#include "kineircd/kineircdconf.h"
#include "kineircd/signals.h"

extern "C" {
	#include <unistd.h>
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


static SIGNAL_HANDLER_FUNC(Rehash)
{
	Services::helpme("Services received rehash signal","Serv");
}

Signals::handlerInfo_type rehashSignalHandler = {&Rehash, Signals::REHASH};

static SIGNAL_HANDLER_FUNC(Death)
{
	Services::helpme("Services received " + String(sys_siglist[signal]) + " Shutting down", "Serv");
	exit(0);
}

Signals::handlerInfo_type deathSignalHandler = {&Death, Signals::VIOLENT_DEATH | Signals::PEACEFUL_DEATH };

namespace Exordium {


namespace Services {

  int sock = -1;
  int maxSock = -1;
  char *inputBuffer = 0;
  unsigned int inputBufferSize = 512;
  unsigned int inputBufferPosition = 0;
  struct sockaddr_in addr;
  String UplinkHost = "";
  time_t startTime;
  time_t lastPing;
  time_t currentTime;
  time_t serverLastSpoke;
  time_t disconnectTime = 0;
  time_t lastCheckPoint;
  time_t lastExpireRun;
  time_t lastModeRun;
  bool connected = false;
  bool stopping = false;
  bool sentPing = false;
  bool burstOk = false;
  unsigned long countTx = 0;
  unsigned long countRx = 0;
  unsigned long remoteAddress;
    std::queue < String > outputQueue;
    std::queue < String > ModeoutputQueue;
  bool SecurePrivmsg = false;
  Core serviceM;
  Kine::SocketIPv4TCP socky;
};

/* Services run */
void
Services::run(void)
{
  Signals signals;
  signals.addHandler(rehashSignalHandler);
  signals.addHandler(deathSignalHandler);
  fd_set inputSet, outputSet;
  struct timeval timer;
  Log::logLine ("Entering main loop...");
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
		//cout << "not connected..." << endl;
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
                  Log::logLine ("Error handling server input. Reconnecting.");
		  Services::helpme("Error in handleInput() - Jumping server","Serv");
		  connected = false;
		  disconnectTime = currentTime;
		  disconnect();
                }
            }
          if (FD_ISSET (socky.getFD(), &outputSet))
            {
              if (!queueFlush ())
                {
                  Log::logLine("Disconnecting... (Queue flushing error)");
	          connected = false;
		  disconnectTime = currentTime;
                  disconnect ();
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
          expireRun ();
//          SynchTime ();

        }
      if (currentTime > (time_t) (lastModeRun + 1))
	{
		lastModeRun = currentTime;
		if(ModequeueReady())
		{
			ModequeueFlush();
		}
	}
      if (!connected && (currentTime >= (time_t) (disconnectTime + 10)))
        {
          connect ();
        }
    }
}
/* Services Init */
int
Services::init(void)
{
	struct hostent *host;
	queueKill ();
	startTime = currentTime = lastPing = lastExpireRun = lastCheckPoint = serverLastSpoke = time (NULL);
	if (!(inputBuffer = (char *) malloc (inputBufferSize)))
    	{
      		Log::logLine ("Fatal Error: Could not allocate input buffer");
      		perror ("malloc");
      		exit (1);
    	}
	memset (&addr, 0, sizeof (addr));
        addr.sin_family = AF_INET;
        if ((host = gethostbyname ("chrome.tx.us.ircdome.org")) == NULL)
        {
      		Log::logLine ("Fatal Error: Error resolving uplinkhost");
      		exit (1);
        }
        memcpy (&addr.sin_addr, host->h_addr_list[0], host->h_length);
        addr.sin_port = htons (8888);
        return true;


}

/* Configuration File Parser */
bool
Services::load_config()
{
  int line = 1;
  String UplinkHost = "none";
  std::ifstream file (CONFIG);
  if (!file.is_open ())
    {
      Log::logLine("Error opening file");
    }
  while (!file.eof ())
    {
      file >> buffer;
      if (buffer.length () == 0 || buffer[0] == '#')
        {
          line++;
          continue;
        }

      StringTokens st (buffer);
      String command = st.nextToken (':').trim ().toUpper ();
      String parameters = st.nextToken (':').trim ();
	if(command == "UPLINKHOST")
	{
		UplinkHost = parameters;
	}
	if(command == "LOGFILE")
	{
		logFileName = parameters;		
	}
	if(command == "MYSQLHOST")
	{
		mysqlHost = parameters;
	}
	if(command == "MYSQLPORT")
	{
		mysqlPort = parameters.toInt();
	}
	if(command == "MYSQLUSER")
	{
		mysqlUser = parameters;
	}
	if(command == "MYSQLPASS")
	{
		mysqlPass = parameters;
	}
	if(command == "MYSQLDB")
	{
		mysqlDb = parameters;
	}
    }
file.close();

if(UplinkHost=="none" || logFileName=="none" || mysqlHost=="none" || mysqlDb=="none")
{
Log::logLine("--> Fatal Error whilst parsing configuration");
exit(1);
}
Log::logLine("--> Successfully parsed Configuration file");
return 0;
};


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
  stringstream bufferin;
  socky.read(bufferin);
  String line;
  while(bufferin.peek()!=-1)
  {
  std::getline(bufferin,line);
  Log::logLine("RX: "+line);
  Parser::parseLine(line);	
  }

  return true;
};

/* Disconnect from Server */
void
Services::disconnect (void)
{
  Log::logLine ("Closing socket.");
  socky.close();
  connected = false;
}

/* Connect to Server */
bool Services::connect (void)
{
  Log::logLine ("Attempting Connection to Uplink");
  if (sock >= 0)
    {
      Log::logLine ("Closing stale network socket");
      socky.close();
      sock = -1;
    }
    socky.setRemoteAddress("209.124.83.254");
    socky.setRemotePort(6667);
    if(!socky.connect())
	{
		//cout << "Socky.connect() returned an error" << endl;
		
	}
    Exordium::Services::connected = true;
  Log::logLine ("Beginning handshake with uplink");
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
	loadModule("love","./modules/love.so");
	loadModule("chan","./modules/chan.so");
	loadModule("serv","./modules/serv.so");
	loadModule("note","./modules/note.so");
	// loadModule("note","./modules/game.so"); */
	loadModule("note","./modules/bot.so");
	registerService ("IRCDome", "ircdome", "ircdome.org", "+dz",
				"Services command console");
	registerService ("Chan", "chan", "ircdome.org", "+dz",
				"Channel Registration Services");
	registerService ("Nick", "nick", "ircdome.org", "+dz",
				"Nickname Registration Services");
	registerService ("Note", "note", "ircdome.org", "+dz",
				"Network Note Services");
	registerService ("Serv", "serv", "ircdome.org", "+dz",
				"Services Staff Interface");
	registerService ("Love", "love", "ircdome.org", "+dz",
				"Love Services");
	// registerService ("Game", "game", "ircdome.org", "+dz",
	//			"Network Games"); 
	registerService ("Bot", "bot", "ircdome.org", "+dz",
				"Bot Interface to Services");
	serviceJoin ("Chan", "#Debug");
	serviceJoin ("Nick", "#Debug");
	serviceJoin ("Note", "#Debug");
	serviceJoin ("Serv", "#Debug");
	// serviceJoin ("Game", "#Debug");
	serviceJoin ("Love", "#Debug");
	serviceJoin ("Bot", "#Debug");
	serviceJoin ("IRCDome", "#debug");
	serviceJoin ("IRCDome", "#Exordium");
	serverMode ("#debug", "+oooooo", "Chan Nick Note Serv Game Love");
	serverMode ("#debug", "+oo","IRCDome Bot");
	serverMode ("#Exordium", "+o","IRCDome");
        Channel::synchChannels();
	connected = true;
	serviceM.dumpModules();
	return;
}


String Services::getQuote(int const &number)
{
String query = "SELECT body from fortunes where id='" + String::convert(number) + "'";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
        return foo;
}
return String("");
}

String Services::getLogCount(void)
{
String query = "SELECT count(*) from log";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
        return foo;
}
return String("0");
}

String Services::getNoteCount(void)
{
String query = "SELECT count(*) from notes";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
        return foo;
}
return String("0");
}


String Services::getGlineCount(void)
{
String query = "SELECT count(*) from glines";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
        return foo;
}
return String("0");
}



void Services::expireRun(void)
{
String nc = Nickname::getRegNickCount();
String cc = Channel::getChanCount();
String oc = Nickname::getOnlineCount();
String lc = Services::getLogCount();
String gc = Services::getGlineCount();
String noc = Services::getNoteCount();
unsigned long rx = Services::getCountRx();
unsigned long tx = Services::getCountTx();
String togo = String("NC [\002")+nc+"\002] CC [\002"+cc+"\002] OC [\002"+oc+"\002] LC [\002"+lc+"\002] GC [\002"+gc+"\002] NOC [\002"+noc+"\002] RX [\002"+String::convert(rx)+"\002] TX [\002"+String::convert(tx)+"\002]";
Services::servicePrivmsg(String(togo),"Oper","#Debug");

}

void
Services::AddOnlineServer (String const &servername, String const &hops, String const &description)
{
	String query = "INSERT into onlineservers values ('','" +
		servername + "','" + hops + "','" + description + "')";
	Sql::query(query);
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
Services::Debug("->Entering Help Client "+nick+" Service "+service+" Topic "+topic+" Parm "+parm);
String lang = Nickname::getLanguage(nick);
Services::Debug("-->Client has "+lang+" as their language");
if(topic == "")
	{
		//No topic, no parm.
		String query = "SELECT txt from help where service='"+service+"' AND word='' AND parm='' AND lang='"+lang+"' ORDER by id";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while ((row = res.fetch_row()))
		{
			String line = ((std::string) row[0]).c_str();
			line = Services::parseHelp(line);
			Services::serviceNotice(line,service,nick);
		}
		return;
	} // End
if(parm == "")
	{
		//No topic, no parm.
		String query = "SELECT txt from help where service='"+service+"' AND word='"+topic+"' AND parm='' AND lang='"+lang+"' ORDER by id";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while ((row = res.fetch_row()))
		{
			String line = ((std::string) row[0]).c_str();
			line = Services::parseHelp(line);
			Services::serviceNotice(line,service,nick);
		}
		return;
	} // End
		String query = "SELECT txt from help where service='"+service+"' AND word='"+topic+"' AND parm='"+parm+"' AND lang='"+lang+"' ORDER by id";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		while ((row = res.fetch_row()))
		{
			String line = ((std::string) row[0]).c_str();
			line = Services::parseHelp(line);
			Services::serviceNotice(line,service,nick);
		}
		return;
	
}
void
Services::sendEmail (String const &to, String const &subject, String const &text)
{
String query = "INSERT DELAYED into emails values ('','"+to+"','"+subject+"','"+text+"')";
Sql::query(query);
}

String
Services::parseHelp (String const &instr)
{
String retstr;
for (unsigned int i = 0; i != instr.length(); i++) {
	if (instr[i] == '%' && ((i != 0) && (instr[i - 1] != '\\'))) {
		switch (instr[++i]) {
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
	} else {
		retstr += instr[i];
		}
	}
return retstr;
}

void
Services::log (String const &nickname, String const &service, String const &text, String const &cname)
{
	String nicks = Nickname::getIDList(nickname);
	String ident = Nickname::getIdent(nickname);
	String host = Nickname::getHost(nickname);
	String query = "INSERT DELAYED into log values('','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','"+cname+"')";
	Sql::query(query);
}

void
Services::log (String const &nickname, String const &service, String const &text)
{
	String nicks = Nickname::getIDList(nickname);
	String ident = Nickname::getIdent(nickname);
	String host = Nickname::getHost(nickname);
	String query = "INSERT DELAYED into log values('','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','')";
	Sql::query(query);
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
           (c_str()[s] == '\n'))) {
      s++;
   }
 
   while ((e > s) &&
          ((c_str()[e - 1] == ' ') ||
           (c_str()[e - 1] == '\t') ||
           (c_str()[e - 1] == '\r') ||
           (c_str()[e - 1] == ':') ||
           (c_str()[e - 1] == '@') ||
           (c_str()[e - 1] == '+') ||
           (c_str()[e - 1] == '\n'))) {
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
Services::Debug("Entering usePrivmsg");
	if(!Nickname::isNickRegistered(nick))
	{
		Services::Debug("Not registered destination.. default to .. notice");
		return false;
	}
String query = String("SELECT privmsg from nicks where nickname='") + nick + "'";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
	String foo = ((std::string) row[0]).c_str();
	Services::Debug(foo);
	if(foo=="1")
		return true;
}	
return false;
}

bool
Services::unloadModule(String const &name)
{
	Services::Debug("Unloading Module "+name);
	serviceM.delModule(name);
	serviceM.delModule(name);
	return true;
}
bool
Services::loadModule (String const &name, String const &fileName)
{
void *handle;
handle = dlopen(fileName.c_str(), RTLD_LAZY);
if(!handle)
{
	String togo = "Error: Could not load "+fileName;
	Services::Debug(String(togo));
	String foo = dlerror();
	Services::Debug("dlError() returned: "+foo);
	return false;
}
Module *(*initfunc)(void) =
                (Module *(*)(void))dlsym(handle, "service_init");
if (initfunc == 0) 
		{
			String togo = "Error: Your module does not contain an init function";
			Services::Debug(togo);
			return false;
        	}
        Module *modInfo = (*initfunc)();
        serviceM.addModule(modInfo->modName, *modInfo->modService, handle);
return true;
}

void
Services::serviceKick(String const &channel, String const &nick, String const &reason)
{
	queueAdd (String (":Chan KICK ")+channel+" "+nick+" :"+reason);
}

bool
Services::isOp(String const &nickname, String const &channel)
{
	int chanid = Channel::getOnlineChanID(channel);
	int nickid = Nickname::getOnlineNickID(nickname);
	String query = "SELECT status from chanstatus where chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'";
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	{
		String status = ((std::string) row[0]).c_str();
		if(status.toInt() == 2)
			{
			Services::Debug("Already opped");
			return true;

			}
	}
Services::Debug("Nah... they aint opped");
return false;
}

bool
Services::isVoice(String const &nickname, String const &channel)
{
	int chanid = Channel::getOnlineChanID(channel);
	int nickid = Nickname::getOnlineNickID(nickname);
	String query = "SELECT status from chanstatus where chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'";
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	while ((row = res.fetch_row()))
	{
		String status = ((std::string) row[0]).c_str();
		if(status.toInt() == 1)
			{
			Services::Debug("Already voiced");
			return true;

			}
	}
Services::Debug("Nah... they aint voiced");
return false;
}

int
Services::countNotes(String const &who)
{
	String query = "select count(*) from notes where nto='" + who + "'";
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	int j = 0;
	while (( row = res.fetch_row()))
	{
		j++;
		String foo = ((std::string) row[0]).c_str();
		return foo.toInt(); 
	}
	//No notes.
	return 0;
}


void
Services::sendNote(String const &from, String const &to, String const &text)
{
	String query = String("insert into notes values('','")+from+"','"+to+"',NOW(),'"+text+"')";
	Sql::query(query);
	int foo = Nickname::getOnlineNickID(to);
	if(foo>0)
	{
		//Client is online.. But are they identified HUHUHUH?!!?
		if(Nickname::isIdentified(to,to))
		{
		String togo = String("You have just received a new note from \002")+from+"\002";
		Services::serviceNotice(togo,"Note",to);
		}
	}

}

void
Services::checkpoint(void)
{
//Any nick mods to be done? :-)
String query = "SELECT * from kills";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
	String id = ((std::string) row[0]).c_str();
	String killt = ((std::string) row[2]).c_str();
	int nowt = currentTime;
	String tomod = ((std::string) row[1]).c_str();
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
			if(!Nickname::isNickRegistered(newnick))
			{
				if(Nickname::getOnlineNickID(newnick)==0)
				{
				//Oke we can use this one :)
				running = false;
				}
			}
		
		}
		//String msg = "This nickname does not belong to you, your nickname will now be changed";
		//Services::serviceNotice(msg,"Nick",tomod);
		//String togo = String(":services.ircdome.org MODNICK ")+tomod+" "+newnick+" :0";
		//Services::queueAdd(String(togo));
		
		
	}
	if((killt.toInt()-nowt)<60)
	{
		if((killt.toInt()-nowt)>50)
		{
			String msg = "You have less than 60 seconds left to identify";
			Services::serviceNotice(msg,"Nick",tomod);
		}
	}
}

}
}


unsigned long Services::random(unsigned long max)
{
	return (unsigned long)(((max+1.0) * rand()) / RAND_MAX);
}
