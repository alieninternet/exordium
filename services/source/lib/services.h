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

# include <exordium/services.h>
# include <exordium/server.h>
# include "parser.h"
# include "console.h"
# include "config.h"

namespace Exordium {
   class ServicesInternal : public Services {
    private:
      Parser parser;
      Console console;

      ConfigInternal& config;

      typedef std::map <AIS::Util::String, User *> user_map;
      typedef std::map <AIS::Util::String, dChan *> chan_map;
      typedef std::map <AIS::Util::String, Server *> server_map;
      
      user_map users;
      chan_map chans;
      server_map servers;
      
      const AIS::Util::String buffer;
      int sock;
      int maxSock;
      char *inputBuffer;
      static const unsigned int inputBufferSize = 512;
      unsigned int inputBufferPosition;
      struct sockaddr_in addr;
//      time_t startTime;
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
      std::queue < AIS::Util::String > outputQueue;
      std::string inputQueue;
      AIS::Util::SocketIPv4TCP socky;
      
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
      AIS::Util::String parseHelp(AIS::Util::String const &);
      
      void checkpoint(void);
	
    public:
      time_t startTime;
      unsigned int buildNumber;
      
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

      time_t getStartTime(void);
      time_t getCurrentTime(void);
	   
   
      // These will disappear upon full integration with kine..
      void run(void);
      
      // Function Declrations below here.
      void shutdown(const AIS::Util::String &);
      

      User* const addUser(const AIS::Util::String& name, const int oid);
      bool delUser(Kine::Name &);
      User* findUser(Kine::Name &);
      
      dChan* const addChan(const Kine::Name& name, const int oid);
      bool delChan(Kine::Name &);
      dChan* findChan(Kine::Name &);
      
      Server* const addServer(const AIS::Util::String&, const int&,
			      const AIS::Util::String&);
      bool delServer(AIS::Util::String &);
      Server* findServer(AIS::Util::String &);
      
      
      
      void setNick(User &,Kine::Name &);
      
      void mode(AIS::Util::String const &, AIS::Util::String const &,
		AIS::Util::String const &, AIS::Util::String const &);
      
      // This shold be in the Service:: class
      void doHelp(User&, AIS::Util::String const &, AIS::Util::String const &,
		  AIS::Util::String const &);
      
      void log(User&, AIS::Util::String const &, AIS::Util::String const &,
	       AIS::Util::String const &);
      void log(User&, AIS::Util::String const &, AIS::Util::String const &);
      
      // This is in the wrong place, surely
      void sendEmail(AIS::Util::String const &, AIS::Util::String const &,
		     AIS::Util::String const &);

      void servicePart (AIS::Util::String const &, AIS::Util::String const &);
      void queueAdd (const AIS::Util::String& line);


      void killnick (AIS::Util::String const &target,
		     AIS::Util::String const &from,
		     AIS::Util::String const &reason)
	{
	   queueAdd(":" + from + " KILL " + target + " :" + reason);
	};
      
      void serviceNotice(AIS::Util::String const &line,
			 AIS::Util::String const &service, 
			 AIS::Util::String const &target)
	{
	   if (usePrivmsg(target.toLower())) {
	      queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	   } else {
	      queueAdd(":" + service + " NOTICE " + target + " :" + line);
	   }
	}
      
      void serviceNoticeForce(AIS::Util::String const &line, 
			      AIS::Util::String const &service,
			      AIS::Util::String const &target)
	{
	   queueAdd(":" + service + " NOTICE " + target + " :" + line);
	}
      
      void servicePrivmsg(AIS::Util::String const &line,
			  AIS::Util::String const &service, 
			  AIS::Util::String const &target)
	{
	   queueAdd(":" + service + " PRIVMSG " + target + " :" + line);
	}
      
      void serviceQuit(const AIS::Util::String& name,
		       const AIS::Util::String& reason)
	{
	   queueAdd(":" + name + " QUIT :" + reason);
	};
      
      // This does not modify a server mode, it should be renamed.
      void serverMode(AIS::Util::String const &chan, AIS::Util::String const &mode,
		      AIS::Util::String const &target)
	{
	   queueAdd(":Chan MODE " + chan + " " + mode + " " + target);
	}
      
      void registerService(AIS::Util::String const &nick,
			   AIS::Util::String const &user,
			   AIS::Util::String const &host,
			   AIS::Util::String const &realname)
	{                               
	   queueAdd ("NICK " + nick + " 1 1 +o " + user + 
		     " " + host + " " + Kine::config().getOptionsServerName() +
		     " 0 0 :" + realname);
        }; 
      
      void serviceJoin(AIS::Util::String const &service,
		       AIS::Util::String const &target);
      
      bool usePrivmsg(AIS::Util::String const &);
      
      // These should be moved to the new channel classes
      void serviceKick(AIS::Util::String const &, AIS::Util::String const &,
		       AIS::Util::String const &);
      bool isOp(AIS::Util::String const &, AIS::Util::String const &);
      bool isVoice(AIS::Util::String const &, AIS::Util::String const &);
      
      
      // This should be moved to the NOTE MODULE
      void sendNote(Kine::Name const &, Kine::Name const &,
		    AIS::Util::String const &);
      
      int locateID(Kine::Name const &nick);
      User* addClient(Kine::Name const &, AIS::Util::String const &,
		      AIS::Util::String const &, AIS::Util::String const &,
		      AIS::Util::String const &, AIS::Util::String const &,
		      AIS::Util::String const &,
		      AIS::Util::String const &);
      int getRequiredAccess(AIS::Util::String const &, AIS::Util::String const &);
      bool isNickRegistered(AIS::Util::String const &);
      // This should be in the NICK MODULE
      void modeIdentify(AIS::Util::String const &);
      void updateLastID(AIS::Util::String const &);
      
      
      
      AIS::Util::String getNick(int const &);
      AIS::Util::String getOnlineNick(int const &);
      int getOnlineChanID(AIS::Util::String const &);
      AIS::Util::String getpendingCode(AIS::Util::String const &);
      void registerNick(AIS::Util::String const &, AIS::Util::String const &,
			AIS::Util::String const &);
      void addOper(AIS::Util::String const &, int access);
      void delOper(AIS::Util::String const &);
      bool isOper(AIS::Util::String const &);
      void validateOper(Kine::Name &);
      void sendGOper(AIS::Util::String const &, AIS::Util::String const &);
      void sendHelpme(AIS::Util::String const &, AIS::Util::String const &);
      void setMode(AIS::Util::String const &, AIS::Util::String const &);      
      unsigned long getCountTx(void);
//	{ return countTx; };
      unsigned long getCountRx(void);
//	{ return countRx; };
   }; // class ServicesInternal
}; // namespace Exordium

#endif // _SOURCE_LIB_SERVICES_H_
