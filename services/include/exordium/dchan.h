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

#ifndef _INCLUDE_EXORDIUM_DCHAN_H_
# define _INCLUDE_EXORDIUM_DCHAN_H_ 1

# include <aisutil/string.h>
# include <map>
# include <exordium/services.h>
# include <exordium/user.h>

namespace Exordium {
   class Services;
   class User;
   class dChan {
    private:
      AIS::Util::String name;
      int onlineID;
      Services& services;
      bool registered; /* Are we a registered channel? */

    public:

     /* addUser - Add's a user to the channels member list */
     void const addUser(Exordium::User &ptr, const int &status);
     /* delUser - delete's a user from a channels member list */
     void const delUser(Exordium::User &ptr);
     int const getChanCount(void);
     const AIS::Util::String& getName(void) const
	{ return name; };

      
      /* getOnlineID()
       * 
       * Return our unique sql identifier
       * 
       */
      const int getOnlineID(void) const
	{ return onlineID; };
      
      /* getOnlineIDString()
       * 
       * Return our unique sql identifier 
       * (as a String)
       * 
       * This is redundant imho. - simon
       */
      const AIS::Util::String getOnlineIDString(void) const
	{ return AIS::Util::String::convert(onlineID); };
      const int getRegisteredID(void);
      const bool isRegistered(void);
      const int getAccess(AIS::Util::String const &);
      const void setChanLog(bool const &);
      const void setChanSecure(bool const &);
      const bool isChanSecure(void);
      const void sendBans(Kine::Name &,AIS::Util::String const &);
      const AIS::Util::String getOwner(void);
      const int getCount(void);
      const void addAccess(AIS::Util::String &,AIS::Util::String const &);
      const void log(User& origin, AIS::Util::String const &, AIS::Util::String const &, AIS::Util::String const &);
      const void updateTopic(AIS::Util::String const &);
      const void setTopic(AIS::Util::String const &, AIS::Util::String const &);
      const AIS::Util::String getTopic(void);
      const bool isOp(AIS::Util::String const &);
      const void mode(AIS::Util::String const &, AIS::Util::String const &,AIS::Util::String const &);
      const bool isVoice(AIS::Util::String const &);
      const void kick(AIS::Util::String const &,AIS::Util::String const &,AIS::Util::String const &);
      const void ban(User& origin,AIS::Util::String const &,AIS::Util::String const &,AIS::Util::String const &);

      const bool getTrackTopics( void );
      const void setTrackTopics( bool Value );

      const bool getEnforceBans( void );
      const void setEnforceBans( bool Value );

      const bool getModeLock( void );
      const void setModeLock( bool Value );

      const AIS::Util::String getChannelModes( void );
      const void setChannelModes( const AIS::Util::String& modes );

      const AIS::Util::String getEntryMsg( void );
      const void setEntryMsg( const AIS::Util::String& entrymsg );

      const AIS::Util::String getPartMsg( void );
      const void setPartMsg( const AIS::Util::String& partmsg );

      const bool getPrivate( void );
      const void setPrivate( bool Value );

      const AIS::Util::String getChanDescription( void );
      const void setChanDescription( const AIS::Util::String& chandesc );

      const AIS::Util::String getUrl( void );
      const void setUrl( const AIS::Util::String& url );

      const AIS::Util::String dChan::getRegistrationDate();

      dChan(AIS::Util::String const &n, int const &oid, Services &s)
	: name(n), //their current nickname.. duh
          onlineID(oid), //as dictated by the sql server..
          services(s)// Gives us access to the full suite of services..
	{};

      ~dChan(void)
	{};
      
      // is equal to operator
      bool operator==(const dChan &c) const
	{ return (onlineID == c.onlineID); };
   };
}; // class Exordium

#endif // _INCLUDE_EXORDIUM_DCHAN_H_
