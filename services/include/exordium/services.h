/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __SERVICES_H
#define __SERVICES_H

#include <kineircd/str.h>
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <fstream>
#include "exordium/log.h"
#include <kineircd/utils.h>
#include <kineircd/password.h>
#include <kineircd/daemon.h>
#include "exordium/service.h"
#include "exordium/conf.h"
#include "exordium/sql.h"
#include <csignal>

extern "C" {
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
};

#include "socket/sockets.h"

#ifdef DMALLOC
# include "dmalloc.h"
#endif

// Maybe this is in the wrong place
#define EXORDIUM_SERVICE_INIT_FUNCTION \
	extern "C" Module *service_init(String const &name) 


namespace Exordium {
   class Services
     {
      private:
	String buffer;
	Kine::Daemon& daemon;
	Log& logger;
	Sql& database;
	const Config& config;
	
	int sock;
	int maxSock;
	char *inputBuffer;
	unsigned int inputBufferSize;
	unsigned int inputBufferPosition;
	struct sockaddr_in addr;
	time_t startTime;
	time_t lastPing;
	time_t disconnectTime;
	time_t stopTime;
	time_t serverLastSpoke;
	time_t lastCheckPoint;
	time_t lastExpireRun;
	time_t lastModeRun;
	bool connected;
	bool stopping;
	bool sendPing;
	bool burstOk;
	unsigned long countTx;
	unsigned long countRx;
	unsigned long remoteAddress;
	std::queue < String > outputQueue;
	std::queue < String > ModeoutputQueue;
	Kine::SocketIPv4TCP socky;
	static const Kine::Utils::base_type PasswordStrBase = 85;
	static const Kine::String::size_type PasswordStrBaseLongPad = 5;
	
      public:
	// dunno where you want these fellow james
	// Mr. Constructor
	Services(Kine::Daemon& d, Log& l, Sql& db, const Config& c);
	
	// Grab the daemon reference
	Kine::Daemon &getDaemon(void) const
	  { return daemon; };
   
	// Grab the logger reference
	Log& getLogger(void)
	  { return logger; };
	
	// Grab the database reference
	Sql& getDatabase(void)
	  { return database; };

	// Grab the configuration reference
	const Config& getConfig(void) const
	  { return config; };
	
	time_t currentTime;
//	bool load_config();
//	String logFileName;
//	String mysqlHost;
//	int mysqlPort;
//	String mysqlDb;
//	String mysqlUser;
//	String mysqlPass;
//	String UplinkHost;
  	void run (void);
	bool handleInput (void);
	bool SecurePrivmsg;
	Core serviceM;  
	
	// Function Declrations below here.
	void shutdown(const String &);
	void SynchTime(void);
	void expireRun(void);
	String getQuote(int const &number);
	String getLogCount(void);
	String getGlineCount(void);
	String getNoteCount(void);
	bool writeData (String &);
	void disconnect(void);
	bool connect(void);
	void doBurst(void);	
	void AddOnlineServer(String const &, String const &, String const &);
	void doPong(String const &);
	void mode (String const &, String const &, String const &,
		   String const &);
	String parseHelp(String const &);
	void doHelp(String const &, String const &,
		    String const &, String const &);
	void log (String const &, String const &, String const &,
		  String const &);
	void log (String const &, String const &, String const &);
	void sendEmail(String const &, String const &, String const &);
	void servicePart (String const &, String const &);
	
	//Inline Functions Below here.
	void killnick (String const &target, String const &from,
		       String const &reason)
	  {
	     queueAdd(":" + from + " KILL " + target + " :" + reason);
	  };

	void helpme (String const &line, String const &from)
	  {
	     queueAdd(":" + from + " HELPME :" + line);
	  };
	
	void serviceNotice(String const &line, String const &service, 
			   String const &target)
	  {
	     if (usePrivmsg(target.toLower())) {
		queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	     } else {
		queueAdd(":" + service + " NOTICE " + target + " :" + line);
	     }
	  }
	
	void servicePrivmsg(String const &line, String const &service, 
			    String const &target)
	  {
	     queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	  }
	
