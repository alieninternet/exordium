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

#include "chan.h"
#include "tables.h"
#include <exordium/channel.h>
#include <kineircd/str.h>
#include <kineircd/config.h>
#include <exordium/dchan.h>
#include <exordium/database/base.h>
#include <exordium/database/database.h>

#include "language.h"

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::ChanModule;

struct Module::functionTableStruct const
  Module::functionTable[] =
{
     {"access", &Module::parseACCESS},
     {"ban", &Module::parseBAN},
     {"kick", &Module::parseKICK},
     {"op", &Module::parseOP},
     {"voice", &Module::parseVOICE},
     {"register", &Module::parseREGISTER},
     {"help", &Module::parseHELP},
     {"deop", &Module::parseDEOP},
     {"devoice", &Module::parseDEVOICE},
     {"topic", &Module::parseTOPIC},
     {"adduser", &Module::parseADDUSER},
     {"info", &Module::parseINFO},
     {"listban", &Module::parseLISTBAN},
     {"set", &Module::parseSET},
     {"seen", &Module::parseSEEN},
     {"commands", &Module::parseCOMMANDS},
     {0, 0}
};

void
  Module::parseLine (StringTokens& line, User& origin
		     , const String& chan)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   String ch = chan;
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st, ch);
	     return;
	  }
     }

   return;
}

void
  Module::parseLine (StringTokens& line, User& origin, const bool safe)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   String ch = "";
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st, ch);
	     return;
	  }
     }
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND,command),getName());
}

CHAN_FUNC (Module::parseCOMMANDS)

{

   String::size_type lineLength = 200;
   origin.sendMessage(GETLANG(COMMAND_LIST_START,getName()),getName());
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	list << " " << functionTable[i].command;
	if (list.str().length() >= lineLength)
	  {
	     origin.sendMessage(list.str(),getName());
	     list.str() = " -=>";
	  }
     }
   if (list.str().length() > 4)
     {
	origin.sendMessage(list.str(),getName());
     }
   origin.sendMessage(GETLANG(COMMAND_LIST_END),getName());

}

CHAN_FUNC (Module::parseSEEN)
{
   origin.sendMessage(GETLANG(ERROR_COMMAND_DISABLED),getName());
   return;
}

CHAN_FUNC (Module::parseSET)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_USAGE_SET),getName());
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   if(!ptr->isRegistered())
     {
        origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick  = st.nextToken();
	if((ptr->getAccess(currnick)>400))
	  {
	     std::cout << command << std::endl;
	     std::cout << value << std::endl;
	     if(command=="log")
	       {
		  if(value=="true")
		    {
		       ptr->setChanLog(true);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_TRUE),getName());
		       return;
		    }
		  if(value=="false")
		    {
		       ptr->setChanLog(false);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_FALSE),getName());
		       return;
		    }
		  origin.sendMessage(GETLANG(chan_SET_CHANLOG_FALSE),getName());
		  return;
	       } // Log
	     if(command=="secure")
	       {

		  if(value=="true")
		    {
		       ptr->setChanSecure(true);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_TRUE),getName());
		       return;
		    }
		  if(value=="false")
		    {
		       ptr->setChanSecure(false);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_FALSE),getName());
		       return;
		    }
		  origin.sendMessage(GETLANG(chan_SET_SECURE_USAGE),getName());
		  return;
	       } // Secure
	     origin.sendMessage(GETLANG(chan_SET_UNSUPPORTED_OPTION),getName());
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseLISTBAN)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_LISTBAN_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getName());
	return;
     }
   String tempnick = origin.getNickname();
   ptr->sendBans(tempnick,getName());
}

CHAN_FUNC (Module::parseINFO)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_INFO_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   if(!ptr->isRegistered())
     {
        origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getName());
	return;
     }
   origin.sendMessage(GETLANG(chan_INFO_START,channel),getName());
   origin.sendMessage(GETLANG(chan_INFO_OWNER,ptr->getOwner()),getName());
   origin.sendMessage(GETLANG(chan_INFO_UNIQUEIDS,
		      String::convert(ptr->getRegisteredID()),
		      String::convert(ptr->getOnlineID())),getName());
}

CHAN_FUNC (Module::parseADDUSER)
{
   String channel = "";
   if(!chan.empty())
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }
   String nickname = tokens.nextToken();
   String level = tokens.nextToken();
   if(channel=="" | nickname=="" | level=="")
     {
	origin.sendMessage(GETLANG(chan_ADDUSER_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   User *uptr = services->findUser(nickname);
    if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }
   if(!uptr->isRegistered())
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getName());
	return;
     }

   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick  = st.nextToken();
	int access = ptr->getAccess(currnick);
	int waccess = level.toInt();
	int taccess = ptr->getAccess(nickname);
	if(waccess==access || waccess>access || waccess<1 || waccess>499)
	  {
	     origin.sendMessage(GETLANG(chan_ADDUSER_LEVEL_MISMATCH),getName());
	     return;
	  }
	if(taccess>0)
	  {
	     origin.sendMessage(GETLANG(chan_ADDUSER_ALREADY_ADDED),getName());
	     return;
	  }
	ptr->addAccess(nickname,level);
	ptr->log(origin,getName(),"Added "+nickname+" with level "+level,channel);
	origin.sendMessage(GETLANG(chan_ADDUSER_SUCCESS),getName());
	return;
     }
}

