/*
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 *
 * This file is a part of Exordium.
 *
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,USA
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
# include "exordium/sql.h"

namespace Exordium
{
   class Services;
   
   class User
     {
      private:
	Kine::String nickname;
	int onlineID;
	Services& services;

      public:
	/* Inline functions here */
	/* getNickname()
	 * 
	 * Return our nickname 
	 * 
	 */
	
	Kine::String getNickname(void)
	  {
	     return nickname;
	  };
	
	/* getOnlineID()
	 * 
	 * Return our unique sql identifier
	 * 
	 */
	int getOnlineID(void)
	  {
	     return onlineID;
	  };
	/* getOnlineIDString()
	 * 
	 * Return our unique sql identifier 
	 * (as a String)
	 */
	Kine::String getOnlineIDString(void)
	  {
	     return String::convert(onlineID);
	  };
	/* setNick(String)
	 * 
	 * Update our records to show a new nickname.....
	 * 
	 */
        void setNick(String const &nick)
	  {
	     
	                  nickname = nick;
	  };
	/* Take note. Their are two sendMessages, the latter allowing
	 * you to override services default behaviour of using the 
	 * nicknames settings of notice/privmsg
	 */
	void sendMessage(String const &message,String const &origin);
	void sendMessage(String const &message,String const &origin, bool const &privmsg);
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
	void updateHost(String const &);
	/* getQuitMessage()
	 * 
	 * Return the last known quit message for this user
	 * 
	 */
	Kine::String getQuitMessage(void);
	/* getAccess(String)
	 * 
	 * Return the access for this user with the given
	 * service.
	 * 
	 */
	int getAccess(Kine::String const &);
	/* addCheckIdentify()
	 * 
	 * Force a client to identify for this nickname...
	 * 
	 */
	void addCheckIdentify(void);
	
	/* Our constructor definition */
	User(String const &nick, int const &oid, Services &s)
	  : nickname(nick), //their current nickname.. duh
	    onlineID(oid), //as dictated by the sql server..
	    services(s)// Gives us access to the full suite of services..
	  {
	     
	  };

     };

};

#endif