	void Debug(String const &line)
	  {
	     queueAdd(":IRCDome PRIVMSG #Exordium :" + line);
	  }
	
	void serverMode(String const &chan, String const &mode,
			String const &target)
	  {
	     ModequeueAdd(":Chan MODE " + chan + " " + mode + " " + target);
	  }
	
	void globop(String const &line, String const &from)
	  {
	     queueAdd(":" + from + " GOPER :" + line);
	  };
	
	void sendCTCPpingReply(String const &from, String const &who,
			       String & data)
	  {
	     queueAdd(":" + from + " NOTICE " + who + " :\001PING " + 
		      data + "\001");
	  };
	
	void sendCTCPversionReply(String const &from, String const &who)
	  {
	     queueAdd(":" + from + " NOTICE " + who +
		      " :\001VERSION IRCDome Network Services (c)2002 James I Wilkins <james@ircdome.org>\001");
	  };
    

	void registerService(String const &nick, String const &user,
			     String const &host, String const &modes,
			     String const &realname)
	  {                               
	     queueAdd ("NICK " + nick + " 1 " + 
		       Kine::String::convert(currentTime) + " " + modes + 
		       " " + user + " " + host +
		       " services.ircdome.org services.ircdome.org 0 1 :" + 
		       realname);
        }; 
	
	void serviceJoin(String const &service, String const &target)
	  {
	     queueAdd(":services.ircdome.org SJOIN " +
		      Kine::String::convert(currentTime) + " " + target +
		      " + :" + service);
	};
	
	bool queueReady(void) const
	  { 
	     return !outputQueue.empty(); 
	  };
	
	bool ModequeueReady(void) const
	  {		
	     return !ModeoutputQueue.empty();
	  };

	// James, this should be in the .cpp file!
	bool queueFlush (void)
	  {
//	     signal(SIGPIPE, SIG_IGN);
	     
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
				std::cout << "Socky write fubared... disconnecting :(" << std::endl;
				connected = false;
				return false;
				}
				//Ignore if we're stopping.. YES I KNOW ITS BAD :(
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

	// .. and this!
  	 bool ModequeueFlush (void)
  	{
    		if (socky.write (ModeoutputQueue.front ()))
      		{	
        		ModeoutputQueue.pop ();
        		return true;
      		}
	return false;
  	};

	void queueKill (void)
	  {
	     while(!outputQueue.empty()) {
		outputQueue.pop();
	     }
	  };
	
	void ModequeueKill (void)
	  {
	     while (!ModeoutputQueue.empty()) {
		ModeoutputQueue.pop();
	     }
	  };

	void queueAdd (const String& line)
	  {
	     if(connected)
	       {
		  logger.logLine(String("TX: ")+line);
		  outputQueue.push (line + "\r\n");
	       }
	     else
	       {
		  logger.logLine("Tried to TX " + line + " but not connected");
	       }
	  };

	void ModequeueAdd(const String & line)
	  {
	     logger.logLine("MQ: " + line);
	     outputQueue.push(line+ "\r\n");
	  };
	
//	void queueAdd(String line)
//	  {
//	     logger.logLine("TX: " + line);
//	     outputQueue.push(line + "\r\n");
//	  };
	
//	void ModequeueAdd(String line)
//	  {
//	     outputQueue.push(line + "\r\n");
//	  };
	
	bool getSecure(void) const
	  {
	     return SecurePrivmsg;
	  };
	
	unsigned long getCountRx(void) const
	  {
	     return countRx;
	  };
	
	unsigned long getCountTx(void) const
	  {
	     return countTx;
	  };
	
	bool usePrivmsg(String const &);
	bool loadModule(const String &, const String &);
	bool unloadModule(const String &);
	void serviceKick(String const &, String const &, String const &);
	bool isOp(String const &, String const &);
	bool isVoice(String const &, String const &);
	int countNotes(String const &);
	void sendNote(String const &,String const &,String const &); 
	void checkpoint(void);
	unsigned long random(unsigned long max);
     };
}; // namespace Exordium

#endif
