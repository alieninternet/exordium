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
      // Where the KineIRCd daemon instance is
      Kine::Daemon& daemon;
      
      // Our database handler, however this should be in the config class..
      CDatabase& database;
      
      // This will be going soon (plv?)
      Channel channel;
      
      // Constructor
      Services(Kine::Daemon& d, CDatabase& db)
	: daemon(d),
          database(db),
          channel(*this)
	{};
      
    public:
      static const Kine::Utils::base_type PasswordStrBase = 85;
      static const AISutil::String::size_type PasswordStrBaseLongPad = 5;
      
      time_t currentTime;
      
      // this is dodgey and should be passed to modules, *not* set by Parser::
      bool SecurePrivmsg;
      
      // Destructor
      virtual ~Services(void)
	{};
      
      // Grab the daemon reference
      Kine::Daemon& getDaemon(void) const
	{ return daemon; };
      
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
	{ daemon.log(line, (Kine::Logger::Mask::type)mask); };

      static unsigned long random(unsigned long max);
      static AISutil::String generatePassword(AISutil::String const &,
					      AISutil::String const &);
      
      // Function Declrations below here.
      virtual int getAccess(AISutil::String &, AISutil::String &) = 0;
      virtual void shutdown(const AISutil::String &) = 0;
      
      // This should be in the GAME MODULE
      virtual AISutil::String getQuote(int const &number) = 0;
      
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
      virtual AISutil::String getRegNickCount(void) = 0;
      
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
      
      // This does not modify a server mode, it should be renamed.
      virtual void serverMode(AISutil::String const &chan,
			      AISutil::String const &mode,
			      AISutil::String const &target) = 0;
      
      virtual void sendCTCPpingReply(AISutil::String const &from,
				     AISutil::String const &who,
				     AISutil::String & data) = 0;
      
      virtual void sendCTCPversionReply(AISutil::String const &from,
					AISutil::String const &who) = 0;
      
      virtual void registerService(AISutil::String const &nick,
				   AISutil::String const &user,
				   AISutil::String const &host,
				   AISutil::String const &modes,
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
      virtual AISutil::String getPass(AISutil::String const &) = 0;
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
      
      // These definately should be in the NICK MODULE or USER CLASS !!! :(
      virtual AISutil::String getURL(AISutil::String const &) = 0;
      virtual AISutil::String getMSN(AISutil::String const &) = 0;
      virtual AISutil::String getYAHOO(AISutil::String const &) = 0;
      virtual AISutil::String getAIM(AISutil::String const &) = 0;
      virtual AISutil::String getICQ(AISutil::String const &) = 0;
      virtual AISutil::String getEmail(AISutil::String const &) = 0;
      virtual AISutil::String getRegDate(AISutil::String const &) = 0;
      virtual AISutil::String getLastID(AISutil::String const &) = 0;
      virtual AISutil::String getLastHost(AISutil::String const &) = 0;
      
      virtual void addOper(AISutil::String const &, int access) = 0;
      virtual void delOper(AISutil::String const &) = 0;
      virtual bool isOper(AISutil::String const &) = 0;
      virtual void validateOper(AISutil::String &) = 0;
      
      // temporary rubbish below..
      virtual void queueAdd (const AISutil::String& line) = 0;
   }; // class Services
}; // namespace Exordium

# include <exordium/database/database.h>
# include <exordium/user.h>

#endif // _INCLUDE_EXORDIUM_SERVICES_H_
