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

#ifndef _INCLUDE_SERVICES_H_
# define _INCLUDE_SERVICES_H_ 1

# include <aisutil/socket/sockets.h>

# include "exordium/parser.h"
# include "exordium/console.h"
# include "exordium/singleton.h"
# include "exordium/log.h"

#include "kineircd/daemon.h"
#include "kineircd/utils.h"

#include <queue>
#include <map>
#include <ctime>

namespace Exordium {

   #define servicesFwd (*Services::instance())
 
   class Services : public Singleton<Services> {
    private:
      Parser parser;
      Console console;

      const AISutil::String buffer;
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
      time_t currentTime;
      bool connected;
      bool stopping;
      bool sendPing;
      bool burstOk;
      unsigned long countTx;
      unsigned long countRx;
      unsigned long remoteAddress;
      std::queue < AISutil::String > outputQueue;
      AISutil::SocketIPv4TCP socky;
      
      bool handleInput(void);
      
      bool queueReady(void) const
	{ return !outputQueue.empty(); };
      
      void queueKill (void)
	{
	   while(!outputQueue.empty()) {
	      outputQueue.pop();
	   }
	};
      
      void SynchTime(void);
      void expireRun(void);
      void disconnect(void);
      bool connect(void);
      AISutil::String getLogCount(void);
      AISutil::String getGlineCount(void);
      AISutil::String getNoteCount(void);
      void doBurst(void);
      bool queueFlush(void);
      AISutil::String parseHelp(AISutil::String const &);
      
	
      void checkpoint(void);


      // Constructor
      Services(void);
	
    public:
      friend class Singleton<Services>;
	
      // Destructor
      ~Services(void)
	{};


      static const Kine::Utils::base_type PasswordStrBase = 85;
      static const AISutil::String::size_type PasswordStrBaseLongPad = 5;

      // *sigh* grab the console object :(
      Console& getConsole(void)
	{ return console; };

      bool SecurePrivmsg;

      time_t getCurrentTime(void) { return currentTime; }

      unsigned long random(unsigned long max) { return (unsigned long)(((max+1.0) * rand()) / RAND_MAX); }


   
      // These will disappear upon full integration with kine..
      void run(void);

      AISutil::String generatePassword(const AISutil::String&, const AISutil::String&);


      // Log a line of text..
      void logLine(const std::string& line,
                   const Log::mask_type mask = Log::Informative)
        { Kine::daemon().log(line, (Kine::Logger::Mask::type)mask); }

      
      // Function Declrations below here.
      int getAccess(AISutil::String &, AISutil::String &);
      void shutdown(const AISutil::String &);
      
      // This should be in the GAME MODULE
      AISutil::String getQuote(int const &number);
      
      void AddOnlineServer(AISutil::String const &, AISutil::String const &, 
			   AISutil::String const &);
      void DelOnlineServer(AISutil::String const &);
      
      
      void mode(AISutil::String const &, AISutil::String const &,
		AISutil::String const &, AISutil::String const &);
      
      // This shold be in the Service:: class
      void doHelp(AISutil::String const &, AISutil::String const &, AISutil::String const &,
		  AISutil::String const &);
      
      void log(User&, AISutil::String const &, AISutil::String const &,
	       AISutil::String const &);
      void log(User&, AISutil::String const &, AISutil::String const &);
      
      // This is in the wrong place, surely
      void sendEmail(AISutil::String const &, AISutil::String const &,
		     AISutil::String const &);

      void servicePart (AISutil::String const &, AISutil::String const &);
      void queueAdd (const AISutil::String& line)
	{
	   if (connected) {
	      logLine("TX: " + line, Log::Debug);
	      outputQueue.push(line + "\r\n");
	      countTx += line.length();
	   } else {
	      logLine("Tried to TX " + line + " but not connected",
		      Log::Debug);
	   }
	};

      void killnick (AISutil::String const &target,
		     AISutil::String const &from,
		     AISutil::String const &reason)
	{
	   queueAdd(":" + from + " KILL " + target + " :" + reason);
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
      
      // This does not modify a server mode, it should be renamed.
      void serverMode(AISutil::String const &chan, AISutil::String const &mode,
		      AISutil::String const &target)
	{
	   queueAdd(":Chan MODE " + chan + " " + mode + " " + target);
	}
      
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
			   AISutil::String const &realname)
	{                               
	   queueAdd ("NICK " + nick + " 1 " + 
		     AISutil::String::convert(currentTime) + " +dz " + user + 
		     " " + host + 
		     " services.ircdome.org services.ircdome.org 0 1 :" + 
		     realname);
        }; 
      
      void serviceJoin(AISutil::String const &service,
		       AISutil::String const &target);
      
      bool usePrivmsg(AISutil::String const &);
      
      // These should be moved to the new channel classes
      void serviceKick(AISutil::String const &, AISutil::String const &,
		       AISutil::String const &);
      
      
      // This should be moved to the NOTE MODULE
      void sendNote(AISutil::String const &, AISutil::String const &,
		    AISutil::String const &);
      
      bool isAuthorised(AISutil::String const &);
      int getRequiredAccess(AISutil::String const &, AISutil::String const &);
      
      void modeIdentify(AISutil::String const &);
            
      void addOper(AISutil::String const &, int access);
      void delOper(AISutil::String const &);
      bool isOper(AISutil::String const &);
      void validateOper(AISutil::String &);
      
      unsigned long getCountTx(void) const
	{ return countTx; };
      unsigned long getCountRx(void) const
	{ return countRx; };
      
      void doPong(AISutil::String const &);
   }; // class Services
}; // namespace Exordium

#endif // _INCLUDE_SERVICES_H_
