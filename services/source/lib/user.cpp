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

#include "exordium/services.h"
#include "exordium/user.h"
#include "exordium/utils.h"
#include <kineircd/str.h>

using AISutil::String;
using AISutil::StringTokens;
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
  User::sendMessage(String const &message, String const &service)
{
   services.serviceNotice(message,service,nickname);
   return;
}

void
  User::sendMessage(String const &message, String const &service, bool const &privmsg)
{
   if(privmsg)
     {
	services.servicePrivmsg(message,service,nickname);
     }
   else
     {
	services.serviceNoticeForce(message,service,nickname);
     }
return;
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
        services.getDatabase().dbUpdate("nicks", "modnick=1", "nickname='"+nickname+"'");
     }
   else
     {
       services.getDatabase().dbUpdate("nicks", "modnick=0", "nickname='"+nickname+"'");
     }
   return;
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
        services.getDatabase().dbUpdate("nicks", "deopaway=1", "nickname='"+nickname+"'");
     }
   else
     {
        services.getDatabase().dbUpdate("nicks", "deopaway=0", "nickname='"+nickname+"'");
     }
   return;
};

/* modNick()
 *
 * Return TRUE if the user has enabled modNick, otherwise FALSE
 *
 */

bool
  User::modNick(void)
{
   if( services.getDatabase().dbSelect("modnick", "nicks", "nickname='"+nickname+"'") < 1 )
      return false;

   if (services.getDatabase().dbGetValue() == "1")
      return true;
   else
      return false;
};


