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
#include <kineircd/utils.h>
#include <kineircd/password.h>
#include <kineircd/daemon.h>
#include <csignal>

extern "C" {
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
};

#include <libais/socket/sockets.h>

#ifdef DMALLOC
# include "dmalloc.h"
#endif

#include "exordium/log.h"
#include "exordium/service.h"
#include "exordium/conf.h"
#include "exordium/parser.h"
#include "exordium/channel.h"
#include "exordium/ircdome.h"
#include "exordium/user.h"
//#include "exordium/userbase.h"


namespace Exordium {
   class Sql;
   
   class Services
     {
      private:
	const LibAIS::String buffer;
	Kine::Daemon& daemon;
	Log& logger;
	Sql& database;
	Config& config;

	Parser parser;
	Channel channel;
	IRCDome ircdome;
	
	
	int sock;
	int maxSock;
	char *inputBuffer;
	static const unsigned int inputBufferSize = 512;
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
	std::queue < LibAIS::String > outputQueue;
	std::queue < LibAIS::String > ModeoutputQueue;
	LibAIS::SocketIPv4TCP socky;
	typedef std::map <LibAIS::String, User *> user_map;
	user_map users;
      public:
//	UserBase clients;
	static const Kine::Utils::base_type PasswordStrBase = 85;
	static const LibAIS::String::size_type PasswordStrBaseLongPad = 5;
	User* const addUser(const LibAIS::String& name, const int oid);
	bool delUser(LibAIS::String &);
	User* findUser(LibAIS::String &);
	void setNick(User &,LibAIS::String &);
	LibAIS::String getRegNickCount(void);
	// dunno where you want these fellow james
	// Mr. Constructor
	Services(Kine::Daemon& d, Log& l, Sql& db, Config& c);
	
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
	Config& getConfig(void)
	  { return config; };

	// Return the channel thingy
	Channel& getChannel(void)
	  { return channel; };

	// Return the IRCDome thingy
	IRCDome& getIRCDome(void)
	  { return ircdome; };
	
	time_t currentTime;
  	void run (void);
	bool handleInput (void);
	bool SecurePrivmsg;

	// Function Declrations below here.
	void shutdown(const LibAIS::String &);
	void SynchTime(void);
	void expireRun(void);
	LibAIS::String getQuote(int const &number);
	LibAIS::String getLogCount(void);
	LibAIS::String getGlineCount(void);
	LibAIS::String getNoteCount(void);
	bool writeData (LibAIS::String &);
	void disconnect(void);
	bool connect(void);
	void doBurst(void);
	void AddOnlineServer(LibAIS::String const &, LibAIS::String const &, 
			     LibAIS::String const &);
        void DelOnlineServer(LibAIS::String const &);

	void doPong(LibAIS::String const &);
	bool queueFlush(void);
	void mode (LibAIS::String const &, LibAIS::String const &, 
		   LibAIS::String const &, LibAIS::String const &);
	LibAIS::String parseHelp(LibAIS::String const &);
	void doHelp(User&, LibAIS::String const &,
		    LibAIS::String const &, LibAIS::String const &);
	void log (User&, LibAIS::String const &, 
		  LibAIS::String const &, LibAIS::String const &);
	void log (User&, LibAIS::String const &, 
		  LibAIS::String const &);
	void sendEmail(LibAIS::String const &, LibAIS::String const &, 
		       LibAIS::String const &);
	void servicePart (LibAIS::String const &, LibAIS::String const &);
	void test(void)
	  {
	     std::cout << 
	       "This is a simple test, proving we can access services" << 
	       std::endl;
	  };
	//Inline Functions Below here.
	void killnick (LibAIS::String const &target,
		       LibAIS::String const &from,
		       LibAIS::String const &reason)
	  {
	     queueAdd(":" + from + " KILL " + target + " :" + reason);
	  };

	void helpme (LibAIS::String const &line, LibAIS::String const &from)
	  {
	     queueAdd(":" + from + " HELPME :" + line);
	  };
	
	void serviceNotice(LibAIS::String const &line,
			   LibAIS::String const &service, 
			   LibAIS::String const &target)
	  {
	     if (usePrivmsg(target.toLower())) {
		queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	     } else {
		queueAdd(":" + service + " NOTICE " + target + " :" + line);
	     }
	  }
	void serviceNoticeForce(LibAIS::String const &line, 
				LibAIS::String const &service,
				LibAIS::String const &target)
	  {
	     queueAdd(":" + service + " NOTICE " + target + " :" + line);
	  }
	
	void servicePrivmsg(LibAIS::String const &line,
			    LibAIS::String const &service, 
			    LibAIS::String const &target)
	  {
	     queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	  }
	
	void Debug(LibAIS::String const &line);
	