CHAN_FUNC (Module::parseTOPIC)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   String topic = tokens.rest();
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>150)
	  {
	     ptr->updateTopic(topic);
	     ptr->setTopic(getName(),topic);
	     ptr->log(origin,"Chan","Set topic to "+topic,channel);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getName(),word,parm);
}

CHAN_FUNC (Module::parseBAN)
{
   String channel = "";
   if(chan!="")
     {

	channel = chan;
     }

   else
     {

	channel = tokens.nextToken();
     }

   if(channel=="")
     {

	origin.sendMessage(GETLANG(chan_BAN_USAGE),getName());
	return;
     }

   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {

	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }
   String who = tokens.nextToken();
   String reason = tokens.rest();
   if(who=="")
     {
	origin.sendMessage(GETLANG(chan_BAN_USAGE),getName());
	return;
     }
   if(reason=="")
     {
	reason = GETLANG(chan_BAN_DEFAULT_REASON);
     }

   User *uptr = services->findUser(who);
   if(uptr==0)
     {
	origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),getName());
	return;
     }
   ptr->ban(*uptr,getName(),reason,origin.getNickname());
   origin.sendMessage(GETLANG(chan_BAN_SUCCESS),getName());
}

CHAN_FUNC (Module::parseREGISTER)
{
   String channel = tokens.nextToken();
   if (channel=="")
     {
        origin.sendMessage(GETLANG(chan_REGISTER_USAGE),getName());
	return;
     }
   if ( channel[0] != '#' )
     {
	origin.sendMessage(GETLANG(chan_REGISTER_NO_HASH),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);

   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   if(!origin.isRegistered())
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getName());
	return;
     }
   if(!origin.isIdentified(origin.getNickname()))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),getName());
	return;
     }
   int owned = services->getChannel().ownedChannels(origin.getNickname());
   if(owned>0)
     {
	origin.sendMessage(GETLANG(chan_REGISTER_GREEDY_USER,Kine::config().getNetworkName()),getName());
	return;
     }
   if(ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_REGISTER_ALREADY),getName());
	return;
     }
   services->getChannel().registerChannel(channel,origin.getNickname());
   origin.sendMessage(GETLANG(chan_REGISTER_SUCCESS),getName());
   ptr->log(origin,"Chan","Registered the channel",channel);
   return;
}