String
  User::getModes(void)
{
   if( services.getDatabase().dbSelect("modes", "onlineclients", "id="+getOnlineIDString()) < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}


/* deopAway(void)
 *
 * Return TRUE if the user wishing to use the Deop On Away functionality
 * otherwise FALSE.
 *
 */

bool
  User::deopAway(void)
{
   if( services.getDatabase().dbSelect("deopaway", "nicks", "nickname='"+nickname+"'") < 1 )
      return false;

   if( services.getDatabase().dbGetValue() == "1" )
      return true;
   else
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
   services.getDatabase().dbUpdate("nicks", "lasthost='"+host+"'", "nickname='"+nickname+"'");
   return;
};

/* getQuitMessage()
 *
 * Return the last known quit message for this user..
 *
 */

String
  User::getQuitMessage(void)
{
   if( services.getDatabase().dbSelect("quitmsg", "nicks", "nickname='"+nickname+"'") < 1)
     return "None Recorded";
   else
     return services.getDatabase().dbGetValue();
};

/* getAccess(String)
 *
 * Return the given access for a user with the given service.
 *
 */

int
  User::getAccess(String const &service)
{
   if( services.getDatabase().dbSelect("access", "access", "nickname='"+nickname+"' AND service='"+service+"'") < 1 )
     return 0;
   else
     return services.getDatabase().dbGetValue().toInt();
}
/* addCheckIdentify()
 *
 * Force this client to identify for their nickname..
 *
 */


// NOTE! Gotta change the service name for sendMessage
void
  User::addCheckIdentify(void)
{
   sendMessage("[Identification Request] This nickname is registered, please identify by typing /msg nick@peoplechat.org identify <password>","Nick");
   services.getDatabase().dbInsert("kills", "'','"+nickname+"','"+String::convert(services.currentTime + 120 + (Utils::random(60)))+"'");
   return;
};

/* countHost()
 *
 * Return the number of online people from this users hostname.
 *
 */

int
  User::countHost(void)
{
   return services.getDatabase().dbSelect("id", "onlineclients", "hostname='"+getHost()+"'");
};

/* getHost(void)
 *
 * Return the hostname for this user.
 *
 */

String
  User::getHost(void)
{
   if( services.getDatabase().dbSelect("hostname", "onlineclients", "id='"+getOnlineIDString()+"'") < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
};

/*
 * getIdent(void)
 * 
 * Return our users ident (username)
 * 
 */

String
  User::getIdent(void)
{
   if( services.getDatabase().dbSelect("username", "onlineclients", "id='"+getOnlineIDString()+"'") < 1 )
     return "";
   else
     return services.getDatabase().dbGetValue();
}

/*
 * isIdentified(void)
 * 
 * Return bool if we are identified as ANY nickname
 * 
 */
bool
  User::isIdentified(void)
{
   if( services.getDatabase().dbSelect("idas", "nicksidentified", "nick='"+getOnlineIDString()+"'") < 1 )
     return false;

   if( services.getDatabase().dbGetValue().toInt() > 0 )
      return true;
   else
      return false;
}

/* isIdentified(String)
 * 
 * Return true if we are identified as the given nickname.
 * 
 */

bool
  User::isIdentified(String const &as)
{
   if( services.getDatabase().dbSelect("idas", "nicksidentified", "nick='"+getOnlineIDString()+"'") < 1 )
     return false;

   String temp = services.getDatabase().dbGetValue();
   String idnick = services.getNick(temp.toInt());

   if( idnick.IRCtoLower() == as.IRCtoLower() )
     return true;
   else
     return false;
}

					      
/* getIDList(void)
 * 
 * Return a list of nicknames we are identified as.
 * 
 */

String
  User::getIDList(void)
{
   String thelist = "";
   String idnick;

   int nbRes = services.getDatabase().dbSelect("idas", "nicksidentified", "nick='"+getOnlineIDString()+"'");

   for(int i=0; i<nbRes; i++)
   {
     idnick = services.getNick(services.getDatabase().dbGetValue().toInt());
     thelist += " ";
     thelist += idnick;
     services.getDatabase().dbGetRow();
   }
   return thelist;
}


/* isPending(void)
 * 
 * Return TRUE if we are still in a pending state for our nick registration
 * 
 */

bool
  User::isPending(void)
{
   if( services.getDatabase().dbSelect("id", "nickspending", "nickname='"+nickname+"'") < 1 )
     return false;

   if ( services.getDatabase().dbGetValue().toInt() > 0 )
     return true;
   else
     return false;
}


/* setLanguage(String)
 * 
 * Update language setting for us.
 * 
 */
void User::setLanguage(const String& lang)
{
   if(isRegistered())
   services.getDatabase().dbUpdate("nicks", "lang='"+lang+"'",
				   "nickname='" + nickname + "'");
}


/* getLanguage()
 *
 * return users language
 */
String User::getLanguage(void)
{
   if (services.getDatabase().dbSelect("lang", "nicks", 
				       "nickname='" + nickname + "'") > 0) {
      return services.getDatabase().dbGetValue();
   }
   
   return "en";
}


/*
 * getPass(String)
 *
 * Return the password for the given nickname
 *
 */
const String User::getPass(void)
{
   if (services.getDatabase().dbSelect("password", "nicks", 
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
   
   return String(services.getDatabase().dbGetValue().data());
}


/* getURL(String)
 *
 * Find and return the URL for the given nickname.
 *
 */
const String User::getURL(void)
{
   if (services.getDatabase().dbSelect("url", "nicks", 
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
   
   return services.getDatabase().dbGetValue();
}


/* getMSN(String)
 *
 * Find and return the MSN for a nick.
 *
 */
const String User::getMSN(void)
{
   if (services.getDatabase().dbSelect("msn", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
   
   return services.getDatabase().dbGetValue();
}


/* getYAHOO(String)
 *
 * Find and return the YAHOO! for a nick.
 *
 */
const String User::getYAHOO(void)
{
   if (services.getDatabase().dbSelect("yahoo", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
   
   return services.getDatabase().dbGetValue();
}


/* getAIM(String)
 *
 * Return the AIM setting for a nickname.
 *
 */
const String User::getAIM(void)
{
   if (services.getDatabase().dbSelect("aim", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
   
   return services.getDatabase().dbGetValue();
}


/* getICQ()
 *
 * Return the ICQ for a nick.
 *
 */
const String User::getICQ(void)
{
   if (services.getDatabase().dbSelect("icq", "nicks", 
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
    
   return services.getDatabase().dbGetValue();
}


/* getEmail
 *
 * Retrieve the nicknames email
 *
 */
const String User::getEmail(void)
{
   if (services.getDatabase().dbSelect("email", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
    
   return services.getDatabase().dbGetValue();
}

/*
 * getRegDate
 * 
 * Return the registration date of this user
 * 
 */

const String User::getRegDate(void)
{
   if (services.getDatabase().dbSelect("registered", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
    
   return services.getDatabase().dbGetValue();
}

/* 
 * getLastID
 * 
 * Return the date this user last identified
 * 
 */

const String User::getLastID(void)
{
   if (services.getDatabase().dbSelect("lastid", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
    
   return services.getDatabase().dbGetValue();
}

/*
 * getLastHost
 * 
 * The hostname this user last identified from
 * 
 */

const String User::getLastHost(void)
{
   if (services.getDatabase().dbSelect("lasthost", "nicks",
				       "nickname='" + nickname + "'") < 1) {
      return "";
   }
   
   return services.getDatabase().dbGetValue();
}
/*
 * getPendingCode
 * 
 * (If any) return the pending code for this user
 * 
 */

const String User::getPendingCode(void)
{
   if (services.getDatabase().dbSelect("auth","nickspending","nickname='"+nickname+"'") < 1)
	return "";
   else
        return services.getDatabase().dbGetValue();
}
/*
 * clearPendingCode
 * 
 * (If any) clears the pending code for this user
 * 
 */
const void User::clearPendingCode(void)
{
services.getDatabase().dbDelete("nickspending","nickname='"+nickname+"'");
}

/*
 * registerNick
 * 
 * Registers this nickname (if not already)
 * 
 */

const void User::registerNick(String const &password,String const &email)
{
if(!isRegistered())
   services.getDatabase().dbInsert("nicks","'','"+nickname.IRCtoLower()+"','"+Utils::generatePassword(nickname.IRCtoLower(),password)+"','"+email+"',NOW(),NOW(),'',0,'english','0','None','None','0','None Set','None Set','None recorded',1");
}

/*
 * isRegistered
 * 
 * Return true/false depending on registration status
 * 
 */
const bool User::isRegistered(void)
{
if (services.getDatabase().dbSelect("id", "nicks", "nickname='"+nickname+"'") < 1)
	return false;
else
{
	if (services.getDatabase().dbGetValue().toInt() > 0)
		return true;
	else
		return false;
}
}
/*
 * log
 * 
 * Log the given data as being done by this user
 * 
 * Two types of functions for log
 */

const void User::log(String const &service,String const &log)
{
services.getDatabase().dbInsert("log","'','"+getIDList()+"','"+getIdent()+"','"+getHost()+"','"+service+"',NOW(),'"+log+"',''");
}

// the other log!! here plz :(

/*
 * genAuth
 * 
 * Generate an authorisation code for this user
 * 
 */
const String User::genAuth(void)
{
String authcode = Utils::generateRegcode(nickname,"VIVA LA FRANCE :)");
services.getDatabase().dbInsert("nickspending","'','"+nickname+"','"+authcode+"'");
return authcode;
}

/*
 * setPassword
 * 
 * Update this users password with the given one
 * 
 */
const void User::setPassword(String const &newpass)
{
   services.getDatabase().dbUpdate("nicks","password'="+newpass+"'","nickname='"+nickname+"'");
         
}

