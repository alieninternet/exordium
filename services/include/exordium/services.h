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
#include "exordium/log.h"
#include <kineircd/utils.h>
#include <kineircd/password.h>
#include <kineircd/daemon.h>
#include "exordium/service.h"
#include "exordium/conf.h"
#include <signal.h>

extern "C" {
        #include <unistd.h>
};

#include "socket/sockets.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

// Maybe this is in the wrong place
#define EXORDIUM_SERVICE_INIT_FUNCTION \
	extern "C" Module *service_init(String const &name) 


using Kine::String;
using Kine::Utils;
namespace Exordium {

class Services
{
private:
  String buffer;
  Kine::Daemon &daemon;
  Config &config;
   
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
   Utils::base_type PasswordStrBase;
   String::size_type PasswordStrBaseLongPad;
public:
   // dunno where you want these fellow james
   // Mr. Constructor
   Services(Kine::Daemon &d, Config &c)
     : daemon(d),
       config(c),
       PasswordStrBase(85), PasswordStrBaseLongPad(5)
       {};
   
   // Grab the daemon reference
   Kine::Daemon &getDaemon(void) const
     { return daemon; };
   
//Public Variables here.
   time_t currentTime;
//	bool load_config();
//	String logFileName;
//	String mysqlHost;
//	int mysqlPort;
//	String mysqlDb;
//	String mysqlUser;
//	String mysqlPass;
//	String UplinkHost;
	int init(void);
  	void run (void);
	bool handleInput (void);
   bool SecurePrivmsg;
   Core serviceM;  

//Function Declrations below here.
	 void shutdown(String const &);
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
	 void mode (String const &, String const &, String const &, String const &);
	 String parseHelp(String const &);
	 void doHelp(String const &, String const &,
                String const &, String const &);
	 void log (String const &,String const &,String const &,String const &);
	 void log (String const &,String const &,String const &);
	 void sendEmail(String const &, String const &, String const &);
	 void servicePart (String const &, String const &);

//Inline Functions Below here.
	 void killnick (String const &target, String const &from,
                        String const &reason)
  	{
    		Services::queueAdd (String (":") + from + " KILL " + target + " :" +
                        reason);
  	};

	 void helpme (String const &line, String const &from)
	{
		Services::queueAdd (String(":") + from + " HELPME :" + line);
		return;
	}
	 void serviceNotice(String const &line, String const &service, String const &target)
	{
	if(Services::usePrivmsg(target.toLower()))
	{
		Services::queueAdd (String (":") + service + " PRIVMSG " + target + " :" +
			line);
		return;
	}
		Services::queueAdd (String (":") + service + " NOTICE " + target + " :" +
			line);

	}
	 void servicePrivmsg(String const &line, String const &service, String const &target)
	{
		Services::queueAdd (String (":") + service + " PRIVMSG " + target + " :" +
			line);
		return;
	}
	 void Debug (String const &line)
	{
		queueAdd(String(":IRCDome PRIVMSG #Exordium :")+line);
	}
	 void serverMode (String const &chan, String const &mode,
				String const &target)
	{
		ModequeueAdd (String (":Chan MODE ") + chan + " " + mode + " " + target);
	}
	 void globop (String const &line, String const &from)
	{
	queueAdd (String (":") + from + " GOPER :" + line);
	};
  void sendCTCPpingReply (String const &from, String const &who,
                                 String & data)
  {
    Services::queueAdd (String (":") + from + " NOTICE " + who +
                        " :\001PING " + data + "\001");
  };
   void sendCTCPversionReply (String const &from, String const &who)
  {
    Services::queueAdd (String (":") + from + " NOTICE " + who +
                        " :\001VERSION IRCDome Network Services (c)2002 James I Wilkins <james@ircdome.org>\001");
  };
    

	 void registerService (String const &nick, String const &user,
        	                     String const &host, String const &modes,
                                     String const &realname)
	{                               
        queueAdd (String ("NICK ") + nick + " 1 " + String::convert (currentTime) +
            " " + modes + " " + user + " " + host +
            " services.ircdome.org services.ircdome.org 0 1 :" + realname);
        }; 
	 void serviceJoin (String const &service, String const &target)
	{
	  queueAdd (String (":services.ircdome.org SJOIN ") + String::convert(currentTime) + 
        	" " + target + " + :" + service);
	};
	 bool queueReady (void)
  	{		
    		return !outputQueue.empty ();
  	};
	 bool ModequeueReady (void)
  	{		
    		return !ModeoutputQueue.empty ();
  	};

  	 bool queueFlush (void)
  	{
		signal(SIGPIPE, SIG_IGN);

		if(Exordium::Services::connected)
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
				Exordium::Services::connected = false;
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
				Exordium::Services::connected = false;
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
		while (!outputQueue.empty ())
      		{
        		outputQueue.pop ();
      		}
  	};

	 void ModequeueKill (void)
  	{
		while (!ModeoutputQueue.empty ())
      		{
        		ModeoutputQueue.pop ();
      		}
  	};

  	 void queueAdd (String & line)
  	{
		if(Exordium::Services::connected)
		{
		Log::logLine(String("TX: ")+line);
		outputQueue.push (line + "\r\n");
		}
		else
		{
		Log::logLine("Tried to TX " + line + " but not connected");
		}
  	};

  	 void ModequeueAdd (String & line)
  	{
		Log::logLine(String("MQ: ")+line);
		outputQueue.push (line+ "\r\n");
  	};

  	 void queueAdd (String line)
  	{
		Log::logLine(String("TX: ")+line);
		outputQueue.push (line + "\r\n");
  	};

  	 void ModequeueAdd (String line)
  	{
		outputQueue.push (line + "\r\n");
  	};
	 bool getSecure (void)
	{
		return SecurePrivmsg;
	};
  	 unsigned long getCountRx (void)
  	{
    		return countRx;
  	};
  	 unsigned long getCountTx (void)
  	{
    		return countTx;
  	};
   bool usePrivmsg (String const &);
   bool loadModule (const String &, const String &);
   bool unloadModule (const String &);
   void serviceKick (String const &, String const &, String const &);
   bool isOp(String const &, String const &);
   bool isVoice(String const &, String const &);
   int countNotes(String const &);
   void sendNote(String const &,String const &,String const &); 
   void checkpoint(void);
   unsigned long random(unsigned long max);
};

};

















#endif

