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

#ifndef _SOURCE_LIB_SERVICES_H_
# define _SOURCE_LIB_SERVICES_H_ 1

# include <aisutil/socket/sockets.h>
# include <kineircd/config.h>

# include "exordium/services.h"
# include "parser.h"
# include "console.h"
# include "config.h"

namespace Exordium {
   class ServicesInternal : public Services {
    private:
      Parser parser;
      Console console;

      ConfigInternal& config;

      typedef std::map <AISutil::String, User *> user_map;
      typedef std::map <AISutil::String, dChan *> chan_map;
      user_map users;
      chan_map chans;
      
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
      void disconnect(void);
      bool connect(void);
      bool queueFlush(void);
      AISutil::String parseHelp(AISutil::String const &);
      
      void checkpoint(void);
	
    public:
      // Constructor
      ServicesInternal(ConfigInternal& c, CDatabase& db);
	
      // Destructor
      ~ServicesInternal(void)
	{};

      // Grab the configuration reference
      const Config& getConfig(void) const
	{ return config; };
      
      // Grab the configuration reference, however for internal use
      ConfigInternal& getConfigInternal(void)
	{ return config; };
      
      // *sigh* grab the console object :(
      Console& getConsole(void)
	{ return console; };

   
      // These will disappear upon full integration with kine..
      void run(void);
      
      // Function Declrations below here.
      int getAccess(AISutil::String &, AISutil::String &);
      void shutdown(const AISutil::String &);
      
      void AddOnlineServer(AISutil::String const &, AISutil::String const &, 
			   AISutil::String const &);
      void DelOnlineServer(AISutil::String const &);
      
      User* const addUser(const AISutil::String& name, const int oid);
      bool delUser(AISutil::String &);
      User* findUser(AISutil::String &);
      
      dChan* const addChan(const AISutil::String& name, const int oid);
      bool delChan(AISutil::String &);
      dChan* findChan(AISutil::String &);
      
      void setNick(User &,AISutil::String &);
      
      void mode(AISutil::String const &, AISutil::String const &,
		AISutil::String const &, AISutil::String const &);
      
      // This shold be in the Service:: class
      void doHelp(User&, AISutil::String const &, AISutil::String const &,
		  AISutil::String const &);
      
      void log(User&, AISutil::String const &, AISutil::String const &,
	       AISutil::String const &);
      void log(User&, AISutil::String const &, AISutil::String const &);
      
      // This is in the wrong place, surely
      void sendEmail(AISutil::String const &, AISutil::String const &,
		     AISutil::String const &);

      void liveLog (AISutil::String const &);

      void servicePart (AISutil::String const &, AISutil::String const &);
      void queueAdd (const AISutil::String& line)
	{
	   if (connected) {
	      //logLine("TX: " + line, Log::Debug);
	      outputQueue.push(line + "\r\n");
	      countTx += line.length();
	   } else {
	     // logLine("Tried to TX " + line + " but not connected",
	//	      Log::Debug);
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
      
      void serviceQuit(const AISutil::String& name,
		       const AISutil::String& reason)
	{
	   queueAdd(":" + name + " QUIT :" + reason);
	};
      
      // This does not modify a server mode, it should be renamed.
      void serverMode(AISutil::String const &chan, AISutil::String const &mode,
		      AISutil::String const &target)
	{
	   queueAdd(":Chan MODE " + chan + " " + mode + " " + target);
	}
      
      void registerService(AISutil::String const &nick,
			   AISutil::String const &user,
			   AISutil::String const &host,
			   AISutil::String const &realname)
	{                               
	   queueAdd ("NICK " + nick + " 1 " + 
		     AISutil::String::convert(currentTime) + " +dz " + user + 
		     " " + host + " " + Kine::config().getOptionsServerName() +
		     " " + Kine::config().getOptionsServerName() + 
		     " 0 1 :" + realname);
        }; 
      
      void serviceJoin(AISutil::String const &service,
		       AISutil::String const &target);
      
      bool usePrivmsg(AISutil::String const &);
      
      // These should be moved to the new channel classes
      void serviceKick(AISutil::String const &, AISutil::String const &,
		       AISutil::String const &);
      bool isOp(AISutil::String const &, AISutil::String const &);
      bool isVoice(AISutil::String const &, AISutil::String const &);
      
      
      // This should be moved to the NOTE MODULE
      void sendNote(AISutil::String const &, AISutil::String const &,
		    AISutil::String const &);
      
      int locateID(AISutil::String const &nick);
      bool isAuthorised(AISutil::String const &);
      User* addClient(AISutil::String const &, AISutil::String const &,
		      AISutil::String const &, AISutil::String const &,
		      AISutil::String const &, AISutil::String const &,
		      AISutil::String const &, AISutil::String const &,
		      AISutil::String const &);
      int getRequiredAccess(AISutil::String const &, AISutil::String const &);
      bool isNickRegistered(AISutil::String const &);
      
      // This should be in the NICK MODULE
      void modeIdentify(AISutil::String const &);
      void updateLastID(AISutil::String const &);
      
      
      int getRegisteredNickID(AISutil::String const &);
      
      AISutil::String getNick(int const &);
      AISutil::String getOnlineNick(int const &);
      AISutil::String getpendingCode(AISutil::String const &);
      void registerNick(AISutil::String const &, AISutil::String const &,
			AISutil::String const &);
      AISutil::String genAuth(AISutil::String const &);
      
      void addOper(AISutil::String const &, int access);
      void delOper(AISutil::String const &);
      bool isOper(AISutil::String const &);
      void validateOper(AISutil::String &);
      
      unsigned long getCountTx(void) const
	{ return countTx; };
      unsigned long getCountRx(void) const
	{ return countRx; };
   }; // class ServicesInternal
}; // namespace Exordium

#endif // _SOURCE_LIB_SERVICES_H_
