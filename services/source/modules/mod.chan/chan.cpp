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
#include <aisutil/utils.h>

#include "language.h"

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
 //    {GETLANG(chan_COMMAND_INFO), &Module::parseINFO},
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
		  if(AISutil::Utils::toBool(value)==1)
		    {
		       ptr->setChanLog(true);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_TRUE),getName());
                       return;
		    }
		  else
		    {
		       ptr->setChanLog(false);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_FALSE),getName());
                       return;
		    }
		  origin.sendMessage(GETLANG(chan_SET_CHANLOG_FALSE),getName());
		  return;
	       } // Log

	     if(command=="secureops" || command=="secure")
	       {

		  if(AISutil::Utils::toBool(value)==1)
		    {
		       ptr->setChanSecure(true);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_TRUE),getName());
                       return;
		    }
		  else
		    {
		       ptr->setChanSecure(false);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_FALSE),getName());
                       return;
		    }
		  origin.sendMessage(GETLANG(chan_SET_SECURE_USAGE),getName());
		  return;
	       } // Secure

             if(command=="enforcebans")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setEnforceBans(true);
                       origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_TRUE), getName());
                       return;
                    }
                  else
                    {
                       ptr->setEnforceBans(false);
                       origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_FALSE), getName());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_USAGE),getName());
                  return;

               } // Enforcebans

             if(command=="tracktopics")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setTrackTopics(true);
                       origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_TRUE), getName());
                       return;
                    }
                  else
                    {
                       ptr->setTrackTopics(false);
                       origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_FALSE), getName());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_USAGE),getName());
                  return;

               }

             if(command=="mlock")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setModeLock(true);
                       origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_TRUE), getName());
                       return;
                    }
                  else
                    {
                       ptr->setModeLock(false);
                       origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_FALSE), getName());
                       return;
                    }
                  origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_USAGE),getName());
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
                           origin.sendMessage(GETLANG(chan_SET_MODE, value), getName());
                      }
                      else
                        origin.sendMessage(GETLANG(chan_SET_MODE_USAGE),getName());
                  }
                  else
                  {
                      ptr->setChannelModes( String("") );
                      origin.sendMessage(GETLANG(chan_SET_MODE, ""), getName());
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
                      origin.sendMessage(GETLANG(chan_SET_ENTRY_MSG), getName());
                  else
                      origin.sendMessage(GETLANG(chan_SET_ENTRY_MSG_OFF), getName());

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
                      origin.sendMessage(GETLANG(chan_SET_PART_MSG), getName());
                  else
                      origin.sendMessage(GETLANG(chan_SET_PART_MSG_OFF), getName());

                  return;
               }



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
      if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
      if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
 /* Can't use a user pointer here, because the user may not be online */
    if(!services->isNickRegistered(nickname))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getName());
	return;
     }
   
    if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
	return;
     }
 
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
	        return;
     }
   
   
   String la = origin.getIDList();
   std::cout << "the list contains " << la << std::endl;
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
	std::cout << "At throwing into user pointer ptr thingy" << std::endl;
	ptr->addAccess(nickname,level);
	ptr->log(origin,getName(),"Added "+nickname+" with level "+level,channel);
	origin.sendMessage(GETLANG(chan_ADDUSER_SUCCESS),getName());
	return;
     }
   std::cout << "User doesn't have access to add? Bug? Feature?" << std::endl;
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
	     ptr->log(origin, getName() ,"Set topic to "+topic,channel);
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
      if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
   ptr->log(origin, getName(),"Registered the channel",channel);
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
   if(services->isFreezed(channel))
     {
	origin.sendMessage("Error: That channel is frozen",getName());
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
		       ptr->mode(getName(),"+o",origin.getNickname());
		       ptr->addUser(origin,2);
		       ptr->log(origin,getName(),"Opped themselves",channel);
		       return;
		    }
		  return;
	       }
             if(fptr==0)
	       return;

	     if(!ptr->isOp(foo))
	       {
		  ptr->mode(getName(),"+o",foo);
		  ptr->addUser(*fptr,2);
		  ptr->log(origin,getName(),"Opped "+foo,channel);
	       }
	     bool more = false;
	     more = tokens.hasMoreTokens();
	     while(more==true)
	       {
		  String foo = tokens.nextToken();
		  if(!ptr->isOp(foo))
		    {
		       ptr->mode(getName(),"+o",foo);
		       User *fptr = services->findUser(foo);
		       if(fptr==0)
			 return;
		       ptr->addUser(*fptr,2);
		       ptr->log(origin,getName(),"Opped " +foo,channel);
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
		       ptr->mode(getName(),"-o",origin.getNickname());
		       ptr->addUser(origin,0);
		       ptr->log(origin,getName(),"Deopped themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(ptr->isOp(foo))
	       {
		  ptr->mode(getName(),"-o",foo);
		  ptr->addUser(*fptr,0);
		  ptr->log(origin,getName(),"Deopped "+foo,channel);
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
		       ptr->mode(getName(),"-o",foo);
		       ptr->addUser(*fptr,0);
		       ptr->log(origin,getName(),"Deopped " +foo,channel);
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
		       ptr->mode(getName(),"+v",origin.getNickname());
		       ptr->addUser(origin,1);
		       ptr->log(origin,getName(),"Voiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(!ptr->isVoice(foo))
	       {
		  ptr->mode(getName(),"+v",foo);
		  ptr->addUser(*fptr,1);
		  ptr->log(origin,getName(),"Voiced "+foo,channel);
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
		       ptr->mode(getName(),"+v",foo);
		       ptr->addUser(*fptr,1);
		       ptr->log(origin,getName(),"Voiced " +foo,channel);
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
		       ptr->mode(getName(),"-v",origin.getNickname());
		       ptr->addUser(origin,0);
		       ptr->log(origin,getName(),"DeVoiced themselves",channel);
		       return;
		    }
		  return;
	       }
	     User *fptr = services->findUser(foo);
	     if(fptr==0)
	       return;
	     if(ptr->isVoice(foo))
	       {
		  ptr->mode(getName(),"-v",foo);
		  ptr->addUser(*fptr,0);
		  ptr->log(origin,getName(),"DeVoiced "+foo,channel);
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
		       ptr->mode(getName(),"-v",foo);
		       ptr->addUser(*fptr,0);
		       ptr->log(origin,getName(),"DeVoiced " +foo,channel);
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
   if(services->isFreezed(channel))
     {
	
	        origin.sendMessage("Error: That channel is frozen",getName());
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
   ptr->log(origin,getName(),"Did a channel access",channel);
   //Finished with result set! Clean up
   delete myRes;
}


CHAN_FUNC (Module::parseDROP)
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

   String reason = tokens.rest();

   if(channel=="" || reason=="")
     {
        origin.sendMessage(GETLANG(chan_DROP_USAGE),getName());
        return;
     }

   dChan *dptr = services->findChan(channel);

   if(dptr==NULL)
   {
        origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getName());
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
               services->serviceNotice(String(togo),getName(),channel);
               origin.sendMessage(GETLANG(chan_DROP_SUCCESS),getName());
           }
           else
           {
               origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),getName());
           }
      }
      else
      {
           origin.sendMessage(GETLANG(chan_NOT_ENOUGH_ACCESS),getName());
      }
   }
   else
   {
      origin.sendMessage(GETLANG(chan_CHAN_NOT_REGISTERED),getName());
   }

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


