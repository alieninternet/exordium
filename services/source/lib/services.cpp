/* $Id$
 *
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "services.h"
#include "exordium/channel.h"
#include "exordium/user.h"
#include "exordium/utils.h"
#include "core_language.h"
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <fstream>
#include "exordium/service.h"
#include <kineircd/kineircdconf.h>

extern "C"
{
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif
};


using AISutil::String;
using AISutil::StringTokens;
using Kine::Password;
using AISutil::StringMask;
using namespace Exordium;


ServicesInternal::ServicesInternal(ConfigInternal& c, CDatabase& db)
  : Services(db),
console(*this),
config(c),
//startTime(time(NULL)),
lastPing(time(NULL)),
stopTime(0),
lastCheckPoint(time(NULL)),
lastExpireRun(time(NULL)),
stopping(false)
{
   database.dbConnect();

   srand(time(NULL));
   logLine ("Cleaning out (any) stale entries from the DB");
   database.dbDelete("onlineclients");
   database.dbDelete("chanstatus");
   database.dbDelete("nicksidentified");
   database.dbDelete("kills");
   database.dbDelete("onlineservers");
   database.dbDelete("onlinechan");
   database.dbDelete("onlineopers");
   
   // Start all the modules
   config.getModules().startAll(*this);
   
   // Is the console actually wanted?
//   if (config.getConsoleEnabled()) {
//	registerService(config.getConsoleName(), "peoplechat", /* Hack for now */
//			config.getConsoleHostname(),
//			config.getConsoleDescription());
//   }
   Kine::langs().registerMap(Language::tagMap);
}


ServicesInternal::~ServicesInternal(void)
{
   logLine("Exordium is shutting down " /*+ reason*/,
	   Log::Warning);

   // Stop and unload all the modules..
   config.getModules().unloadAll(/*reason*/);

   // I do not like this.. oh well..  - pickle
   if (config.getConsoleEnabled())
     {
//	queueAdd(config.getConsoleName()+" QUIT :"+reason);
     }

   // Nasty hack for now.
//   String tofo = "\002Services shutting down\002 : " + reason;
//   queueAdd(":" + Kine::config().getOptionsServerName() +
//	    " SQUIT " + Kine::config().getOptionsServerName() + " :" +
//	    reason);

   stopping = true;
   stopTime = Kine::daemon().getTime().seconds + 15;

   // Clean up before we die
   database.dbDelete("onlineclients");
   database.dbDelete("chanstatus");
   database.dbDelete("nicksidentified");
   database.dbDelete("kills");
   database.dbDelete("onlineservers");
   database.dbDelete("onlinechan");
   database.dbDelete("onlineopers");
}

/* SynchTime()
 *
 * Perform various tasks that need doing on an ongoing basis
 * those being;
 * 	Expire any channel bans that need expiring
 *	Update ServerWide Time.
 */

void ServicesInternal::SynchTime(void)
{
   //Undo any expired glines

   //Undo any expired channel bans
   //time_t ctime = currentTime;
   //int nbRes = database.dbSelect("id,chan,mask", "chanbans", "expireon<"+ctime);
   //for(int i=0; i<nbRes; i++)
   //  {
   //	channel.RemoveBan(database.dbGetValue(0), database.dbGetValue(1), database.dbGetValue(2));
   //	database.dbGetRow();
   //   }
   //
}


/* doHelp(User,String,String,String)
 *
 * Generate a help page from our dynamic help system
 *
 */
