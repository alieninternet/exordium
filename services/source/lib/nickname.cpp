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

#include "exordium/nickname.h"
#include <kineircd/str.h>
#include "exordium/services.h"
#include "exordium/sql.h"
#include "exordium/utils.h"


using namespace Exordium;
using Kine::String;
using Kine::StringMask;
using Kine::Password;
namespace Exordium
{

   String
     Nickname::stripModes(String const &nick)
       {
	  char *temp = new char[nick.length() + 1];

	  for (register unsigned int i = (nick.length() + 1); i--;)
	    {

	       char ch = nick.c_str()[i];
	       switch(ch)
		 {

		  case '@': /* Opped.. */
		    temp[i] = ' ';
		    continue;
		  case '+': /* Voiced */
		    temp[i] = ' ';
		    continue;
		  default:
		    temp[i] = ch;
		 }

	    }

	  String result(temp);
	  delete(temp);
	  return result.trim();

       }


   String
     Nickname::getQuitMessage(String const &nick)
       {
	  String query = "SELECT quitmsg from nicks where nickname='"+nick+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       return foo;
	    }
	  return "";
       }

   String
     Nickname::getRegNickCount(void)
       {
	  String query = "SELECT count(*) from nicks";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       return foo;
	    }
	  res.free_result();
	  return String("0");
       }

   String
     Nickname::getOnlineCount(void)
       {
	  String query = "SELECT count(*) from onlineclients";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       return foo;
	    }
	  res.free_result();

	  return String("0");
       }

   int
     Nickname::getAccess(String const &service, String const &who)
       {
	  String query = "SELECT access from access where nickname='" + who + "' and service='" + service + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String axs = ((std::string) row[0]);
	       res.free_result();
	       return axs.toInt();
	    }
	  res.free_result();
	  return 0;
       }

   String
     Nickname::generatePassword(String const &nickname, String const &password)
       {
	  String bob((char *)Kine::Password::makePassword(nickname,password).s_char,(String::size_type)20);
	  std::cout << "Pass length is " << bob.length() << std::endl;
	  std::cout << "Its: " << bob << std::endl;
	  return bob;

       }

   bool
     Nickname::isAuthorised(String const &server)
       {
	  String query = "SELECT id from serverlist where name='" + server + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       if((int)row[0]>0)
		 {
		    res.free_result();
		    return true;
		 }
	    }
	  res.free_result();
	  return false;
       }

   void
     Nickname::addCheckidentify(String const &nick)
       {
	  services.serviceNotice("\002[\002Identification Request\002]\002 This nickname is Registered. Please identify","Nick",nick);
	  int rfac = services.random(60);
	  String query = "INSERT into kills values('','"+nick+"','" + String::convert(services.currentTime + 120 + rfac) + "')";
	  services.getDatabase().query(query);

       }

   void
     Nickname::addClient (String nick, String hops, String timestamp, String username,
			  String host, String vwhost, String server, String modes,
			  String realname)
       {
	  services.getDatabase().query (String ("insert into onlineclients values('','") + nick.toLower() +String ("','") + hops + String ("','") + timestamp +String ("','") + username + String ("','") + host + String ("','") + vwhost + String ("','") + server + String ("','") + modes + String ("','") + services.getDatabase().makeSafe(realname) +String ("')"));
	  //Now retrieve that id that has just been given.....
	  int foo = getOnlineNickID(nick);
	  User *ptr = services.addUser(nick,foo);
       }

   int
     Nickname::countHost(String const &hostname)
       {
	  String query = "select id from onlineclients where hostname='"+hostname+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  int num = res.num_fields();
	  res.free_result();
	  return num;
       }

   int
     Nickname::getRequiredAccess(String const &service, String const &command)
       {
	  String query = "SELECT access from commands where service='"+ service + "' AND command='" + command + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String access = ((std::string) row[0]);
	       res.free_result();
	       return access.toInt();
	    }
	  res.free_result();
	  //If no entry.... return >max
	  return 999;
       }

   bool
     Nickname::isNickRegistered (String const &nick)
       {
	  String query = String("select id from nicks where nickname='") + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       if((int)row[0]>0)
		 {
		    res.free_result();
		    return true;
		 }
	    }
	  res.free_result();
	  return false;
       }

   String
     Nickname::getPass (String const &nick)
       {
	  String query = "select password from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String password((char *)row[0],(String::size_type)20);
	       res.free_result();
	       return password;
	    }
	  res.free_result();
	  return String("");
       }

   int
     Nickname::getOnlineNickID(String const &nick)
       {
          // Saftey Check - Remove any special chars.
	  String newnick = stripModes(nick.toLower());
	   
	  String query = "select id from onlineclients where nickname='" + newnick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String id = ((std::string) row[0]);
	       res.free_result();
	       return id.toInt();
	    }
	  res.free_result();
	  services.Debug("ERROR: Returning 0 (NOT KNOWN) from getOnlineNickID for nickname "+newnick.toLower());
	  return 0;
       }

   int
     Nickname::getRegisteredNickID(String const &nick)
       {
	  String query = "select id from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String id = ((std::string) row[0]);
	       res.free_result();
	       return id.toInt();
	    }
	  res.free_result();
	  return 0;
       }

   void
     Nickname::modeIdentify (String const &nick)
       {
	  String tosend = String(":services.ircdome.org SVSMODE ")+nick+" +r";
	  services.queueAdd(String(tosend));
	  return;
       }

   void
     Nickname::updateLastID (String const &nick)
       {
	  String query = "UPDATE nicks set lastid=NOW() where nickname='" + nick + "'";
	  services.getDatabase().query(query);
	  return;
       }

   String
     Nickname::getHost(String const &nick)
       {
	  String query = "SELECT hostname from onlineclients where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String host = ((std::string) row[0]);
	       res.free_result();
	       return host;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getIdent(String const &nick)
       {
	  String query = "SELECT username from onlineclients where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String ident = ((std::string) row[0]);
	       res.free_result();
	       return ident;
	    }
	  res.free_result();
	  return String("");
       }

   bool
     Nickname::isIdentified(String const &nick)
       {
	  int onlineID = getOnlineNickID(nick);
	  String query = "SELECT idas from identified where nick='"+String::convert(onlineID)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       if(temp.toInt() > 0)
		 return true;
	    }
	  res.free_result();
	  return false;
       }

   bool
     Nickname::isIdentified (String const &nick, String const &as)
       {
	  int onlineID = getOnlineNickID(nick);
	  String query = "SELECT idas from identified where nick='"+String::convert(onlineID)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       String idnick = getNick(temp.toInt());
	       res.free_result();
	       if(idnick.toLower()==as.toLower())
		 {
		    return true;
		 }

	    }
	  res.free_result();
	  return false;
       }

   String
     Nickname::getNick(int const &id)
       {
	  String query = "SELECT nickname from nicks where id='"+String::convert(id)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       return foo;
	    }
	  res.free_result();
	  return String("");
       };

   String
     Nickname::getOnlineNick(int const &id)
       {
	  String query = "SELECT nickname from onlineclients where id='"+String::convert(id)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String foo = ((std::string) row[0]);
	       res.free_result();
	       return foo;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getIDList(String const &nick)
       {
	  int onlineID = getOnlineNickID(nick);
	  String thelist = "";
	  String query = "SELECT idas from identified where nick='"+String::convert(onlineID)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String rval = ((std::string) row[0]);
	       String idnick = getNick(rval.toInt());
	       thelist = String(thelist)+" "+String(idnick);

	    }
	  res.free_result();
	  return thelist;
       }

   String
     Nickname::getpendingCode(String const &nick)
       {
	  String query = "SELECT auth from pendingnicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String ident = ((std::string) row[0]);
	       res.free_result();
	       return ident;
	    }
	  res.free_result();
	  return String("");
       }

   bool
     Nickname::isPending(String const &nick)
       {
	  String query = "SELECT id from pendingnicks where nickname='"+nick+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       if((int)row[0]>0)
		 {
		    res.free_result();
		    return true;
		 }
	    }
	  res.free_result();
	  return false;
       }

   void
     Nickname::registerNick(String const &nick, String const &password, String const &email)
       {
	  String gpass = generatePassword(nick.toLower(),password);
	  String query = "INSERT into nicks values('','" + nick + "','" + gpass + "','" + email + "',NOW(),NOW(),'',0,'english','0','No MSN Set','http://ircdome.org',0,'None Set','None Set','No Quit Message Recorded',1)";
	  services.getDatabase().query(query);
       }

   String
     Nickname::genAuth(String const &nickname)
       {
	  String authcode = Kine::Utils::SHA1::digestToStr(Kine::Password::
							   makePassword("VIVA LA FRANCE :)",nickname),Services::PasswordStrBase,Services::PasswordStrBaseLongPad);

	  String query = "INSERT into pendingnicks values ('','"+nickname+"','" +authcode+"')";
	  services.getDatabase().query(query);
	  String tosend = "\002[\002New Registration\002]\002 Nickname "+nickname+" -> "+authcode;
	  services.Debug(tosend);
	  return authcode;
       }

   void
     Nickname::setLanguage (String const &nick, String const &lang)
       {
	  String query = "UPDATE nicks set lang='" + lang + "' WHERE nickname='" + nick + "'";
	  services.getDatabase().query(query);

       }

   String
     Nickname::getURL(String const &nick)
       {
	  String query = "SELECT url from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getMSN(String const &nick)
       {
	  String query = "SELECT msn from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;

	    }
	  return String("");
       }

   String
     Nickname::getYAHOO(String const &nick)
       {
	  String query = "SELECT yahoo from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getAIM(String const &nick)
       {
	  String query = "SELECT aim from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getICQ(String const &nick)
       {
	  String query = "SELECT icq from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getLanguage(String const &nick)
       {
	  if(!isNickRegistered(nick))
	    {
	       return String("english");
	    }
	  String query = "SELECT lang from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String lang = ((std::string) row[0]);
	       res.free_result();
	       return lang;
	    }
	  res.free_result();
	  return String("english");
       }

   String
     Nickname::getEmail(String const &nick)
       {
	  String query = "SELECT email from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getRegDate(String const &nick)
       {
	  String query = "SELECT registered from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getLastID(String const &nick)
       {
	  String query = "SELECT lastid from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

   String
     Nickname::getLastHost(String const &nick)
       {
	  String query = "SELECT lasthost from nicks where nickname='" + nick + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String temp = ((std::string) row[0]);
	       res.free_result();
	       return temp;
	    }
	  res.free_result();
	  return String("");
       }

}

