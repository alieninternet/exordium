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

# include <kineircd/str.h>
# include <map>
# include <exordium/services.h>
# include <exordium/user.h>

namespace Exordium {
   class Services;
   class User;
   class dChan {
    private:
      AISutil::String name;
      int onlineID;
      Services& services;
      bool registered; /* Are we a registered channel? */

    public:

     /* addUser - Add's a user to the channels member list */
     void const addUser(Exordium::User &ptr, const int &status);
     /* delUser - delete's a user from a channels member list */
     void const delUser(Exordium::User &ptr);
     int const getChanCount(void);
     const AISutil::String& getName(void) const
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
      const AISutil::String getOnlineIDString(void) const
	{ return AISutil::String::convert(onlineID); };
      const int getRegisteredID(void);
      const bool isRegistered(void);
      const int getAccess(AISutil::String const &);
      const void setChanLog(bool const &);
      const void setChanSecure(bool const &);
      const bool isChanSecure(void);
      const void sendBans(AISutil::String &,AISutil::String const &);
      const AISutil::String getOwner(void);
      const int getCount(void);
      const void addAccess(AISutil::String &,AISutil::String const &);
      const void log(User& origin, AISutil::String const &, AISutil::String const &, AISutil::String const &);
      const void updateTopic(AISutil::String const &);
      const void setTopic(AISutil::String const &, AISutil::String const &);
      const bool isOp(AISutil::String const &);
      const void mode(AISutil::String const &, AISutil::String const &,AISutil::String const &);
      const bool isVoice(AISutil::String const &);
      const void kick(AISutil::String const &,AISutil::String const &,AISutil::String const &);
      const void ban(User& origin,AISutil::String const &,AISutil::String const &,AISutil::String const &);
      dChan(AISutil::String const &n, int const &oid, Services &s)
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
