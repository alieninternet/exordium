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

// This is naughty..
#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

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
#include <vector>

extern "C" {
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
};

#include <aisutil/socket/sockets.h>

#ifdef DMALLOC
# include "dmalloc.h"
#endif

#include <exordium/service.h>
#include <exordium/conf.h>
#include <exordium/parser.h>
#include <exordium/channel.h>
#include <exordium/ircdome.h>
#include <exordium/user.h>
#include <exordium/dchan.h>
#include <exordium/log.h>


namespace Exordium {
   class CDatabase;  
 
   class Services
     {
      private:
        const AISutil::String buffer;
        Kine::Daemon& daemon;
	
        CDatabase& database;
      
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
	bool connected;
	bool stopping;
	bool sendPing;
	bool burstOk;
        unsigned long countTx;
	unsigned long countRx;
	unsigned long remoteAddress;
	std::queue < AISutil::String > outputQueue;
	AISutil::SocketIPv4TCP socky;
	typedef std::map <AISutil::String, User *> user_map;
	typedef std::map <AISutil::String, dChan *> chan_map;
	user_map users;
	chan_map chans;
	
	void SynchTime(void);
	void expireRun(void);
	void disconnect(void);
	bool connect(void);
	bool handleInput (void);
	AISutil::String getLogCount(void);
	AISutil::String getGlineCount(void);
	AISutil::String getNoteCount(void);
	void doBurst(void);
	bool queueFlush(void);
	AISutil::String parseHelp(AISutil::String const &);
	
	bool queueReady(void) const
	  { 
	     return !outputQueue.empty(); 
	  };
	
	void queueKill (void)
	  {
	     while(!outputQueue.empty()) {
		outputQueue.pop();
	     }
	  };
	
	AISutil::String getOnlineCount(void) const
	  {
	     return AISutil::String::convert(users.size());
	  };
	
	void checkpoint(void);
	
      public:
	static const Kine::Utils::base_type PasswordStrBase = 85;
	static const AISutil::String::size_type PasswordStrBaseLongPad = 5;
	User* const addUser(const AISutil::String& name, const int oid);
	dChan* const addChan(const AISutil::String& name, const int oid);
	bool delUser(AISutil::String &);
	bool delChan(AISutil::String &);
	User* findUser(AISutil::String &);
	dChan* findChan(AISutil::String &);
	void setNick(User &,AISutil::String &);
	AISutil::String getRegNickCount(void);
	
	// Mr. Constructor
	Services(Kine::Daemon& d, Config& c, CDatabase& db);
	
        // Mr. Destructor
        ~Services(void);	

	// Grab the daemon reference
	Kine::Daemon &getDaemon(void) const
	  { return daemon; };
   
	// Grab the database reference
	CDatabase& getDatabase(void)
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

	// this is dodgey and should be passed to modules, *not* set by Parser::
	bool SecurePrivmsg;

	// Log a line of text..
	void logLine(const AISutil::String& line,
		     const Log::mask_type mask = Log::Informative)
	  { std::clog << "Log(" << mask << "): " << line << std::endl; };

	// Function Declrations below here.
	int getAccess(AISutil::String &, AISutil::String &);
	void shutdown(const AISutil::String &);
	
	// This should be in the GAME MODULE
	AISutil::String getQuote(int const &number);
	
	void AddOnlineServer(AISutil::String const &, AISutil::String const &, 
			     AISutil::String const &);
        void DelOnlineServer(AISutil::String const &);

	void doPong(AISutil::String const &);
	void mode (AISutil::String const &, AISutil::String const &, 
		   AISutil::String const &, AISutil::String const &);
	
	// This shold be in the Service:: class
	void doHelp(User&, AISutil::String const &,
		    AISutil::String const &, AISutil::String const &);
	
	void log (User&, AISutil::String const &, 
		  AISutil::String const &, AISutil::String const &);
	void log (User&, AISutil::String const &, 
		  AISutil::String const &);
	void sendEmail(AISutil::String const &, AISutil::String const &, 
		       AISutil::String const &);
	void servicePart (AISutil::String const &, AISutil::String const &);

	//Inline Functions Below here.
	void killnick (AISutil::String const &target,
		       AISutil::String const &from,
		       AISutil::String const &reason)
	  {
	     queueAdd(":" + from + " KILL " + target + " :" + reason);
	  };

	// This should be replaced with logLine() calls
	void helpme (AISutil::String const &line, AISutil::String const &from)
	  {
	     queueAdd(":" + from + " HELPME :" + line);
	  };
	
	void serviceNotice(AISutil::String const &line,
			   AISutil::String const &service, 
			   AISutil::String const &target)
	  {
	     if (usePrivmsg(target.toLower())) {
		queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	     } else {
		queueAdd(":" + service + " NOTICE " + target + " :" + line);
	     }
	  }
	void serviceNoticeForce(AISutil::String const &line, 
				AISutil::String const &service,
				AISutil::String const &target)
	  {
	     queueAdd(":" + service + " NOTICE " + target + " :" + line);
	  }
	
