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

#ifndef _INCLUDE_EXORDIUM_SERVICES_H_
# define _INCLUDE_EXORDIUM_SERVICES_H_ 1

# include <ctime>
# include <kineircd/str.h>
# include <kineircd/utils.h>
# include <kineircd/password.h>
# include <kineircd/daemon.h>

# include <exordium/config.h>
# include <exordium/channel.h>
# include <exordium/dchan.h>
# include <exordium/log.h>


namespace Exordium {
   class CDatabase;  
   class User;
   
   class Services {
    protected:
      // Our database handler, however this should be in the config class..
      CDatabase& database;
      
      // This will be going soon (plv?)
      Channel channel;
      
      // Constructor
      Services(CDatabase& db)
	: database(db),
          channel(*this)
	{};
      
    public:
      static const int buildNumber = 1;
      time_t currentTime;
      
      // Destructor
      virtual ~Services(void)
	{};
      
      // Grab the database reference
      CDatabase& getDatabase(void)
	{ return database; };
      
      // Grab the configuration reference
      virtual const Config& getConfig(void) const = 0;
      
      // Return the channel thingy
      Channel& getChannel(void)
	{ return channel; };
      
      // Log a line of text..
      void logLine(const std::string& line,
		   const Log::mask_type mask = Log::Informative)
	{ 
Kine::daemon().log(line, (Kine::Logger::Mask::type)mask); 
//liveLog(line);
//	queueAdd(":PeopleChat PRIVMSG #Debug : " + line);
//	servicePrivmsg(line,"PeopleChat","#Debug");
};
      virtual void liveLog(const AISutil::String &) = 0;
      // Function Declrations below here.
      virtual int getAccess(AISutil::String &, AISutil::String &) = 0;
      virtual void shutdown(const AISutil::String &) = 0;
      
      virtual void AddOnlineServer(AISutil::String const &,
				   AISutil::String const &, 
				   AISutil::String const &) = 0;
      virtual void DelOnlineServer(AISutil::String const &) = 0;
      
      virtual User* const addUser(const AISutil::String& name,
				  const int oid) = 0;
      virtual bool delUser(AISutil::String &) = 0;
      virtual User* findUser(AISutil::String &) = 0;
      
      virtual dChan* const addChan(const AISutil::String& name,
				   const int oid) = 0;
      virtual bool delChan(AISutil::String &) = 0;
      virtual dChan* findChan(AISutil::String &) = 0;
      
      virtual void setNick(User &,AISutil::String &) = 0;
      
      virtual void mode(AISutil::String const &, AISutil::String const &,
			AISutil::String const &, AISutil::String const &) = 0;
      
      // This shold be in the Service:: class
      virtual void doHelp(User&, AISutil::String const &,
			  AISutil::String const &,
			  AISutil::String const &) = 0;
      
      virtual void log(User&, AISutil::String const &, 
		       AISutil::String const &, AISutil::String const &) = 0;
      virtual void log(User&, AISutil::String const &,
		       AISutil::String const &) = 0;
      
      // This is in the wrong place, surely
      virtual void sendEmail(AISutil::String const &, AISutil::String const &,
			     AISutil::String const &) = 0;
      
      virtual void servicePart (AISutil::String const &,
				AISutil::String const &) = 0;
      
      //Inline Functions Below here.
      virtual void killnick (AISutil::String const &target,
			     AISutil::String const &from,
			     AISutil::String const &reason) = 0;
      
      virtual void serviceNotice(AISutil::String const &line,
				 AISutil::String const &service, 
				 AISutil::String const &target) = 0;
      
      virtual void serviceNoticeForce(AISutil::String const &line, 
				      AISutil::String const &service,
				      AISutil::String const &target) = 0;
      
      virtual void servicePrivmsg(AISutil::String const &line,
				  AISutil::String const &service, 
				  AISutil::String const &target) = 0;
      
      virtual void serviceQuit(const AISutil::String& name,
			       const AISutil::String& reason) = 0;
      
      // This does not modify a server mode, it should be renamed.
      virtual void serverMode(AISutil::String const &chan,
			      AISutil::String const &mode,
			      AISutil::String const &target) = 0;
      
      virtual void registerService(AISutil::String const &nick,
				   AISutil::String const &user,
				   AISutil::String const &host,
				   AISutil::String const &realname) = 0;
      
      virtual void serviceJoin(AISutil::String const &service,
			       AISutil::String const &target) = 0;
      
      virtual bool usePrivmsg(AISutil::String const &) = 0;
      
      // These should be moved to the new channel classes
      virtual void serviceKick(AISutil::String const &,
			       AISutil::String const &,
			       AISutil::String const &) = 0;
      virtual bool isOp(AISutil::String const &,
			AISutil::String const &) = 0;
      virtual bool isVoice(AISutil::String const &,
			   AISutil::String const &) = 0;
      // And this should be removed when the new channel classes are done.
      virtual void queueAdd(const AISutil::String&) = 0;
      
      
      // This should be moved to the NOTE MODULE
      virtual void sendNote(AISutil::String const &, AISutil::String const &,
			    AISutil::String const &) = 0;
      
      virtual int locateID(AISutil::String const &nick) = 0;
      virtual bool isAuthorised(AISutil::String const &) = 0;
      virtual User* addClient(AISutil::String const &, AISutil::String const &,
			      AISutil::String const &, AISutil::String const &,
			      AISutil::String const &, AISutil::String const &,
			      AISutil::String const &, AISutil::String const &,
			      AISutil::String const &) = 0;
      virtual int getRequiredAccess(AISutil::String const &,
				    AISutil::String const &) = 0;
      virtual bool isNickRegistered(AISutil::String const &) = 0;
      
      // This should be in the NICK MODULE
      virtual void modeIdentify(AISutil::String const &) = 0;
      virtual void updateLastID(AISutil::String const &) = 0;
      
      
      virtual int getRegisteredNickID(AISutil::String const &) = 0;

      virtual AISutil::String getNick(int const &) = 0;
      virtual AISutil::String getOnlineNick(int const &) = 0;
      virtual AISutil::String getpendingCode(AISutil::String const &) = 0;
      virtual void registerNick(AISutil::String const &,
				AISutil::String const &,
				AISutil::String const &) = 0;
      virtual AISutil::String genAuth(AISutil::String const &) = 0;
      
      virtual void addOper(AISutil::String const &, int access) = 0;
      virtual void delOper(AISutil::String const &) = 0;
      virtual bool isOper(AISutil::String const &) = 0;
      virtual void validateOper(AISutil::String &) = 0;
      virtual void sendGOper(AISutil::String const &,
			AISutil::String const &) = 0;
      virtual void sendHelpme(AISutil::String const &,
			AISutil::String const &) = 0;
      virtual void setMode(AISutil::String const &,
			AISutil::String const &) = 0;
      virtual unsigned long getCountTx(void) = 0;
      virtual unsigned long getCountRx(void) = 0;
   }; // class Services
}; // namespace Exordium

# include <exordium/database/database.h>
# include <exordium/user.h>

#endif // _INCLUDE_EXORDIUM_SERVICES_H_
