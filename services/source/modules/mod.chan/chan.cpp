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
#include "language.h"

#include <exordium/channel.h>
#include <kineircd/config.h>
#include <exordium/dchan.h>
#include <exordium/database/base.h>
#include <exordium/database/database.h>
#include <aisutil/utils.h>

using AISutil::String;
using AISutil::StringTokens;
using AISutil::Utils;
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
     {"drop", &Module::parseDROP},
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
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND,command),getNickname());
}

CHAN_FUNC (Module::parseCOMMANDS)

{

   String::size_type lineLength = 200;
   origin.sendMessage(GETLANG(COMMAND_LIST_START,getNickname()),getNickname());
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	list << " " << functionTable[i].command;
	if (list.str().length() >= lineLength)
	  {
	     origin.sendMessage(list.str(),getNickname());
	     list.str() = " -=>";
	  }
     }
   if (list.str().length() > 4)
     {
	origin.sendMessage(list.str(),getNickname());
     }
   origin.sendMessage(GETLANG(COMMAND_LIST_END),getNickname());

}

CHAN_FUNC (Module::parseSEEN)
{
   origin.sendMessage(GETLANG(ERROR_COMMAND_DISABLED),getNickname());
   return;
}

CHAN_FUNC (Module::parseSET)
{
   String channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_USAGE_SET),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(!ptr->isRegistered())
     {
        origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getNickname());
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
		  if(AISutil::Utils::toBool(value)==1)
		    {
		       ptr->setChanLog(true);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_TRUE),getNickname());
                       return;
		    }
		  else
		    {
		       ptr->setChanLog(false);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_FALSE),getNickname());
                       return;
		    }
		  origin.sendMessage(GETLANG(chan_SET_CHANLOG_FALSE),getNickname());
		  return;
	       }
	     // Log
	     //
	     if(command=="secureops" || command=="secure")
	       {

		  if(AISutil::Utils::toBool(value)==1)
		    {
		       ptr->setChanSecure(true);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_TRUE),getNickname());
                       return;
		    }
		  else
		    {
		       ptr->setChanSecure(false);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_FALSE),getNickname());
                       return;
		    }
		  origin.sendMessage(GETLANG(chan_SET_SECURE_USAGE),getNickname());
		  return;
	       }
	     // Secure
	     //
             if(command=="enforcebans")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setEnforceBans(true);
                       origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setEnforceBans(false);
                       origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_FALSE), getNickname());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_USAGE),getNickname());
                  return;

               }
	     // Enforcebans
	     //
             if(command=="tracktopics")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setTrackTopics(true);
                       origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setTrackTopics(false);
                       origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_FALSE), getNickname());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_USAGE),getNickname());
                  return;

               }

             if(command=="mlock")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setModeLock(true);
                       origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setModeLock(false);
                       origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_FALSE), getNickname());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_USAGE),getNickname());
                  return;

               }

             // Note: Right now there is no validation on the modes..
             if(command=="mode")
               {
                  if( value.length() > 0 )
		    {
		       if ( value[0] == '+' || value[0] == '-' )
			 {
			    ptr->setChannelModes( value );
			    origin.sendMessage(GETLANG(chan_SET_MODE, value), getNickname());
			 }
		       else
			 origin.sendMessage(GETLANG(chan_SET_MODE_USAGE),getNickname());
		    }
                  else
		    {
		       ptr->setChannelModes( String("") );
		       origin.sendMessage(GETLANG(chan_SET_MODE, ""), getNickname());
		    }

                  return;

               }

             if(command=="entrymsg")
               {
                  String line = value + " " +tokens.rest();

                  if( line.length() > 200 )
		    ptr->setEntryMsg(line.substr(0, 200));
                  else
		    ptr->setEntryMsg(line);

                  if(value.length()>0)
		    origin.sendMessage(GETLANG(chan_SET_ENTRY_MSG), getNickname());
                  else
		    origin.sendMessage(GETLANG(chan_SET_ENTRY_MSG_OFF), getNickname());

                  return;
               }

             if(command=="partmsg")
               {
                  String line = value + " " +tokens.rest();

                  if( line.length() > 200 )
		    ptr->setPartMsg(line.substr(0, 200));
                  else
		    ptr->setPartMsg(line);

                  if(value.length()>0)
		    origin.sendMessage(GETLANG(chan_SET_PART_MSG), getNickname());
                  else
		    origin.sendMessage(GETLANG(chan_SET_PART_MSG_OFF), getNickname());

                  return;
               }

             if(command=="private")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setPrivate(true);
                       origin.sendMessage(GETLANG(chan_SET_PRIVATE_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setPrivate(false);
                       origin.sendMessage(GETLANG(chan_SET_PRIVATE_FALSE), getNickname());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_PRIVATE_USAGE),getNickname());
                  return;

               }


             if(command=="description")
               {
                  String line = value + " " +tokens.rest();

                  if( line.length() > 250 )
                    ptr->setChanDescription(line.substr(0, 250));
                  else
                    ptr->setChanDescription(line);

                  origin.sendMessage(GETLANG(chan_SET_DESCRIPTION), getNickname());

                  return;
               }


             if(command=="url")
               {
                  String line = value + " " +tokens.rest();

                  if( line.length() > 200 )
                    ptr->setUrl(line.substr(0, 200));
                  else
                    ptr->setUrl(line);

                  origin.sendMessage(GETLANG(chan_SET_URL), getNickname());

                  return;
               }



	     origin.sendMessage(GETLANG(chan_SET_UNSUPPORTED_OPTION),getNickname());
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   delete ptr;
   return;
}

