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

#ifndef _USER_H_
# define _USER_H_

# include <kineircd/str.h>
# include <map>
# include "exordium/services.h"

namespace Exordium {
   class Services;

   class User {
    private:
      LibAIS::String nickname;
      int onlineID;
      Services& services;
      
    public:
      /* Inline functions here */
      /* getNickname()
       * 
       * Return our nickname 
       * 
       */
      
      const LibAIS::String& getNickname(void) const
	{ return nickname; };
      
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
      const LibAIS::String getOnlineIDString(void) const
	{ return LibAIS::String::convert(onlineID); };

      
      /* setNick(String)
       * 
       * Update our records to show a new nickname.....
       * 
       */
      void setNick(const LibAIS::String& nick)
	{ nickname = nick; };
      


      /* Take note. Their are two sendMessages, the latter allowing
       * you to override services default behaviour of using the 
       * nicknames settings of notice/privmsg
       */
      void sendMessage(LibAIS::String const &message,
                       LibAIS::String const &service);
      void sendMessage(LibAIS::String const &message,
                       LibAIS::String const &service,
                       bool const &privmsg);
      
      /* setModNick(bool)
       * 
       * Modify the value of ModNick
       * 
       */
      void setModNick(bool const &);
      
      /* modNick()
       * 
       * Return the current value of modNick
       * 
       */
      bool modNick(void);
      
      /* setDeopAway(bool)
       * 
       * Modify the value of DeopAway
       * 
       */
      void setDeopAway(bool const &);
      
      /* deopAway()
       * 
       * Return the value of DeopAway
       * 
       */
      bool deopAway(void);
      
      /* updateHost(String)
       * 
       * Update the clients hostname.
       * 
       */
      void updateHost(LibAIS::String const &);
      
      /* getQuitMessage()
       * 
       * Return the last known quit message for this user
       * 
       */
      LibAIS::String getQuitMessage(void);
      
      /* getAccess(String)
       * 
       * Return the access for this user with the given
       * service.
       * 
       */
      int getAccess(LibAIS::String const &);
      
      /* addCheckIdentify()
       * 
       * Force a client to identify for this nickname...
       * 
       */
      void addCheckIdentify(void);
      
      /* countHost()
       * 
       * Return the number of online people from this
       * users hostname.
       * 
       */
      int countHost(void);
      
      /* getHost()
       * 
       * Return the users hostname.
       * 
       */
      LibAIS::String getHost(void);
      
      /* getIdent()
       * 
       * Return the users ident (username)
       * 
       */
      LibAIS::String getIdent(void);
      
      /* isIdentified()
       * 
       * Return true if we are identified as *ANY* nickname.
       * 
       */
      bool isIdentified(void);
      
      /* isIdentified(String)
       * 
       * Return true if we are identified as the given nickname.
       * 
       */
      bool isIdentified(LibAIS::String const &);
      
      /* getModes()
       *
       * Query the DB for current user's modes
       *
       */
      LibAIS::String getModes(void);
             
      /* getIDList(String)
       * 
       * Return a list of nicknames we are identified as...
       * 
       */
      LibAIS::String getIDList(void);
      
      /* isPending()
       * 
       * Return TRUE if we are pending verification :-)
       * 
       */
      bool isPending(void);
      
      /* setLanguage(String)
       * 
       * Update our clients language.......
       * 
       * This could possibly be broken.. but i'll look at it later
       * its not a serious quirk (relating to the possibiltiy of someone
       * changing the language for a nick not in use... )
       */
      void setLanguage(LibAIS::String const &);

      /* getLanguage()
       *
       * Retrieve users language
       */
      LibAIS::String getLanguage(void);
      
      /* Our constructor definition */
      User(LibAIS::String const &nick, int const &oid, Services &s)
	: nickname(nick), //their current nickname.. duh
          onlineID(oid), //as dictated by the sql server..
          services(s)// Gives us access to the full suite of services..
	{};

      // is equal to operator
      bool operator==(const User &u) const
	{ return (onlineID == u.onlineID); };
   };
}; // class Exordium

#endif