void
  ServicesInternal::doHelp(User& origin, String const &service,
			   String const &topic, String const &parm)
{
   if(topic == "")
     {
	//No topic, no parm.
	int nbRes = database.dbSelect("txt", "help",
				      "service='"+service+"' AND word='' AND parm='' AND lang='"+origin.getLanguage()+"' ORDER by id");

	String line="";

	for(int i=0; i<nbRes; i++)
	  {
	     line = parseHelp(database.dbGetValue());
	     serviceNotice(line,service,origin.getNickname());
	     database.dbGetRow();
	  }
	return;
     }
   // End
   if(parm == "")
     {
	//No topic, no parm.

	int nbRes = database.dbSelect("txt", "help",
				      "service='"+service+"' AND word='"+topic+"' AND parm='' AND lang='"+origin.getLanguage()+"' ORDER by id");

	String line="";

	for(int i=0; i<nbRes; i++)
	  {
	     line = parseHelp(database.dbGetValue());
	     serviceNotice(line,service,origin.getNickname());
	     database.dbGetRow();
	  }

	return;
     }
   // End
   //
   int nbRes = database.dbSelect("txt", "help",
				 "service='"+service+"' AND word='"+topic+"' AND parm='"+parm+"' AND lang='"+origin.getLanguage()+"' ORDER by id");

   String line="";

   for(int i=0; i<nbRes; i++)
     {
	line = parseHelp(database.dbGetValue());
	serviceNotice(line,service,origin.getNickname());
	database.dbGetRow();
     }
   return;
}

/* sendEmail(String,String,String)
 *
 * post an email into the database, which is polled later by a third party
 * utilite to send any pending emails
 *
 */

void
  ServicesInternal::sendEmail (String const &to, String const &subject, String const &text)
{
   database.dbInsert("emails", "'','"+to+"','"+subject+"','"+text+"'");
}

/* parseHelp(In)
 *
 * This parses our special codes in the help files
 * allowing for items such as bold, italics etc
 *
 */

String
  ServicesInternal::parseHelp (String const &instr)
{
   String retstr;
   for (unsigned int i = 0; i != instr.length(); i++)
     {
	if (instr[i] == '%' && ((i != 0) && (instr[i - 1] != '\\')))
	  {
	     switch (instr[++i])
	       {
		case 'u':
		  retstr += '\037';
		  break;
		case 'r':
		  retstr += '\026';
		  break;
		case 'b':
		  retstr += '\002';
		  break;
		default:
		  retstr += instr[i];
	       }
	  }
	else
	  {
	     retstr += instr[i];
	  }
     }
   return retstr;
}

/* log(String,String,String)
 *
 * Logs the given information into the database
 * which can later be accessed by staff and/or
 * the web interface
 *
 * There are two versions, one for normal logs, and the other
 * for channel based access.
 */

void
  ServicesInternal::log (User& origin, String const &service, String const &text, String const &cname)
{
   //	  String thenick = origin.IRCtoLower();
   //	  User *ptr = findUser(thenick);
   String nicks = origin.getIDList();
   String ident = origin.getIdent();
   String host = origin.getHost();
   database.dbDelayedInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"','"+cname+"'");
}

void
  ServicesInternal::log (User& origin, String const &service, String const &text)
{
   //	  String thenick = nick.IRCtoLower();
   //	  User *ptr = findUser(thenick);
   String nicks = origin.getIDList();
   String ident = origin.getIdent();
   String host = origin.getHost();
   database.dbDelayedInsert("log", "'','"+nicks+"','"+ident+"','"+host+"','"+service+"',NOW(),'"+text+"',''");
}

/* Overload kine's trim to add some stuff of our own! :-) */
String String::trim(void) const
{
   size_type s = 0;
   size_type e = length();

   while ((s < e) &&
	  ((c_str()[s] == ' ') ||
	   (c_str()[s] == '\t') ||
	   (c_str()[s] == '\r') ||
	   (c_str()[s] == ':') ||
	   (c_str()[s] == '@') ||
	   (c_str()[s] == '+') ||
	   (c_str()[s] == '\n')))
     {
	s++;
     }

   while ((e > s) &&
	  ((c_str()[e - 1] == ' ') ||
	   (c_str()[e - 1] == '\t') ||
	   (c_str()[e - 1] == '\r') ||
	   (c_str()[e - 1] == ':') ||
	   (c_str()[e - 1] == '@') ||
	   (c_str()[e - 1] == '+') ||
	   (c_str()[e - 1] == '\n')))
     {
	e--;
     }

   return substr(s, e - s);
}


   /* usePrivmsg(nick)
    *
    * Figure out whether we should use the privmsg or
    * the notice interface to talk to a client.
    */

bool
  ServicesInternal::usePrivmsg (String const &nick)
{
   if(!isNickRegistered(nick))
     {
	return false;
     }

   if( database.dbSelect("privmsg", "nicks", "nickname='"+nick+"'") < 1 )
     return false;

   if( database.dbGetValue() == "1")
     return true;
   else
     return false;
}