CHAN_FUNC (Module::parseOP)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_OP_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }
   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	bool foundmatch = false;
	if(ptr->getAccess(currnick)>100)
	  {
	     String foo = tokens.nextToken();
	     User *fptr = services->findUser(foo);
	     if(foo=="")
	       {
		  if(!ptr->isOp(origin.getNickname()))
		    {
		       ptr->mode("Chan","+o",origin.getNickname());
		       ptr->addUser(origin,2);
		       ptr->log(origin,"Chan","Opped themselves",channel);
		       return;
		    }
		  return;
	       }
             if(fptr==0)
	       return;

	     if(!ptr->isOp(foo))
	       {
		  ptr->mode("Chan","+o",foo);
		  ptr->addUser(*fptr,2);
		  ptr->log(origin,"Chan","Opped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!ptr->isOp(foo))
		    {
		       ptr->mode("Chan","+o",foo);
		       User *fptr = services->findUser(foo);
		       if(fptr==0)
			 return;
		       ptr->addUser(*fptr,2);
		       ptr->log(origin,"Chan","Opped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseDEOP)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_DEOP_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>100)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(ptr->isOp(origin.getNickname()))
		    {
		       ptr->mode("Chan","-o",origin.getNickname());
		       ptr->addUser(origin,0);
		       ptr->log(origin,"Chan","Deopped themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(ptr->isOp(foo))
	       {
		  ptr->mode("Chan","-o",foo);
		  ptr->addUser(*fptr,0);
		  ptr->log(origin,"Chan","Deopped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  User *fptr = services->findUser(foo);
		  if(fptr==0)
		    return;
		  if(ptr->isOp(foo))
		    {
		       ptr->mode("Chan","-o",foo);
		       ptr->addUser(*fptr,0);
		       ptr->log(origin,"Chan","Deopped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseVOICE)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_VOICE_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>50)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(!ptr->isVoice(origin.getNickname()))
		    {
		       ptr->mode("Chan","+v",origin.getNickname());
		       ptr->addUser(origin,1);
		       ptr->log(origin,"Chan","Voiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(!ptr->isVoice(foo))
	       {
		  ptr->mode("Chan","+v",foo);
		  ptr->addUser(*fptr,1);
		  ptr->log(origin,"Chan","Voiced "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  User *fptr = services->findUser(foo);
		  if(fptr==0)
		    return;
		  if(!ptr->isVoice(foo))
		    {
		       ptr->mode("Chan","+v",foo);
		       ptr->addUser(*fptr,1);
		       ptr->log(origin,"Chan","Voiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseDEVOICE)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_DEVOICE_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>50)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(ptr->isVoice(origin.getNickname()))
		    {
		       ptr->mode("Chan","-v",origin.getNickname());
		       ptr->addUser(origin,0);
		       ptr->log(origin,"Chan","DeVoiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(ptr->isVoice(foo))
	       {
		  ptr->mode("Chan","-v",foo);
		  ptr->addUser(*fptr,0);
		  ptr->log(origin,"Chan","DeVoiced "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  User *fptr = services->findUser(foo);
		  if(fptr==0)
		    return;
		  if(ptr->isVoice(foo))
		    {
		       ptr->mode("Chan","-v",foo);
		       ptr->addUser(*fptr,0);
		       ptr->log(origin,"Chan","DeVoiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseKICK)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   String who = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" | who=="" | reason=="")
     {
	origin.sendMessage(GETLANG(chan_KICK_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   String thelist = origin.getIDList();
   StringTokens st (thelist);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>100)
	  {
	     if(who.toLower()==getName())
	       {
		  ptr->kick(getName(),origin.getNickname(),"Trying to kick me isn't nice");
		  return;
	       }
	     ptr->kick(getName(),who,"("+origin.getNickname()+"/"+currnick+") "+reason);
	     User *fptr = services->findUser(who);
	     if(fptr==0)
	       return;
	     ptr->delUser(*fptr);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
   return;
}

CHAN_FUNC (Module::parseACCESS)
{
   String channel = "";
   if(chan!="")
     {
	channel = chan;
     }
   else
     {
	channel = tokens.nextToken();
     }

   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_ACCESS_USAGE),getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }

   if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getName());
	return;
     }

   origin.sendMessage(GETLANG(chan_ACCESS_START,channel),
		      getName());

   int nbRes =
     services->getDatabase().dbSelect("nickid,access", "chanaccess",
				      "chanid=" +
				      String::convert(ptr->getRegisteredID()));

   CResult *myRes = services->getDatabase().dbGetResultSet();

   String tnickid, taccess;

   for (int i = 0; i < nbRes; i++)
     {
	int foo = myRes->getValue(i,0).toInt();
	origin.sendMessage(services->getNick(myRes->getValue(i,0).toInt()) + " has level "
			   +myRes->getValue(i,1)+" access",getName());
     }
   origin.sendMessage(GETLANG(chan_ACCESS_FINISH,channel),getName());
   ptr->log(origin,"Chan","Did a channel access",channel);
   //Finished with result set! Clean up
   delete myRes;
}

EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }

void
  Module::handleAway(Exordium::User& origin, const String &reason)
{

   if(reason=="")
     {
	/* User is coming back - do nothing  - should really op em i suppose*/
	return;
     }
   if(origin.deopAway())
     {
	if(services->getDatabase().dbSelect("chanid","chanstatus",
					    "nickid="+origin.getOnlineIDString()+
					    " AND status=2") > 0)
	  {
	     String foo = services->getDatabase().dbGetValue();
	     String cname = services->getChannel().getChanIDName(foo.toInt());
	     String cstr = origin.getNickname()+" "+origin.getNickname();
	     services->serverMode(cname,"-o+v",cstr);
	     services->getChannel().internalVoice(origin.getNickname(),cname);
	     services->getChannel().internalDeOp(origin.getNickname(),cname);

	  }

     }

}

// Module information structure
const Module::moduleInfo_type Module::moduleInfo =
{
   "Channel Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::CLIENT_AWAY /* AWAY's */
};

// Start the service
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;

   // Try and do a db thing?
   db = new CDatabase(services->getDatabase());
   // Attempt to affirm our database tables..
   unsigned int i = 0;
   while (Tables::tables[i] != 0)
     {
	// Try to affirm this table..
	if (!services->getDatabase().affirmTable(*(Tables::tables[i])))
	  {
	     services->logLine(String("Unable to affirm mod_chan database "
				      "table '") +
			       Tables::tables[i]->name + "'",
			       Log::Fatality);
	     return false;
	  }

	// Next table..
	i++;
     }
   Kine::langs().registerMap(Language::tagMap);
   // Register ourself to the network
   services->registerService(getName(), getName(),
			     getConfigData().getHostname(),
			     getConfigData().getDescription());

   // This should be internal :(
   services->getChannel().synchChannels();

   // We started okay :)
   return true;
}

// Stop the service - bye bye!
void Module::stop(const String& reason)
{
   // Quit :(
   services->serviceQuit(getName(), reason);
}
