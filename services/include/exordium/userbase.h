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

#ifndef _USERBASE_H
# define _USERBASE_H

# include "exordium/user.h"
# include "exordium/services.h"


namespace Exordium
{
   class Services;
   class UserBase
     {

      private:
	typedef std::map <int, User *> user_map;
	user_map users;
	int usercount;
	Services& services;
      public:
	UserBase(Services& s)
	  : services(s)
	    {
	       users.clear(); //Simon said so!
	       usercount = 0; //We don't start with any clients you know
	    };
	
	User* addUser(Kine::String &, int &);
	bool delUser(Kine::String &);
	User* findUser(Kine::String &);

     };

};
#endif
