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
	Kine::String getNickname(void)
	  {
	     return nickname;
	  };
	int getOnlineID(void)
	  {
	     return onlineID;
	  };
	String getOnlineIDString(void)
	  {
	     return String::convert(onlineID);
	  };
	
	/* Take note. Their are two sendMessages, the latter allowing
	 * you to override services default behaviour of using the 
	 * nicknames settings of notice/privmsg
	 */
	void sendMessage(String const &message,String const &origin);
	void sendMessage(String const &message,String const &origin, bool const &privmsg);
	void setModNick(bool const &);
	bool modNick(void);
	void setDeopAway(bool const &);
        bool deopAway(void);
	void updateHost(String const &);
	void setNick(String const &nick)
	  {
	     nickname = nick;
	  };
	User(String const &nick, int const &oid, Services &s)
	  : nickname(nick), //their current nickname.. duh
	    onlineID(oid), //as dictated by the sql server..
	    services(s)// Gives us access to the full suite of services..
	  {
	     
	  };

     };

};

#endif