	void serverMode(LibAIS::String const &chan, LibAIS::String const &mode,
			LibAIS::String const &target)
	  {
	     ModequeueAdd(":Chan MODE " + chan + " " + mode + " " + target);
	  }
	
	void globop(LibAIS::String const &line, LibAIS::String const &from)
	  {
	     queueAdd(":" + from + " GOPER :" + line);
	  };
	
	void sendCTCPpingReply(LibAIS::String const &from,
			       LibAIS::String const &who,
			       LibAIS::String & data)
	  {
	     queueAdd(":" + from + " NOTICE " + who + " :\001PING " + 
		      data + "\001");
	  };
	
	void sendCTCPversionReply(LibAIS::String const &from,
				  LibAIS::String const &who)
	  {
	     queueAdd(":" + from + " NOTICE " + who +
		      " :\001VERSION IRCDome Network Services (c)2002 James I Wilkins <james@ircdome.org>\001");
	  };
    

	void registerService(LibAIS::String const &nick,
			     LibAIS::String const &user,
			     LibAIS::String const &host,
			     LibAIS::String const &modes,
			     LibAIS::String const &realname)
	  {                               
	     queueAdd ("NICK " + nick + " 1 " + 
		       LibAIS::String::convert(currentTime) + " " + modes + 
		       " " + user + " " + host +
		       " services.ircdome.org services.ircdome.org 0 1 :" + 
		       realname);
        }; 
	
	void serviceJoin(LibAIS::String const &service,
			 LibAIS::String const &target);
	
	bool queueReady(void) const
	  { 
	     return !outputQueue.empty(); 
	  };
	
	bool ModequeueReady(void) const
	  {		
	     return !ModeoutputQueue.empty();
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

	void queueAdd (const LibAIS::String& line)
	  {
	     if(connected)
	       {
		  logger.logLine("TX: " + line);
		  outputQueue.push (line + "\r\n");
		  countTx += line.length();
	       }
	     else
	       {
		  logger.logLine("Tried to TX " + line + " but not connected");
	       }
	  };

	void ModequeueAdd(const LibAIS::String & line)
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
	LibAIS::String getOnlineCount(void)
	  {
	     return LibAIS::String::convert(users.size());
	  };
	bool usePrivmsg(LibAIS::String const &);
	void serviceKick(LibAIS::String const &, LibAIS::String const &,
			 LibAIS::String const &);
	bool isOp(LibAIS::String const &, LibAIS::String const &);
	bool isVoice(LibAIS::String const &, LibAIS::String const &);
	int countNotes(LibAIS::String const &);
	void sendNote(LibAIS::String const &,LibAIS::String const &,LibAIS::String const &); 
	void checkpoint(void);
	unsigned long random(unsigned long max);
	int locateID(LibAIS::String const &nick);
	LibAIS::String generatePassword(LibAIS::String const &,LibAIS::String const &);
	bool isAuthorised(LibAIS::String const &);
	User* addClient(LibAIS::String const &, LibAIS::String const &,
			LibAIS::String const &, LibAIS::String const &,
			LibAIS::String const &, LibAIS::String const &,
			LibAIS::String const &, LibAIS::String const &,
			LibAIS::String const &);
	int getRequiredAccess(LibAIS::String const &, LibAIS::String const &);
	bool isNickRegistered(LibAIS::String const &);
	LibAIS::String getPass(LibAIS::String const &);
	LibAIS::String stripModes(LibAIS::String const &);
	int getRegisteredNickID(LibAIS::String const &);
	void modeIdentify(LibAIS::String const &);
	void updateLastID(LibAIS::String const &);
	LibAIS::String getNick(int const &);
	LibAIS::String getOnlineNick(int const &);
	LibAIS::String getpendingCode(LibAIS::String const &);
	void registerNick(LibAIS::String const &,LibAIS::String const &,LibAIS::String const &);
	LibAIS::String genAuth(LibAIS::String const &);
	LibAIS::String getURL(LibAIS::String const &);
	LibAIS::String getMSN(LibAIS::String const &);
	LibAIS::String getYAHOO(LibAIS::String const &);
	LibAIS::String getAIM(LibAIS::String const &);
	LibAIS::String getICQ(LibAIS::String const &);
//	LibAIS::String getLanguage(LibAIS::String const &);
	LibAIS::String getEmail(LibAIS::String const &);
	LibAIS::String getRegDate(LibAIS::String const &);
	LibAIS::String getLastID(LibAIS::String const &);
	LibAIS::String getLastHost(LibAIS::String const &);
        void addOper(LibAIS::String const &, int access);
        void delOper(LibAIS::String const &);
        bool isOper(LibAIS::String const &);
        void validateOper(LibAIS::String &);
	
     };
}; // namespace Exordium

#include "exordium/sql.h"

#endif
