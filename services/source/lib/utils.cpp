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

#include <aisutil/sha1.h>
#include <kineircd/password.h>

#include "exordium/utils.h"

using AIS::Util::String;
using namespace Exordium;

// Random number generator
unsigned long Exordium::Utils::random(unsigned long max)
{
   return (unsigned long)(((max+1.0) * rand()) / RAND_MAX);
}

/* generatePassword(String,String)
 *
 * Generate a new password for the given user.
 *
 */
String Exordium::Utils::generatePassword(const String& nickname,
					 const String& password)
{
   std::cout << "GeneratePassword(): Nickname :"<<nickname<<": Password :"
     <<
     password << ":" << std::endl;

   //  static const Kine::Utils::base_type PasswordStrBase = 85;
   //   static const AIS::Util::String::size_type PasswordStrBaseLongPad = 5;
   //String pass =
   //Kine::Utils::SHA1::digestToStr(Kine::Password::makePassword(nickname,password),PasswordStrBase,PasswordStrBaseLongPad);
   //std::cout << "GeneratePassword() :" << pass << ":" << std::endl;
   return password;
   //
   //   return String((char *)Kine::Password::makePassword(nickname,
   //						      password).s_char,
   //		 (String::size_type)20);
}

/* generatePassword(String,String)
 *
 * Generate a new password for the given user.
 *
 */
String Exordium::Utils::generateRegcode(const String& nickname,
					const String& password)
{
   std::cout << "GeneratePassword(): Nickname :"<<nickname<<": Password :"
     <<
     password << ":" << std::endl;

   static const unsigned char PasswordStrBase = 85;
   static const AIS::Util::String::size_type PasswordStrBaseLongPad = 5;
   String pass =
     AIS::Util::SHA1::digestToStr(Kine::Password::makePassword(nickname,password),PasswordStrBase,PasswordStrBaseLongPad);
   std::cout << "GeneratePassword() :" << pass << ":" << std::endl;
   return pass;
   //
   //   return String((char *)Kine::Password::makePassword(nickname,
   //						      password).s_char,
   //		 (String::size_type)20);
}