	void servicePrivmsg(AISutil::String const &line,
			    AISutil::String const &service, 
			    AISutil::String const &target)
	  {
	     queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	  }

	// This, also, should be replaced by logLine calls
	void Debug(AISutil::String const &line);

	// This does not modify a server mode, it should be renamed.
	void serverMode(AISutil::String const &chan, AISutil::String const &mode,
			AISutil::String const &target)
	  {
	     queueAdd(":Chan MODE " + chan + " " + mode + " " + target);
	  }

	// This should be replaced with logLine() calls
	void globop(AISutil::String const &line, AISutil::String const &from)
	  {
	     queueAdd(":" + from + " GOPER :" + line);
	  };

	void sendCTCPpingReply(AISutil::String const &from,
			       AISutil::String const &who,
			       AISutil::String & data)
	  {
	     queueAdd(":" + from + " NOTICE " + who + " :\001PING " + 
		      data + "\001");
	  };
	
	void sendCTCPversionReply(AISutil::String const &from,
				  AISutil::String const &who)
	  {
	     queueAdd(":" + from + " NOTICE " + who +
		      " :\001VERSION IRCDome Network Services (c)2002 James I Wilkins <james@ircdome.org>\001");
	  };
    

	void registerService(AISutil::String const &nick,
			     AISutil::String const &user,
			     AISutil::String const &host,
			     AISutil::String const &modes,
			     AISutil::String const &realname)
	  {                               
	     queueAdd ("NICK " + nick + " 1 " + 
		       AISutil::String::convert(currentTime) + " " + modes + 
		       " " + user + " " + host +
		       " services.ircdome.org services.ircdome.org 0 1 :" + 
		       realname);
        }; 
	
	void serviceJoin(AISutil::String const &service,
			 AISutil::String const &target);
	
	void queueAdd (const AISutil::String& line)
	  {
	     if(connected)
	       {
		  logLine("TX: " + line,
			  Log::Debug);
		  outputQueue.push (line + "\r\n");
		  countTx += line.length();
	       }
	     else
	       {
		  logLine("Tried to TX " + line + " but not connected",
			  Log::Debug);
	       }
	  };

	bool usePrivmsg(AISutil::String const &);
	
	// These should be moved to the new channel classes
	void serviceKick(AISutil::String const &, AISutil::String const &,
			 AISutil::String const &);
	bool isOp(AISutil::String const &, AISutil::String const &);
	bool isVoice(AISutil::String const &, AISutil::String const &);
	
	
//	int countNotes(AISutil::String const &);

	// This should be moved to the NOTE MODULE
	void sendNote(AISutil::String const &,AISutil::String const &,AISutil::String const &);

	static unsigned long random(unsigned long max);
	
	int locateID(AISutil::String const &nick);
	static AISutil::String generatePassword(AISutil::String const &,AISutil::String const &);
	bool isAuthorised(AISutil::String const &);
	User* addClient(AISutil::String const &, AISutil::String const &,
			AISutil::String const &, AISutil::String const &,
			AISutil::String const &, AISutil::String const &,
			AISutil::String const &, AISutil::String const &,
			AISutil::String const &);
	int getRequiredAccess(AISutil::String const &, AISutil::String const &);
	bool isNickRegistered(AISutil::String const &);
	
	// This should be in the NICK MODULE
	AISutil::String getPass(AISutil::String const &);
	void modeIdentify(AISutil::String const &);
	void updateLastID(AISutil::String const &);
	
	
//	AISutil::String stripModes(AISutil::String const &);
	int getRegisteredNickID(AISutil::String const &);
	
	AISutil::String getNick(int const &);
	AISutil::String getOnlineNick(int const &);
	AISutil::String getpendingCode(AISutil::String const &);
	void registerNick(AISutil::String const &,AISutil::String const &,AISutil::String const &);
	AISutil::String genAuth(AISutil::String const &);
	
	// These definately should be in the NICK MODULE
	AISutil::String getURL(AISutil::String const &);
	AISutil::String getMSN(AISutil::String const &);
	AISutil::String getYAHOO(AISutil::String const &);
	AISutil::String getAIM(AISutil::String const &);
	AISutil::String getICQ(AISutil::String const &);
//	AISutil::String getLanguage(AISutil::String const &);
	AISutil::String getEmail(AISutil::String const &);
	AISutil::String getRegDate(AISutil::String const &);
	AISutil::String getLastID(AISutil::String const &);
	AISutil::String getLastHost(AISutil::String const &);
	
        void addOper(AISutil::String const &, int access);
        void delOper(AISutil::String const &);
        bool isOper(AISutil::String const &);
        void validateOper(AISutil::String &);

	// These will disappear upon full integration with kine..
        unsigned long getCountTx(void) const
	  { return countTx; };
	unsigned long getCountRx(void) const
	  { return countRx; };
     };
}; // namespace Exordium

#include <exordium/database/database.h>

#endif
