/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _INCLUDE_EXORDIUM_SERVICES_H_
# define _INCLUDE_EXORDIUM_SERVICES_H_ 1

# include <ctime>
# include <aisutil/string.h>
# include <kineircd/password.h>
# include <kineircd/daemon.h>
# include <kineircd/clientname.h>
# include <kineircd/channelname.h>

# include <exordium/config.h>
# include <exordium/channel.h>
# include <exordium/dchan.h>
# include <exordium/log.h>
# include <exordium/static.h>
# include <exordium/server.h>


namespace Exordium {
   class CDatabase;  
   class User;
   
   class Services {
    protected:
      // Our database handler, however this should be in the config class..
      CDatabase& database;
      
      // This will be going soon (plv?)
      Channel channel;
      
      Static gstatic;
      // Constructor
      Services(CDatabase& db)
	: database(db),
          channel(*this),
          gstatic(*this)
	{};
      
    public:
      static const int buildNumber = 1;
      time_t startTime;
      
      // Destructor
      virtual ~Services(void)
	{};
      
      // Grab the database reference
      CDatabase& getDatabase(void)
	{ return database; };
      
      // Grab the configuration reference
      virtual const Config& getConfig(void) const = 0;
      
      // Return the channel reference
      Channel& getChannel(void)
	{ return channel; };
      
      // Return the static reference
      Static& getStatic(void)
	{ return gstatic; };
	   
      
      // Log a line of text..
      void logLine(const std::string& line,
		   const Log::mask_type mask = Log::Informative)
	{ 
          Kine::daemon().log(line, (Kine::Logger::Mask::type)mask); 
        };
      
      // 'User' operability functions
      virtual User* const addUser(const AISutil::String& name,
				  const int oid) = 0;
      virtual bool delUser(Kine::ClientName &) = 0;
      virtual User* findUser(Kine::ClientName &) = 0;
      
      // 'dChan' operability functions
      virtual dChan* const addChan(const Kine::ChannelName& name,
				   const int oid) = 0;
      virtual bool delChan(Kine::ChannelName &) = 0;
      virtual dChan* findChan(Kine::ChannelName &) = 0;
      
      // 'Server' operability functions
      virtual Server* const addServer(const AISutil::String&,
				      const int &,
				      const AISutil::String &) = 0;
      
      virtual bool delServer(AISutil::String &) = 0;
      
      virtual Server* findServer(AISutil::String &) = 0;
				      
      
      // Allows Services:: to change a User's nick.
      virtual void setNick(User &, Kine::ClientName &) = 0;
      
      // Send modes.
      void mode(AISutil::String const &, AISutil::String const &,
		AISutil::String const &, AISutil::String const &) {};

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
      
      void servicePart (AISutil::String const &,
			AISutil::String const &) {};
      
      //Inline Functions Below here.
      void killnick(AISutil::String const &target,
		    AISutil::String const &from,
		    AISutil::String const &reason) {};
      
      void serviceNotice(AISutil::String const &line,
			 AISutil::String const &service, 
			 AISutil::String const &target) {};
      
      void serviceNoticeForce(AISutil::String const &line, 
			      AISutil::String const &service,
			      AISutil::String const &target) {};
      
      void servicePrivmsg(AISutil::String const &line,
			  AISutil::String const &service,
			  AISutil::String const &target) {};
      
      // This does not modify a server mode, it should be renamed.
      void serverMode(AISutil::String const &chan,
		      AISutil::String const &mode,
		      AISutil::String const &target) {};
      
      void serviceJoin(AISutil::String const &service,
		       AISutil::String const &target) {};
      
      virtual bool usePrivmsg(AISutil::String const &) = 0;
      
      // These should be moved to the new channel classes
      void serviceKick(AISutil::String const &,
		       AISutil::String const &,
		       AISutil::String const &) {};
      virtual bool isOp(Kine::ClientName const &,
			Kine::ChannelName const &) = 0;
      virtual bool isVoice(AISutil::String const &,
			   AISutil::String const &) = 0;
      
      
      // This should be moved to the NOTE MODULE
      virtual void sendNote(Kine::ClientName const &, Kine::ClientName const &,
			    AISutil::String const &) = 0;
      
      virtual int locateID(Kine::ClientName const &nick) = 0;
      virtual User* addClient(Kine::ClientName const &, AISutil::String const &,
			      AISutil::String const &, AISutil::String const &,
			      AISutil::String const &, AISutil::String const &,
			      AISutil::String const &,
			      AISutil::String const &) = 0;
      virtual int getRequiredAccess(AISutil::String const &,
				    AISutil::String const &) = 0;
      virtual bool isNickRegistered(AISutil::String const &) = 0;
      
      // This should be in the NICK MODULE
      void modeIdentify(AISutil::String const &) {};
      virtual void updateLastID(AISutil::String const &) = 0;
      
      

      virtual AISutil::String getNick(int const &) = 0;
      virtual AISutil::String getOnlineNick(int const &) = 0;
      virtual int getOnlineChanID(AISutil::String const &) = 0;
      virtual void addOper(AISutil::String const &, int access) = 0;
      virtual void delOper(AISutil::String const &) = 0;
      virtual bool isOper(AISutil::String const &) = 0;
      virtual void validateOper(Kine::ClientName &) = 0;
      void sendGOper(AISutil::String const &,
		     AISutil::String const &) {};
      void sendHelpme(AISutil::String const &,
		      AISutil::String const &) {};
      void setMode(AISutil::String const &,
		   AISutil::String const &) {};
      time_t getStartTime(void) { return startTime; };
   }; // class Services
}; // namespace Exordium

# include <exordium/database/database.h>
# include <exordium/user.h>

#endif // _INCLUDE_EXORDIUM_SERVICES_H_