CHAN_FUNC (Module::parseLISTBAN)
{
   String channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_LISTBAN_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {

	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }
   String tempnick = origin.getNickname();
   ptr->sendBans(tempnick,getNickname());
   return;
}

CHAN_FUNC (Module::parseINFO)
{
   String channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_INFO_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(!ptr->isRegistered())
     {
        origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {

	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   origin.sendMessage(GETLANG(chan_INFO_START,channel),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_OWNER,ptr->getOwner()),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_REGDATE,ptr->getRegistrationDate()),getNickname());


   bool Private = ptr->getPrivate();

   if( Private )
   {
       origin.sendMessage(GETLANG(chan_INFO_PRIVATE, ptr->getPrivate() ? "ON" : "OFF"),getNickname());

       // Privacy is on, only IRCops and people with access may see the information.
       String modes = origin.getModes();
       int pos = modes.find("o");

       if( ( pos < 0  || pos > modes.length() ) && ptr->getAccess( origin.getNickname() ) < 1 )
          return;
   }

   origin.sendMessage(GETLANG(chan_INFO_CDESC, ptr->getChanDescription()),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_URL, ptr->getUrl()),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_TOPIC, ptr->getTopic()),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_MODES, ptr->getChannelModes()),getNickname());

   origin.sendMessage(GETLANG(chan_INFO_SET_OPTIONS_START),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_SET_SECURE, ptr->isChanSecure() ? "ON" : "OFF"),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_SET_TRACKTOPICS, ptr->getTrackTopics() ? "ON" : "OFF"),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_SET_ENFORCEBANS, ptr->getEnforceBans() ? "ON" : "OFF"),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_SET_MLOCK, ptr->getModeLock() ? "ON" : "OFF"),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_SET_OPTIONS_END),getNickname());

   origin.sendMessage(GETLANG(chan_INFO_UNIQUEIDS,
			      String::convert(ptr->getRegisteredID()),
			      String::convert(ptr->getOnlineID())),getNickname());
}

CHAN_FUNC (Module::parseADDUSER)
{
   String channel = tokens.nextToken();
   String nickname = tokens.nextToken();
   String level = tokens.nextToken();
   if(channel=="" | nickname=="" | level=="")
     {
	origin.sendMessage(GETLANG(chan_ADDUSER_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(!services->isNickRegistered(nickname))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
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
	     origin.sendMessage(GETLANG(chan_ADDUSER_LEVEL_MISMATCH),getNickname());
	     return;
	  }
	if(taccess>0)
	  {
	     origin.sendMessage(GETLANG(chan_ADDUSER_ALREADY_ADDED),getNickname());
	     return;
	  }
	ptr->addAccess(nickname,level);
	ptr->log(origin,getNickname(),"Added "+nickname+" with level "+level,channel);
	origin.sendMessage(GETLANG(chan_ADDUSER_SUCCESS),getNickname());
	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseTOPIC)
{
   String channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_TOPIC_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
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
	     ptr->setTopic(getNickname(),topic);
	     ptr->log(origin, getNickname() ,"Set topic to "+topic,channel);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getNickname(),word,parm);
}

CHAN_FUNC (Module::parseBAN)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_BAN_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   String who = tokens.nextToken();
   String reason = tokens.rest();
   if(who=="")
     {
	origin.sendMessage(GETLANG(chan_BAN_USAGE),getNickname());
	return;
     }
   if(reason=="")
     {
	reason = GETLANG(chan_BAN_DEFAULT_REASON);
     }

   User *uptr = services->findUser(who);
   if(uptr==0)
     {
	origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {

	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   ptr->ban(*uptr,getNickname(),reason,origin.getNickname());
   origin.sendMessage(GETLANG(chan_BAN_SUCCESS),getNickname());
}

CHAN_FUNC (Module::parseREGISTER)
{
   String channel = tokens.nextToken();
   if (channel=="")
     {
        origin.sendMessage(GETLANG(chan_REGISTER_USAGE),getNickname());
	return;
     }
   if ( channel[0] != '#' )
     {
	origin.sendMessage(GETLANG(chan_REGISTER_NO_HASH),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);

   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }

   if(!origin.isRegistered())
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   if(!origin.isIdentified(origin.getNickname()))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),getNickname());
	return;
     }
   int owned = services->getChannel().ownedChannels(origin.getNickname());
   if(owned>0)
     {
	origin.sendMessage(GETLANG(chan_REGISTER_GREEDY_USER,Kine::config().getNetworkName()),getNickname());
	return;
     }
   if(ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_REGISTER_ALREADY),getNickname());
	return;
     }
   services->getChannel().registerChannel(channel,origin.getNickname());
   origin.sendMessage(GETLANG(chan_REGISTER_SUCCESS),getNickname());
   ptr->log(origin, getNickname(),"Registered the channel",channel);
   return;
}