// Handle a TOPIC command
// Context: When we enter this method we know that the channel is valid
// and that the channel is registered.
void
  Module::handleTopic(const String& origin, Exordium::dChan& channel, const String &newTopic)
{

  // If the TOPIC was sent by myself(chan) we just return
  if( origin.toLower() == getName().toLower() )
    return;

  User *source = services->findUser( (String&)origin );

  // In this case the TOPIC was sent by a server, for example returning from a netsplit.
  // Also, if the channel is freezed we revert back.
  if( source == NULL || services->isFreezed( channel.getName() ) )
  {
     channel.setTopic( getName(), channel.getTopic() );
  }

  // Else it's from a user, we must check access
  else
  {

     // If the user doesnt have required access we revert back
     if( channel.getAccess( origin ) <= 100 )
         channel.setTopic( getName(), channel.getTopic() );
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
      origin.sendMessage( msg, getName() );


   if( status < 2 )
   {
       if(origin.isIdentified(origin.getNickname()))
       {
           if(channel.getAccess(origin.getNickname())>99)
           {
                channel.mode(getName(),"+o",origin.getNickname());
                channel.addUser(origin,2);
                channel.log(origin,getName(),"Autoopped on join",channel.getName());
           }

       }
   }
   else
   {
       if(channel.isChanSecure())
       {
           if(channel.getAccess(origin.getNickname())<100)
           {
                channel.mode(getName(), "-o", origin.getNickname());
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
      origin.sendMessage( msg, getName() );
}


void
  Module::handleChannelMode( dChan &channel, const String &modes, const String &target, const String &source )
{
  bool add=false;
  bool take=false;
  int i;
  int length = modes.length();
  String targ=target;


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
                std::cout << "New Channel ban set by : " << source << " on " << target << std::endl;

           }
           if(take)
           {
                std::cout << "New Channel ban removed by : " << source << " on " << target << std::endl;
           }
       }
  
       if(modes[i] == 'o')
       {
           if(add)
           {
                if(target.toLower()==getName().toLower())
                    return;
  
                std::cout << "Checking to see if " << target << " has access in " << channel.getName() << std::endl;
 
                User *ptr = services->findUser( targ );

                if(channel.isChanSecure())
                {
                    std::cout << "  ^- SecureOps is enabled" << std::endl;

                    /* Channel has SECURE enabled */
                    int axs = channel.getAccess(target);
                    std::cout << "  ^- " << target << " has level " << axs << std::endl;

                    if(axs<100)
                    {
                         std::cout << " ^- Deop the bitch" << std::endl;
                         /* Hey not allowed to be opped!! */

                         channel.mode(getName(), "-o",target);
                         /* Tell origin off! */

                    }
                }

                channel.addUser(*ptr,2);
           }
           
           if(take)
           {
                if(target.toLower()==getName().toLower())
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

                         channel.mode(getName(), "+o",target);

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
                 if(target.toLower()==getName().toLower())
                     return;
                 User *ptr = services->findUser( targ );
                 channel.addUser(*ptr,1);
           }
           if(take)
           {
                 if(target.toLower()==getName().toLower())
                     return;
                 User *ptr = services->findUser( targ );
                 channel.addUser(*ptr,0);
           }
       }
  }



}


//void
//  Module::handleBan



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
