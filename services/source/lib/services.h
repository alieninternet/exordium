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

# include <kineircd/config.h>

# include <exordium/services.h>
# include <exordium/server.h>
# include "console.h"
# include "config.h"

namespace Exordium {
   class ServicesInternal : public Services {
    private:
      Console console;

      ConfigInternal& config;

      typedef std::map <AISutil::String, User *> user_map;
      typedef std::map <AISutil::String, dChan *> chan_map;
      typedef std::map <AISutil::String, Server *> server_map;
      
      user_map users;
      chan_map chans;
      server_map servers;
      
      time_t lastCheckPoint;
      time_t lastExpireRun;
      
      void SynchTime(void);
      AISutil::String parseHelp(AISutil::String const &);
      
      void checkpoint(void);
	
    public:
      // Constructor
      ServicesInternal(ConfigInternal& c, CDatabase& db);
	
      // Destructor
      ~ServicesInternal(void);

      // Grab the configuration reference
      const Config& getConfig(void) const
	{ return config; };
      
      // Grab the configuration reference, however for internal use
      ConfigInternal& getConfigInternal(void)
	{ return config; };
      
      // *sigh* grab the console object :(
      Console& getConsole(void)
	{ return console; };


      User* const addUser(const AISutil::String& name, const int oid);
      bool delUser(Kine::ClientName &);
      User* findUser(Kine::ClientName &);
      
      dChan* const addChan(const Kine::ChannelName& name, const int oid);
      bool delChan(Kine::ChannelName &);
      dChan* findChan(Kine::ChannelName &);
      
      Server* const addServer(const AISutil::String&, const int&,
			      const AISutil::String&);
      bool delServer(AISutil::String &);
      Server* findServer(AISutil::String &);
      
      
      
      void setNick(User &,Kine::ClientName &);
      
      // This shold be in the Service:: class
      void doHelp(User&, AISutil::String const &, AISutil::String const &,
		  AISutil::String const &);
      
      void log(User&, AISutil::String const &, AISutil::String const &,
	       AISutil::String const &);
      void log(User&, AISutil::String const &, AISutil::String const &);
      
      // This is in the wrong place, surely
      void sendEmail(AISutil::String const &, AISutil::String const &,
		     AISutil::String const &);

      bool usePrivmsg(AISutil::String const &);
      
      // These should be moved to the new channel classes
      bool isOp(const Kine::ClientName& nick,
		const Kine::ChannelName& channel);
      bool isVoice(AISutil::String const &, AISutil::String const &);
      
      
      // This should be moved to the NOTE MODULE
      void sendNote(Kine::ClientName const &, Kine::ClientName const &,
		    AISutil::String const &);
      
      int locateID(Kine::ClientName const &nick);
      User* addClient(Kine::ClientName const &, AISutil::String const &,
		      AISutil::String const &, AISutil::String const &,
		      AISutil::String const &, AISutil::String const &,
		      AISutil::String const &,
		      AISutil::String const &);
      int getRequiredAccess(AISutil::String const &, AISutil::String const &);
      bool isNickRegistered(AISutil::String const &);
      // This should be in the NICK MODULE
      void updateLastID(AISutil::String const &);
      
      
      
      AISutil::String getNick(int const &);
      AISutil::String getOnlineNick(int const &);
      int getOnlineChanID(AISutil::String const &);
      AISutil::String getpendingCode(AISutil::String const &);
      void registerNick(AISutil::String const &, AISutil::String const &,
			AISutil::String const &);
      void addOper(AISutil::String const &, int access);
      void delOper(AISutil::String const &);
      bool isOper(AISutil::String const &);
      void validateOper(Kine::ClientName &);
   }; // class ServicesInternal
}; // namespace Exordium

#endif // _SOURCE_LIB_SERVICES_H_