CHAN_FUNC (Module::parseOP)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_OP_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
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
	if(ptr->getAccess(currnick)>99)
	  {
	     String foo = tokens.nextToken();
	     User *fptr = services->findUser(foo);
	     if(foo=="")
	       {
		  if(!ptr->isOp(origin.getNickname()))
		    {
		       ptr->mode(getNickname(),"+o",origin.getNickname());
		       ptr->addUser(origin,2);
		       ptr->log(origin,getNickname(),"Opped themselves",channel);
		       return;
		    }
		  return;
	       }
             if(fptr==0)
	       return;

	     if(!ptr->isOp(foo))
	       {
		  ptr->mode(getNickname(),"+o",foo);
		  ptr->addUser(*fptr,2);
		  ptr->log(origin,getNickname(),"Opped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!ptr->isOp(foo))
		    {
		       ptr->mode(getNickname(),"+o",foo);
		       User *fptr = services->findUser(foo);
		       if(fptr==0)
			 return;
		       ptr->addUser(*fptr,2);
		       ptr->log(origin,getNickname(),"Opped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseDEOP)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_DEOP_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   String la = origin.getIDList();
   StringTokens st (la);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>99)
	  {
	     String foo = tokens.nextToken();
	     if(foo=="")
	       {
		  if(ptr->isOp(origin.getNickname()))
		    {
		       ptr->mode(getNickname(),"-o",origin.getNickname());
		       ptr->addUser(origin,0);
		       ptr->log(origin,getNickname(),"Deopped themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(ptr->isOp(foo))
	       {
		  ptr->mode(getNickname(),"-o",foo);
		  ptr->addUser(*fptr,0);
		  ptr->log(origin,getNickname(),"Deopped "+foo,channel);
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
		       ptr->mode(getNickname(),"-o",foo);
		       ptr->addUser(*fptr,0);
		       ptr->log(origin,getNickname(),"Deopped " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseVOICE)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_VOICE_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
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
		       ptr->mode(getNickname(),"+v",origin.getNickname());
		       ptr->addUser(origin,1);
		       ptr->log(origin,getNickname(),"Voiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(!ptr->isVoice(foo))
	       {
		  ptr->mode(getNickname(),"+v",foo);
		  ptr->addUser(*fptr,1);
		  ptr->log(origin,getNickname(),"Voiced "+foo,channel);
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
		       ptr->mode(getNickname(),"+v",foo);
		       ptr->addUser(*fptr,1);
		       ptr->log(origin,getNickname(),"Voiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseDEVOICE)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_DEVOICE_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
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
		       ptr->mode(getNickname(),"-v",origin.getNickname());
		       ptr->addUser(origin,0);
		       ptr->log(origin,getNickname(),"DeVoiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(ptr->isVoice(foo))
	       {
		  ptr->mode(getNickname(),"-v",foo);
		  ptr->addUser(*fptr,0);
		  ptr->log(origin,getNickname(),"DeVoiced "+foo,channel);
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
		       ptr->mode(getNickname(),"-v",foo);
		       ptr->addUser(*fptr,0);
		       ptr->log(origin,getNickname(),"DeVoiced " +foo,channel);
		    }
		  more = tokens.hasMoreTokens();
	       }
	     more = st.hasMoreTokens();
	  }

	return;
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseKICK)
{
   String channel = tokens.nextToken();
   String who = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" | who=="" | reason=="")
     {
	origin.sendMessage(GETLANG(chan_KICK_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   String thelist = origin.getIDList();
   StringTokens st (thelist);
   bool more = false;
   more = st.hasMoreTokens();
   while(more==true)
     {
	String currnick = st.nextToken();
	if(ptr->getAccess(currnick)>99)
	  {
	     if(who.toLower()==getNickname())
	       {
		  ptr->kick(getNickname(),origin.getNickname(),"Trying to kick me isn't nice");
		  return;
	       }
	     ptr->kick(getNickname(),who,"("+origin.getNickname()+"/"+currnick+") "+reason);
	     User *fptr = services->findUser(who);
	     if(fptr==0)
	       return;
	     ptr->delUser(*fptr);
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
   return;
}

CHAN_FUNC (Module::parseACCESS)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_ACCESS_USAGE),getNickname());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }

   if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getNickname());
	return;
     }
   if(services->isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   origin.sendMessage(GETLANG(chan_ACCESS_START,channel),
		      getNickname());

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
			   +myRes->getValue(i,1)+" access",getNickname());
     }
   origin.sendMessage(GETLANG(chan_ACCESS_FINISH,channel),getNickname());
   ptr->log(origin,getNickname(),"Did a channel access",channel);
   //Finished with result set! Clean up
   delete myRes;
}

CHAN_FUNC (Module::parseDROP)
{
   String channel = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" || reason=="")
     {
        origin.sendMessage(GETLANG(chan_DROP_USAGE),getNickname());
        return;
     }
   dChan *dptr = services->findChan(channel);
   if(dptr==0)
     {
        origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
        return;
     }

   if(dptr->isRegistered())
     {
	if(dptr->getAccess(origin.getNickname()) == 500)
	  {
	     if(origin.isIdentified())
	       {
		  services->getChannel().deregisterChannel(channel);
		  String togo = "This channel has been deregistered \002"+reason;
		  services->serviceNotice(String(togo),getNickname(),channel);
		  origin.sendMessage(GETLANG(chan_DROP_SUCCESS),getNickname());
	       }
	     else
	       {
		  origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),getNickname());
	       }
	  }
	else
	  {
	     origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getNickname());
	  }
     }
   else
     {
	origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getNickname());
     }

}

EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }

void
  Module::handleAway(Exordium::User& origin, const String &reason)
{

   if(!origin.isIdentified(origin.getNickname()))
     {
	/* Not identified, do nothing */
     }
   
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
//	     services->getChannel().internalVoice(origin.getNickname(),cname);
//	     services->getChannel().internalDeOp(origin.getNickname(),cname);

	  }

     }

}

// Handle a TOPIC command
// Context: When we enter this method we know that the channel is valid
// and that the channel is registered.
void
  Module::handleTopic(const String& origin, Exordium::dChan& channel, const String &newTopic)
{

   // If the TOPIC was sent by myself(chan) we just return
   if( origin.toLower() == getNickname().toLower() )
     return;

   User *source = services->findUser( (String&)origin );

   // In this case the TOPIC was sent by a server, for example returning from a netsplit.
   // Also, if the channel is freezed we revert back.
   if( source == NULL || services->isFreezed( channel.getName() ) )
     {
	channel.setTopic( getNickname(), channel.getTopic() );
     }

   // Else it's from a user, we must check access
   else
     {

	// If the user doesnt have required access we revert back
	if( channel.getAccess( origin ) <= 100 )
	  channel.setTopic( getNickname(), channel.getTopic() );
	else
	  {
	     if( channel.getTrackTopics() )
	       channel.updateTopic( newTopic );
	  }

     }

}

void
  Module::handleChannelJoin( User &origin, dChan &channel, const int &status )
{
   String msg = channel.getEntryMsg();

   if( msg.length() > 0 )
     origin.sendMessage( msg, getNickname() );

   if( status < 2 )
     {
	if(origin.isIdentified(origin.getNickname()))
	  {
	     if(channel.getAccess(origin.getNickname())>99)
	       {
		  channel.mode(getNickname(),"+o",origin.getNickname());
		  channel.addUser(origin,2);
		  channel.log(origin,getNickname(),"Autoopped on join",channel.getName());
	       }

	  }
     }
   else
     {
	if(channel.isChanSecure())
	  {
	     if(channel.getAccess(origin.getNickname())<100)
	       {
		  channel.mode(getNickname(), "-o", origin.getNickname());
		  channel.addUser(origin, status-2);
	       }
	  }

     }

}

void
  Module::handleChannelPart( User &origin, dChan &channel )
{
   String msg = channel.getPartMsg();

   if( msg.length() > 0 )
     origin.sendMessage( msg, getNickname() );
}

void
  Module::handleChannelMode( dChan &channel, const String &modes, const String &target, const String &source )
{
   bool add=false;
   bool take=false;
   int i;
   int length = modes.length();
   String targ;

   StringTokens st(target);

   for (i = 0; i!=length; i++)
     {
	if (modes[i] == '+')
	  {
	     add = true;
	     take = false;
	     continue;
	  }
	if (modes[i] == '-')
	  {
	     add = false;
	     take = true;
	     continue;
	  }

	if(modes[i]=='b')
	  {
	     if(add)
	       {
                  targ = st.nextToken();

		  std::cout << "New Channel ban set by : " << source << " on " << targ << std::endl;

	       }
	     if(take)
	       {
                  targ = st.nextToken();

		  std::cout << "New Channel ban removed by : " << source << " on " << targ << std::endl;
	       }
	  }

	if(modes[i] == 'o')
	  {
	     if(add)
	       {
                  targ = st.nextToken();

		  if(targ.toLower()==getNickname().toLower())
                    return;

		  std::cout << "Checking to see if " << targ << " has access in " << channel.getName() << std::endl;

		  User *ptr = services->findUser( targ );

		  if(channel.isChanSecure())
		    {
		       std::cout << "  ^- SecureOps is enabled" << std::endl;

                    /* Channel has SECURE enabled */
		       int axs = channel.getAccess(targ);
		       std::cout << "  ^- " << targ << " has level " << axs << std::endl;

		       if(axs<100)
			 {
			    std::cout << " ^- Deop the bitch" << std::endl;
                         /* Hey not allowed to be opped!! */

			    channel.mode(getNickname(), "-o",targ);
                         /* Tell origin off! */

			 }
		    }

		  channel.addUser(*ptr,2);
	       }

	     if(take)
	       {
                  targ = st.nextToken();

		  if(targ.toLower()==getNickname().toLower())
		    return;

		  User *ptr = services->findUser( targ );

		  if(channel.isChanSecure())
		    {
		       std::cout << "  ^- SecureOps is enabled" << std::endl;

                    /* Channel has SECURE enabled */
		       int axs = channel.getAccess(source);
		       std::cout << "  ^- " << source << " has level " << axs << std::endl;

		       if(axs<100)
			 {
			    std::cout << " ^- Reop the user who was deopped by non-privileged user" << std::endl;

			    channel.mode(getNickname(), "+o",targ);

			 }
		       else
                         channel.addUser(*ptr,0);

		    }
		  else
                    channel.addUser(*ptr,0);

	       }
	  }

	if(modes[i] == 'v')
	  {
	     if(add)
	       {
                  targ = st.nextToken();

		  if(targ.toLower()==getNickname().toLower())
		    return;
		  User *ptr = services->findUser( targ );
		  channel.addUser(*ptr,1);
	       }
	     if(take)
	       {
                  targ = st.nextToken();

		  if(targ.toLower()==getNickname().toLower())
		    return;
		  User *ptr = services->findUser( targ );
		  channel.addUser(*ptr,0);
	       }
	  }
     }

}

//void
//  Module::handleBan
//
//
//
// Module information structure
const Module::moduleInfo_type Module::moduleInfo =
{
   "Channel Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::CLIENT_AWAY   |   /* AWAY's */
     Exordium::Service::moduleInfo_type::Events::CHANNEL_TOPIC |   /* Topic being changed */
     Exordium::Service::moduleInfo_type::Events::CHANNEL_JOIN  |   /* User joining the channel */
     Exordium::Service::moduleInfo_type::Events::CHANNEL_PART  |   /* User leaving a channel */
     Exordium::Service::moduleInfo_type::Events::CHANNEL_BAN   |   /* Channel ban activated */
     Exordium::Service::moduleInfo_type::Events::CHANNEL_MODE      /* Channel mode applied */

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
   services->registerService(getNickname(), getUsername(),
			     getHostname(), getDescription());

   // This should be internal :(
   services->getChannel().synchChannels();

   // We started okay :)
   return true;
}

// Stop the service - bye bye!
void Module::stop(const String& reason)
{
   // Quit :(
   services->serviceQuit(getNickname(), reason);
}
