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

#ifndef __USER_H_
#define __USER_H_

#include <kineircd/str.h>
#include <map>


namespace Exordium {

   class User {
    private:  	   
      typedef std::map <int, User *> modules_type;
      modules_type Users;
      
    public:
      User(void) 
	{
	   Users.clear();
	};
      
      bool test(void)
	{
	   std::cout << "This is proof that you have access Users::" << std::endl;
	   return true;
	};
      
      
      
      
   };
};

#endif