bool
  ServicesInternal::isOp(const Kine::ClientName &nick,
			 const Kine::ChannelName &chan)
{
   int chanid = channel.getOnlineChanID(chan);
   int nickid = locateID(nick);
   if( database.dbSelect("status", "chanstatus", "chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'") < 1)
     return false;

   if( database.dbGetValue().toInt() == 2 )
     return true;
   else
     return false;
}

bool
  ServicesInternal::isVoice(String const &nick, String const &chan)
{
   int chanid = channel.getOnlineChanID(chan);
   int nickid = locateID(nick);
   if( database.dbSelect("status", "chanstatus", "chanid='" + String::convert(chanid)+"' AND nickid='" + String::convert(nickid)+"'") < 1 )
     return false;

   if( database.dbGetValue().toInt() == 1 )
     return true;
   else
     return false;
}

void
  ServicesInternal::sendNote(Kine::ClientName const &from,
			     Kine::ClientName const &to, String const &text)
{
   Kine::ClientName thenick = to.IRCtoLower();
   database.dbInsert("notes", "'','"+from+"','"+to+"',NOW(),'"+text+"'");
   int foo = locateID(thenick);
   if(foo>0)
     {
	User *ptr = findUser(thenick);
	if(ptr==0)
	  return;
	//Client is online.. But are they identified HUHUHUH?!!?
	if(ptr->isIdentified(to))
	  {
	     String togo = String("\002[\002New Note\002]\002 From \002")+from+"\002";
	     serviceNotice(togo,"Note",to);
	  }
     }

}

// TODO: check why the 2 lines are commented
void ServicesInternal::checkpoint(void)
{
   /* Decrease network wide flood counters */
   user_map::iterator it;
   for (it = users.begin(); it != users.end(); it++)
     {
	String tmp = (*it).first;
	if(((*it).second)!=0)
	  (*it).second->decFloodCount();
     }

   //Any nick mods to be done? :-)

   int nbRes = database.dbSelect("kills");

   for(int i=0; i<nbRes; i++)
     {
	String id = database.dbGetValue(0);
	String killt = database.dbGetValue(2);
	int nowt = Kine::daemon().getTime().seconds;
	Kine::ClientName tomod = database.dbGetValue(1);
	database.dbGetRow();

	if(killt.toInt() < nowt)
	  {
	     database.dbDelete("kills", "id='"+id+"'");
	     int foo = 0;
	     bool running = true;
	     Kine::ClientName newnick = "";
	     while(running)
	       {
		  foo++;
		  newnick = tomod+String::convert(foo);
		  if(!isNickRegistered(newnick))
		    {
		       if(locateID(newnick)==0)
			 {
			    //Oke we can use this one :)
			    running = false;
			 }
		    }

	       }
	     User *ptr = findUser(tomod);
	     if(ptr==0)
	       {
#ifdef DEBUG
		  logLine("I could not find the users record.. hence i"
			  " can't change their nick " + tomod,
			  Log::Debug);
#endif
		  return;
	       }

	     String msg = "Non-Identification: Your nickname is now being changed";
	     serviceNotice(msg,"Nick",tomod);
//	     queueAdd(":" + Kine::config().getOptionsServerName() +
//		      " SVSNICK " + tomod + " " + newnick + " :0");
	     //		    setNick(*ptr,newnick);
	     //		    database.query("UPDATE onlineclients set nickname='"+newnick+"' WHERE nickname='"+tomod+"'");
	  }
	if((killt.toInt()-nowt)<60)
	  {
	     if((killt.toInt()-nowt)>50)
	       {
		  String msg = "\002[\002Identification Warning\002]\002 Less than 60 seconds left to identify";
		  serviceNotice(msg,"Nick",tomod);
	       }
	  }
     }

}

/* addServer(String,int,String)
 * 
 * Add a server to our server map
 * 
 */
Server* const ServicesInternal::addServer(const String &name, const int &hops, const String &desc)
{
   return servers[name] = new Server(name,hops,desc,*this);
}

/* addChan(String,Int)
 *
 * Add a channel to our channel map
 *
 */
