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
#include <kineircd/str.h>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

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

void
  User::sendMessage(String const &message,String const &origin)
{
   services.serviceNotice(message,origin,nickname);
}

void 
  User::sendMessage(String const &message, String const &origin, bool const &privmsg)
{
    if(privmsg)
     {
	services.servicePrivmsg(message,origin,nickname);
     }
   else
     {
	services.serviceNoticeForce(message,origin,nickname);
     }
   
}

/* setModNick(bool)
 * 
 * Updates the setting for modnick in the database
 * modnick controls whether services will force a user 
 * to identify as a nickname when it is in use (true)
 * or just require identification before any services
 * are used (false)
 * 
 */

void
  User::setModNick(bool const &val)
{
   String query;
   if(val)
     {
	query = "UPDATE nicks set modnick=1 where nickname='"+nickname+"'";
     }
   else
     {
	query = "UPDATE nicks set modnick=0 where nickname='"+nickname+"'";
     }
   services.getDatabase().query(query);
   
};

/* setDeopAway(bool)
 * 
 * Updates the setting for Deop on Away in the database.
 * 
 * Deop On Away will deop (and voice) the user in any channels
 * they are opped in when the user goes away.
 *
 */

void
  User::setDeopAway(bool const &val)
{
   String query;
   if(val)
     {
	query = "UPDATE nicks set deopaway=1 where nickname='"+nickname+"'";
     }
   else
     {
	query = "UPDATE nicks set deopaway=0 where nickname='"+nickname+"'";
     }
   services.getDatabase().query(query);
};

/* modNick()
 * 
 * Return TRUE if the user has enabled modNick, otherwise FALSE
 * 
 */

bool
  User::modNick(void)
{
   MysqlRes res = services.getDatabase().query("SELECT modnick from nicks where nickname='"+nickname+"'");
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	if(row[0]=="1")
	  {
	     return true;
	  }
	else
	  {
	     return false;
	  }
     }
   return false;
};

/* deopAway(void)
 * 
 * Return TRUE if the user wishing to use the Deop On Away functionality
 * otherwise FALSE.
 * 
 */

bool
  User::deopAway(void)
{
   MysqlRes res = services.getDatabase().query("SELECT deopaway from nicks where nickname='"+nickname+"'");
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	if(row[0]=="1")
	  {
	     return true;
	  }
	else
	  {
	     return false;
	  }
     }
   return false;
};

/* updateHost(String)
 * 
 * Update the database to show the users new host.
 * 
 */

void
  User::updateHost(String const &host)
{
   services.getDatabase().query("UPDATE nicks set lasthost='"+host+"' WHERE nickname='"+nickname+"'");
};


/* getQuitMessage()
 * 
 * Return the last known quit message for this user..
 * 
 */

String
  User::getQuitMessage(void)
{
   MysqlRes res = services.getDatabase().query("SELECT quitmsg from nicks where nickname='" + nickname + "'");
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	return row[0];
     }
   return "";
};

/* getAccess(String)
 * 
 * Return the given access for a user with the given service.
 * 
 */

int 
  User::getAccess(String const &service)
{
   MysqlRes res = services.getDatabase().query("SELECT access from access where nickname='"+nickname+"' AND service='"+service+"'");
   MysqlRow row;
   while ((row = res.fetch_row()))
     {
	return String(row[0]).toInt();
     }
   return 0;
}
/* addCheckIdentify()
 * 
 * Force this client to identify for their nickname..
 * 
 */

void
  User::addCheckIdentify(void)
{
   sendMessage("[Identification Request] This nickname is registered, please identify by typing /msg nick@ircdome.org identify <password>","Nick");
   services.getDatabase().query("INSERT into kills values('','"+nickname+"','"+String::convert(services.currentTime + 120 + (services.random(60)))+"')");
};


