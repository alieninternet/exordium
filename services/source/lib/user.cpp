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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "exordium/user.h"
#include "exordium/database/database.h"
#include "exordium/services.h"

namespace Exordium
{

void User::addIdentifiedNick(AISutil::String const &nick)
{
   if(!isIdentified(nick))
     identified.push_back(nick);
}

void User::delIdentifiedNick(AISutil::String const &nick)
{
   vector<String>::iterator pos=find(identified.begin(), identified.end(), nick);

   if(pos!=identified.end())
     identified.erase(pos);
}

bool User::isIdentified(AISutil::String const &nick)
{
   vector<String>::iterator pos=find(identified.begin(), identified.end(), nick);

   if(pos!=identified.end())
     return true;
   else
     return false;
}


bool User::isIdentified(void)
{
  if(identified.size()>0)
    return true;
  else
    return false;
}

int User::nbIdentifiedNicks(void)
{
  return identified.size();
}

String User::getIDList(void)
{
  String idlist="";

  for(int i=0; i<identified.size(); i++)
  {
    idlist+=identified[i]+=" ";
  }

  return idlist;
}

void UserList::delUserByServer(AISutil::String const &server)
{
  map<String, User*>::iterator useriter;

  for(useriter=users.begin(); useriter!=users.end(); ++useriter)
  {
     if(useriter->second->getServer()==server)
        delUser(useriter->second->getNick());
  }

}


/* sendMessage(String,String,<bool>)
 *
 * Two versions of sendMessage, the first which will use
 * the settings from the nickname to determine whether
 * they have choosen the privmsg or notice interface for services.
 *
 * The latter version allowing you to dictate how you wish to send
 * the message to the user by setting the boolean value to TRUE
 * for a privmsg, FALSE for a notice.
 *
 */

void User::sendMessage(String const &message, String const &service)
{
   servicesFwd.serviceNotice(message,service,getNick());
}

void User::sendMessage(String const &message, String const &service, bool const &privmsg)
{
   if(privmsg)
     {
        servicesFwd.servicePrivmsg(message,service,getNick());
     }
   else
     {
        servicesFwd.serviceNoticeForce(message,service,getNick());
     }
}


void User::addPendingNick(AISutil::String const &nick)
{
   if(!isPending(nick))
   {
     pendingnicks.push_back(nick);
     sendMessage("[Identification Request] This nickname is registered, please identify by typing /msg nick@ircdome.org identify <password>","Nick");
     databaseFwd.dbInsert("kills", "'','"+getNick()+"','"+String::convert(servicesFwd.getCurrentTime() + 120 +(servicesFwd.random(60)))+"'");
   }
}

void User::delPendingNick(AISutil::String const &nick)
{
   vector<String>::iterator pos=find(pendingnicks.begin(), pendingnicks.end(), nick);

   if(pos!=pendingnicks.end())
   {
     pendingnicks.erase(pos);
     databaseFwd.dbDelete("kills", "nick='"+getNick()+"'");
   }
}


bool User::isPending(AISutil::String const &nick)
{
   vector<String>::iterator pos=find(pendingnicks.begin(), pendingnicks.end(), nick);

   if(pos!=pendingnicks.end())
     return true;
   else
     return false;
}


int  User::nbPengingNicks(void)
{
   return pendingnicks.size();
}



UserList::~UserList(void)
{
  map<String, User*>::iterator useriter;

  for(useriter=users.begin(); useriter!=users.end(); ++useriter)
  {
     delete (*useriter).second;
  }

  users.clear();
}


void  UserList::addUser(User *newuser)
{
  if(newuser!=NULL)
     if(!userExists(newuser->getNick()))
        users[newuser->getNick()]=newuser;

// NOTE to self:  Dont forget to add logging here in else for inconsistency
}


void  UserList::addUser(String const &nick, int hops, String const &timestamp, String const &username,
                        String const &host, String const &vwhost, String const &server, 
                        String const &modes, String const &realname)
{
   if(!userExists(nick))
   {
     User *newuser=new User;

     newuser->setNick(nick);
     newuser->setHops(hops);
     newuser->setTimestamp(timestamp);
     newuser->setUsername(username);
     newuser->setHostname(host);
     newuser->setVwhost(vwhost);
     newuser->setServer(server);
     newuser->setModes(modes);
     newuser->setRealname(realname);

     users[nick]=newuser;
   }
}


void  UserList::delUser(String const &nick)
{
  if(userExists(nick))
  {
    User *u=users[nick];
    users.erase(nick);
    delete u;
  }
}


User* UserList::getUser(String const &nick)
{
  if(userExists(nick))  
    return users[nick];
  else
    return NULL;
}

bool UserList::userExists(String const &nick)
{
  if(users.find(nick)!=users.end())
    return true;
  else
    return false;
}

int UserList::nbUsers(void)
{
  return users.size();
}

}; // End namespace Exordium