dChan* const ServicesInternal::addChan(const Kine::ChannelName& name, const int oid)
{
   return chans[name] = new dChan(name.IRCtoLower(),oid,*this);
}

/* addUser(String,Int)
 *
 * Add the user to our users map
 *
 */
User* const ServicesInternal::addUser(const String& name, const int oid)
{
   return users[name] = new User(name,oid,*this);
}

/* findUser(String)
 *
 * Find and return a pointer to a user.
 *
 */
User* ServicesInternal::findUser(Kine::ClientName &name)
{
   User *ptr = users[name.IRCtoLower().trim()];
   if(ptr == 0)
     {
	return 0;
     }
   return ptr;
}

/* findChan(String)
 *
 * Find and return a pointer to that channel.
 *
 */
dChan* ServicesInternal::findChan(Kine::ChannelName &name)
{
   dChan *ptr = chans[name.IRCtoLower().trim()];
   if(ptr==0)
     {
	return 0;
     }
   return ptr;
}

/* findServer(String)
 * 
 * Find and return a pointer to that server.
 * 
 */
Server* ServicesInternal::findServer(String &name)
{
   Server *ptr = servers[name];
   if(ptr==0)
     {
	return 0;
     }
   return ptr;
}

/* delUser(String)
 *
 * Find.. and delete the given user.
 *
 */
bool ServicesInternal::delUser(Kine::ClientName &name)
{
   users.erase(name.IRCtoLower());

   database.dbDelete("onlineclients", "nickname='"+name+"'");

   return true;
};

/* delChan(String)
 *
 * Delete the given channel.
 *
 */
bool ServicesInternal::delChan(Kine::ChannelName &name)
{
   chans.erase(name.IRCtoLower());
   database.dbDelete("onlinechan","name='"+name.IRCtoLower()+"'");
   return true;
}

/* delServer(String)
 * 
 * Delete the given server.
 * 
 */
bool ServicesInternal::delServer(String &name)
{
   servers.erase(name);
   return true;

}

/* setNick(User,String)
 *
 * Find and update the given user record as having
 * a new nickname.
 *
 */
void ServicesInternal::setNick(User &who, Kine::ClientName &newnick)
{
#ifdef DEBUG
   logLine("setNick: " + newnick, Log::Debug);
#endif
   String fixedNewNick = newnick.IRCtoLower();
   user_map::iterator user = users.find(who.getNickname().IRCtoLower());
   users.erase(user);
   users[fixedNewNick] = &who;
   who.setNick(fixedNewNick);

   // Update nickname in table onlineclients
   database.dbUpdate("onlineclients", "nickname='"+fixedNewNick+"'", "id="+who.getOnlineIDString());
};

/* addClient(...)
 *
 * Add a user and setup a new map entry for them...
 * ..
 */

User* ServicesInternal::addClient(Kine::ClientName const &nick, String const &hops,
				  String const &timestamp,
				  String const &username, String const &host,
				  String const &server,
				  String const &modes, String const &realname)
{
   database.dbInsert("onlineclients", "'','"+nick.toLower()+"','"
		     +hops + "','" + timestamp + "','" + username + "','"
		     +host + "','"+host+"','" + server + "','"
		     +modes+ "','" + realname + "'");
   int foo = locateID(nick);
   String client = nick.IRCtoLower();
   User *ptr = addUser(client,foo);
   return ptr;
};

/* locateID....
 *
 * shouldn't be here.. really .. just a stop gap for now
 *
 */
int ServicesInternal::locateID(Kine::ClientName const &nick)
{

   // Saftey Check - Remove any special chars.
   String newnick = nick.IRCtoLower();

   if( database.dbSelect("id", "onlineclients", "nickname='"+newnick+"'") < 1 )
     {
#ifdef DEBUG
	logLine("ERROR: Returning 0 (NOT KNOWN) from getOnlineNickID",
		Log::Debug);
#endif
	return 0;
     }
   else
     return database.dbGetValue().toInt();
}

/* getRequiredAccess(String,String)
 *
 * Find ( and return ) the required access to run a command on
 * with the given service.
 *
 */

