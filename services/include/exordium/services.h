/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __SERVICES_H
#define __SERVICES_H

#include "kineircd/str.h"
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
#include "kineircd/utils.h"
#include "kineircd/password.h"
#include "exordium/service.h"
#include <signal.h>

extern "C" {
        #include <unistd.h>
};

#include "socket/sockets.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

using Kine::String;
using Kine::Utils;
namespace Exordium {

class Services
{
private:
  String buffer;
  static int sock;
  static int maxSock;
  static char *inputBuffer;
  static unsigned int inputBufferSize;
  static unsigned int inputBufferPosition;
  static struct sockaddr_in addr;
  static time_t startTime;
  static time_t lastPing;
  static time_t disconnectTime;
  static time_t serverLastSpoke;
  static time_t lastCheckPoint;
  static time_t lastExpireRun;
  static time_t lastModeRun;
  static bool connected;
  static bool stopping;
  static bool sendPing;
  static bool burstOk;
  static unsigned long countTx;
  static unsigned long countRx;
  static unsigned long remoteAddress;
  static std::queue < String > outputQueue;
  static std::queue < String > ModeoutputQueue;
  static Kine::SocketIPv4TCP socky;
public:
//Public Variables here.
  static time_t currentTime;
  static const Utils::base_type PasswordStrBase = 85;
  static const String::size_type PasswordStrBaseLongPad = 5;
	bool load_config();
	String logFileName;
	String mysqlHost;
	int mysqlPort;
	String mysqlDb;
	String mysqlUser;
	String mysqlPass;
	String UplinkHost;
	int init(void);
  	void run (void);
	bool handleInput (void);
  static bool SecurePrivmsg;
  static Core serviceM;  

//Function Declrations below here.
	void expireRun(void);
	static String getQuote(int const &number);
	static String getLogCount(void);
	static String getGlineCount(void);
	static String getNoteCount(void);
	static bool writeData (String &);
	static void disconnect(void);
	static bool connect(void);
	static void doBurst(void);	
	static void AddOnlineServer(String const &, String const &, String const &);
	static void doPong(String const &);
	static void mode (String const &, String const &, String const &, String const &);
	static String parseHelp(String const &);
	static void doHelp(String const &, String const &,
                String const &, String const &);
	static void log (String const &,String const &,String const &,String const &);
	static void log (String const &,String const &,String const &);
	static void sendEmail(String const &, String const &, String const &);
	static void servicePart (String const &, String const &);

//Inline Functions Below here.
	static void killnick (String const &target, String const &from,
                        String const &reason)
  	{
    		Services::queueAdd (String (":") + from + " KILL " + target + " :" +
                        reason);
  	};

	static void helpme (String const &line, String const &from)
	{
		Services::queueAdd (String(":") + from + " HELPME :" + line);
		return;
	}
	static void serviceNotice(String const &line, String const &service, String const &target)
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
	static void servicePrivmsg(String const &line, String const &service, String const &target)
	{
		Services::queueAdd (String (":") + service + " PRIVMSG " + target + " :" +
			line);
		return;
	}
	static void Debug (String const &line)
	{
		queueAdd(String(":IRCDome PRIVMSG #Exordium :")+line);
	}
	static void serverMode (String const &chan, String const &mode,
				String const &target)
	{
		ModequeueAdd (String (":Chan MODE ") + chan + " " + mode + " " + target);
	}
	static void globop (String const &line, String const &from)
	{
	queueAdd (String (":") + from + " GOPER :" + line);
	};
 static void sendCTCPpingReply (String const &from, String const &who,
                                 String & data)
  {
    Services::queueAdd (String (":") + from + " NOTICE " + who +
                        " :\001PING " + data + "\001");
  };
  static void sendCTCPversionReply (String const &from, String const &who)
  {
    Services::queueAdd (String (":") + from + " NOTICE " + who +
                        " :\001VERSION IRCDome Network Services (c)2002 James I Wilkins <james@ircdome.org>\001");
  };
    

	static void registerService (String const &nick, String const &user,
        	                     String const &host, String const &modes,
                                     String const &realname)
	{                               
        queueAdd (String ("NICK ") + nick + " 1 " + String::convert (currentTime) +
            " " + modes + " " + user + " " + host +
            " services.ircdome.org services.ircdome.org 0 1 :" + realname);
        }; 
	static void serviceJoin (String const &service, String const &target)
	{
	  queueAdd (String (":services.ircdome.org SJOIN ") + String::convert(currentTime) + 
        	" " + target + " + :" + service);
	};
	static bool queueReady (void)
  	{		
    		return !outputQueue.empty ();
  	};
	static bool ModequeueReady (void)
  	{		
    		return !ModeoutputQueue.empty ();
  	};

  	static bool queueFlush (void)
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
				cout << "Socky write fubared... disconnecting :(" << endl;
				Exordium::Services::connected = false;
				return false;
			}
			}
			else
			{
				cout << "Socky is dead" << endl;
				Exordium::Services::connected = false;
				return false;
			}
			return false;
			
		}
		else
		{
		cout << "Trying to queueflush when disconnected ?!" << endl;
    		return false;
		}
  	};

  	static bool ModequeueFlush (void)
  	{
    		if (socky.write (ModeoutputQueue.front ()))
      		{	
        		ModeoutputQueue.pop ();
        		return true;
      		}
	return false;
  	};

	static void queueKill (void)
  	{
		while (!outputQueue.empty ())
      		{
        		outputQueue.pop ();
      		}
  	};

	static void ModequeueKill (void)
  	{
		while (!ModeoutputQueue.empty ())
      		{
        		ModeoutputQueue.pop ();
      		}
  	};

  	static void queueAdd (String & line)
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

  	static void ModequeueAdd (String & line)
  	{
		Log::logLine(String("MQ: ")+line);
		outputQueue.push (line+ "\r\n");
  	};

  	static void queueAdd (String line)
  	{
		Log::logLine(String("TX: ")+line);
		outputQueue.push (line + "\r\n");
  	};

  	static void ModequeueAdd (String line)
  	{
		outputQueue.push (line + "\r\n");
  	};
	static bool getSecure (void)
	{
		return SecurePrivmsg;
	};
  	static unsigned long getCountRx (void)
  	{
    		return countRx;
  	};
  	static unsigned long getCountTx (void)
  	{
    		return countTx;
  	};
  static bool usePrivmsg (String const &);
  static bool loadModule (const String &, const String &);
  static bool unloadModule (const String &);
  static void serviceKick (String const &, String const &, String const &);
  static bool isOp(String const &, String const &);
  static bool isVoice(String const &, String const &);
  static int countNotes(String const &);
  static void sendNote(String const &,String const &,String const &); 
  static void checkpoint(void);
  static unsigned long random(unsigned long max);
};

};

















#endif

