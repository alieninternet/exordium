/*
 *  * Exordium Network Services
 *  * Copyright (C) 2002 IRCDome Development Team
 *  *
 *  * This file is a part of Exordium.
 *  *
 *  * $Id$
 *  *
 *  * This program is free software; you can redistribute it and/or modify
 *  * it under the terms of the GNU General Public License as published by
 *  * the Free Software Foundation; either version 2 of the License, or
 *  * (at your option) any later version.
 *  *
 *  * This program is distributed in the hope that it will be useful,
 *  * but WITHOUT ANY WARRANTY; without even the implied
 *  * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  * See the GNU General Public License for more details.
 *  *
 *  * You should have received a copy of the GNU General Public License
 *  * along with this program; if not, write to the Free Software
 *  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,USA
 *  *
 *  * For contact details for the IRCDome Development Team please see the
 *  * enclosed README file.
 *  *
 * */

#ifndef __NICKNAME_H_
#define __NICKNAME_H_

#include <kineircd/str.h>

namespace Exordium {
   class Services;
   
   class Nickname {
    private:
      Services& services;
      
    public:
      Nickname(Services& s)
	: services(s)
	{};
      Kine::String stripModes(Kine::String const &nick);
      Kine::String getRegNickCount(void);
      Kine::String getOnlineCount(void);
      void updateHost(Kine::String const &, Kine::String const &);
      Kine::String generatePassword(Kine::String const &, Kine::String const &);
      int getAccess(Kine::String const &, Kine::String const &);
      bool isAuthorised(Kine::String const &);
      void addCheckidentify(Kine::String const &);
      void addClient (Kine::String nick, Kine::String hops, Kine::String timestamp,
		      Kine::String username, Kine::String host, Kine::String vwhost,
		      Kine::String server, Kine::String modes, Kine::String realname);
      int countHost(Kine::String const &);
      Kine::String getQuitMessage(Kine::String const &);
      int getRequiredAccess(Kine::String const &, Kine::String const &);
      bool isNickRegistered (Kine::String const &);
      Kine::String getPass (Kine::String const &);
      int getOnlineNickID(Kine::String const &);    
      int getRegisteredNickID(Kine::String const &);
      void modeIdentify (Kine::String const &);
      void updateLastID(Kine::String const &);
      Kine::String getHost(Kine::String const &);
      Kine::String getIdent(Kine::String const &);
      bool isIdentified(Kine::String const &);
      bool isIdentified(Kine::String const &, Kine::String const &);
      Kine::String getNick(int const &);
      Kine::String getOnlineNick(int const &);
      Kine::String getIDList(Kine::String const &);
      Kine::String getpendingCode(Kine::String const &);  
      bool isPending(Kine::String const &);
      void registerNick(Kine::String const &,Kine::String const &,Kine::String const &);
      Kine::String genAuth(Kine::String const &);   
      void setLanguage(Kine::String const &,Kine::String const &);
      Kine::String getICQ (Kine::String const &nick);
      Kine::String getMSN (Kine::String const &nick);
      Kine::String getYAHOO (Kine::String const &nick);
      Kine::String getAIM (Kine::String const &nick);
      Kine::String getURL (Kine::String const &nick);
      Kine::String getLanguage(Kine::String const &nick);
      Kine::String getEmail (const Kine::String & nick);
      Kine::String getLastHost (const Kine::String &nick);
      Kine::String getLastID (const Kine::String &nick);  
      Kine::String getRegDate (const Kine::String &nick);
      void setDeopAway(Kine::String const &, bool const &);
      bool deopAway(Kine::String const &);
      void setModNick(Kine::String const &, bool const &);
      
      // I think this got removed and someone forgot to commit!!
//      bool modNick(Kine::String const &);
   };
};

// Complete the forwarded definition
# include "exordium/services.h"

#endif