int ServicesInternal::getRequiredAccess(String const &service,
					String const &command)
{
   if( database.dbSelect("access", "commands", "service='"+service+"' AND command='"+command+"'") < 1 )

     /* If no match return 999 - to ensure no-one can use the command
      * since its obvious a command entry hasn't been set up for it */
     return 999;

   else
     return database.dbGetValue().toInt();
};

/* isNickRegistered(String)
 *
 * Return TRUE if the given nickname is registered, otherwise false
 *
 */

bool ServicesInternal::isNickRegistered(String const &nick)
{
   if( database.dbSelect("id", "nicks", "nickname='"+nick+"'") < 1 )
     return false;
   else
     {
	if( database.dbGetValue().toInt() > 0 )
	  return true;
	else
	  return false;
     }
}

/*
 * updateLastID(String)
 *
 * Updates the last identified time of a client
 * to now.
 *
 */

void ServicesInternal::updateLastID(String const &nick)
{
   database.dbUpdate("nicks", "lastid=NOW()", "nickname='"+nick+"'");
}

/*
 * getNick(int)
 *
 * Return the nickname for the given registered nickname ID
 *
 */
String ServicesInternal::getNick(int const &id)
{
   if( database.dbSelect("nickname", "nicks", "id='"+String::convert(id)+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/*
 * getOnlineNick(int)
 *
 * Same as above except for Online ID's
 *
 */
String ServicesInternal::getOnlineNick(int const &id)
{
   if(id==0)
     return "";
   if( database.dbSelect("nickname", "onlineclients", "id='"+String::convert(id)+"'") < 1 )
     return "";
   else
     return database.dbGetValue();
}

/*
 * getOnlineChan(int)
 *
 * Return the unique ID for any given channel
 *
 */
int ServicesInternal::getOnlineChanID(String const &id)
{
   if( database.dbSelect("id", "onlinechan", "name='"+id+"'") < 1 )
     return 0;
   else
     return database.dbGetValue().toInt();
}

void ServicesInternal::addOper(String const &nick, int access)
{
   database.dbInsert("onlineopers", "''," + String::convert(gstatic.getRegisteredNickID(nick)) + "," +String::convert(access));
}

void ServicesInternal::delOper(String const &nick)
{
   database.dbDelete("onlineopers", "nickid="+String::convert(gstatic.getRegisteredNickID(nick)));
}

bool ServicesInternal::isOper(String const &nick)
{
   if( database.dbSelect("id", "onlineopers", "nickid="+ String::convert(gstatic.getRegisteredNickID(nick))) < 1 )
     return false;
   else
     return true;
}

void ServicesInternal::validateOper(Kine::ClientName &origin)
{
   //Active Oper? (hah :-)
   User *ptr = findUser(origin);
   int axs = ptr->getAccess("Oper");

   if(axs==0)
     {
	//Non-Authorised.
	String tosend = origin+" just tried to become an IRC Operator - \002No Access\002";
	logLine(tosend, Log::Warning);
	String reason = "You have no permission to become an IRC Operator";
	/* We have a problem here - for some reason the parser is trying
	 * to validate every online user - this is what happened when I
	 * brought Oper online today
	 *
	 * >09:32:41< *** Quits: Praetorian (Killed (Oper ()))
	 * >09:32:41< *** Quits: Luster (Killed (Oper ()))
	 * >09:32:41< *** Quits: allan1 (Killed (Oper ()))
	 *
	 * None of these users had usermode +o :-)
	 */
	killnick(origin, "Oper", reason);
	return;
     }
   if(axs==-1)
     {
	String tosend = origin+" just tried to become an IRC Operator - \002Suspended\002";
	logLine(tosend, Log::Warning);
	String reason = "You are suspended - Do not try to become an Operator";
	killnick(origin, "Oper", reason);
	return;
     }
   if(axs>0)
     {
	String tosend = origin+" just became an IRC Operator - level "+String::convert(axs);
	sendGOper("Oper",tosend);

	if (!isOper(origin))
	  addOper(origin, axs);
#ifdef DEBUG
	else
	  {
	     logLine("Warning: inconsistency in ValidateOper: "
		     "new oper already in onlineopers!",
		     Log::Debug);
	  }
#endif

	return;
     }

}
