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

#include "exordium/services.h"
#include "exordium/user.h"
#include "exordium/userbase.h"

#include <kineircd/str.h>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

/* Add a user into our map thingy :*(
 * and return its new record just to be nice (if they want it)
 *
 */

User*
  UserBase::addUser(Kine::String &name, int &oid)
{
   usercount++;
   users[oid] = new User(name.IRCtoLower(),oid,services);
   services.Debug("Have added new client named " + name);
   return users[oid];
};

/* Find, and return the record associated with a nickname
 * 
 * Since users are stored via a unique integer, we need
 * to firstly retrieve that, then grab the record.
 */

User*
  UserBase::findUser(Kine::String &name)
{
   int uid = services.locateID(name);
   if(uid==0) /* The user was not found... */
     {
	return 0;
     }
   User *ptr = users[uid];
   return ptr;
};


/* delUser(who)
 * 
 * Deletes the record for the given user... returns true on success
 * 
 */
bool
  UserBase::delUser(Kine::String &name)
{
   for(user_map::iterator it = users.begin();
       it != users.end(); it++)
     {
	std::cout << "Looping" << std::endl;
	int id = (*it).first;
	User *ptr = (*it).second;
	if(ptr->getNickname() == name.IRCtoLower())
	  {
	     //Found our record.. delete it based on its id
	     delete ptr;
	     users.erase(id);
	     return true;
	  }
	
     }
   /* If we get here .. we can't find the record they've asked for.. return false
    * so they can deal with it how they choose... */
   
   return false;
};
