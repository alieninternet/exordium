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

#ifndef _INCLUDE_EXORDIUM_USER_H_
# define _INCLUDE_EXORDIUM_USER_H_ 1

#include <kineircd/str.h>
#include <ctime>
#include <map>
#include <vector>
#include <exordium/singleton.h>

using AISutil::String;
using namespace std;

namespace Exordium {

   #define userListFwd (*UserList::instance())

   class User
   {

    private:
      AISutil::String nick;
      int hops;
      AISutil::String timestamp;
      AISutil::String username;
      AISutil::String hostname;
      AISutil::String vwhost;
      AISutil::String server;
      AISutil::String modes;
      AISutil::String realname;

      vector<AISutil::String> identified;
      vector<AISutil::String> pendingnicks;

    public:

      // Getters
      AISutil::String  getNick(void)      { return nick; }
      int              getHops(void)      { return hops; }
      AISutil::String  getTimestamp(void) { return timestamp; }
      AISutil::String  getUsername(void)  { return username; }
      AISutil::String  getHostname(void)  { return hostname; }
      AISutil::String  getVwhost(void)    { return vwhost; }
      AISutil::String  getServer(void)    { return server; }
      AISutil::String  getModes(void)     { return modes; }
      AISutil::String  getRealname(void)  { return realname; }

      // Setters
      void setNick      (AISutil::String mynick)      { nick=mynick; }
      void setHops      (int myhops)                  { hops=myhops; }
      void setTimestamp (AISutil::String mystamp)     { timestamp=mystamp; }
      void setUsername  (AISutil::String myusername)  { username=myusername; }
      void setHostname  (AISutil::String myhostname)  { hostname=myhostname; }
      void setVwhost    (AISutil::String myvwhost)    { vwhost=myvwhost; }
      void setServer    (AISutil::String myserver)    { server=myserver; }
      void setModes     (AISutil::String mymodes)     { modes=mymodes; }
      void setRealname  (AISutil::String myrealname)  { realname=myrealname; }


      void addIdentifiedNick(AISutil::String const &nick);
      void delIdentifiedNick(AISutil::String const &nick);

      bool isIdentified(AISutil::String const &nick);
      bool isIdentified(void);
 
      int nbIdentifiedNicks(void);

      AISutil::String getIDList(void);

      void addPendingNick(AISutil::String const &nick);
      void delPendingNick(AISutil::String const &nick);
      bool isPending(AISutil::String const &nick);

      int nbPengingNicks(void);

      void sendMessage(String const &message, String const &service);
      void sendMessage(String const &message, String const &service, bool const &privmsg);
   };


   class UserList:public Singleton<UserList>
   {
      public:
                friend class Singleton<UserList>;

                ~UserList(void);

                void  addUser(User *newuser);
                void  addUser(AISutil::String const &nick, int hops,
                              AISutil::String const &timestamp, AISutil::String const &username,
                              AISutil::String const &host, AISutil::String const &vwhost,
                              AISutil::String const &server, AISutil::String const &modes,
                              AISutil::String const &realname);

                void  delUser(AISutil::String const &nick);
                void  delUserByServer(AISutil::String const &server);
                User* getUser(AISutil::String const &nick);

                bool  userExists(AISutil::String const &nick);
                int   nbUsers(void);

      private:
                UserList() {}
                map<AISutil::String, User*> users;
   };

}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_USER_H_
