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
#include <kineircd/languages.h>
#include <exordium/dchan.h>
#include <exordium/database/base.h>
#include <exordium/database/database.h>
#include <aisutil/utils.h>

using AISutil::String;
using AISutil::StringTokens;
using AISutil::Utils;
using namespace Exordium::ChanModule;

struct Service::functionTableStruct const
  Service::functionTable[] =
{
     {"access", &Service::parseACCESS},
     {"ban", &Service::parseBAN},
     {"kick", &Service::parseKICK},
     {"op", &Service::parseOP},
     {"voice", &Service::parseVOICE},
     {"register", &Service::parseREGISTER},
     {"help", &Service::parseHELP},
     {"deop", &Service::parseDEOP},
     {"devoice", &Service::parseDEVOICE},
     {"topic", &Service::parseTOPIC},
     {"adduser", &Service::parseADDUSER},
     {"info", &Service::parseINFO},
     {"listban", &Service::parseLISTBAN},
     {"set", &Service::parseSET},
     {"seen", &Service::parseSEEN},
     {"commands", &Service::parseCOMMANDS},
     {"drop", &Service::parseDROP},
     {0, 0}
};

void
  Service::parseLine (StringTokens& line, User& origin
		     , const Kine::ChannelName& chan)
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
  Service::parseLine (StringTokens& line, User& origin, const bool safe)
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

CHAN_FUNC (Service::parseCOMMANDS)

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
   origin.log(getNickname(),"Fetched commands list");

}

CHAN_FUNC (Service::parseSEEN)
{
   origin.sendMessage(GETLANG(ERROR_COMMAND_DISABLED),getNickname());
   return;
}

CHAN_FUNC (Service::parseSET)
{
   Kine::ChannelName channel = tokens.nextToken();
   channel = channel.IRCtoLower();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_USAGE_SET),getNickname());
	return;
     }
   if(isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   dChan *ptr = services.findChan(channel);
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
		       ptr->log(origin,getNickname(),"Enabled channel log email",channel);
		       origin.sendMessage(GETLANG(chan_SET_CHANLOG_TRUE),getNickname());
                       return;
		    }
		  else
		    {
		       ptr->setChanLog(false);
		       ptr->log(origin,getNickname(),"Disabled channel log email",channel);
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
		       ptr->log(origin,getNickname(),"Enabled secure ops",channel);
		       origin.sendMessage(GETLANG(chan_SET_SECURE_TRUE),getNickname());
                       return;
		    }
		  else
		    {
		       ptr->setChanSecure(false);
		       ptr->log(origin,getNickname(),"Disabled secure ops",channel);
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
		       ptr->log(origin,getNickname(),"Enabled ban enforcement",channel);
                       origin.sendMessage(GETLANG(chan_SET_ENFORCE_BANS_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setEnforceBans(false);
		       ptr->log(origin,getNickname(),"Disabled ban enforcement",channel);
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
		       ptr->log(origin,getNickname(),"Enabled topic tracking",channel);
                       origin.sendMessage(GETLANG(chan_SET_TRACK_TOPICS_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setTrackTopics(false);
		       ptr->log(origin,getNickname(),"Disabled topic tracking",channel);
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
		       ptr->log(origin,getNickname(),"Enabled mode lock",channel);
                       origin.sendMessage(GETLANG(chan_SET_MODE_LOCK_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setModeLock(false);
		       ptr->log(origin,getNickname(),"Disabled mode lock",channel);
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
			    ptr->log(origin,getNickname(),"Set modes to "+value,channel);
			    origin.sendMessage(GETLANG(chan_SET_MODE, value), getNickname());
			 }
		       else
			 origin.sendMessage(GETLANG(chan_SET_MODE_USAGE),getNickname());
		    }
                  else
		    {
		       ptr->setChannelModes( String("") );
		       ptr->log(origin,getNickname(),"Cleared channel modes",channel);
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
		    {
		       ptr->log(origin,getNickname(),"Set entry message to "+value,channel);
		       origin.sendMessage(GETLANG(chan_SET_ENTRY_MSG), getNickname());
		    }

                  else
		    {
		       ptr->log(origin,getNickname(),"Disabled entry message",channel);
		       origin.sendMessage(GETLANG(chan_SET_ENTRY_MSG_OFF), getNickname());

		    }

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
		    {
		       ptr->log(origin,getNickname(),"Set part message to "+value,channel);
		       origin.sendMessage(GETLANG(chan_SET_PART_MSG), getNickname());
		    }
                  else
		    {
		       ptr->log(origin,getNickname(),"Disabled part message",channel);
		       origin.sendMessage(GETLANG(chan_SET_PART_MSG_OFF), getNickname());
		    }

                  return;
               }

             if(command=="private")
               {
                  if(AISutil::Utils::toBool(value)==1)
                    {
                       ptr->setPrivate(true);
		       ptr->log(origin,getNickname(),"Enabled Private mode",channel);
                       origin.sendMessage(GETLANG(chan_SET_PRIVATE_TRUE), getNickname());
                       return;
                    }
                  else
                    {
                       ptr->setPrivate(false);
		       ptr->log(origin,getNickname(),"Disabled Private mode",channel);
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
		  ptr->log(origin,getNickname(),"Set channel description to "+line,channel);
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
		  ptr->log(origin,getNickname(),"Set channel url to "+line,channel);
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

CHAN_FUNC (Service::parseLISTBAN)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_LISTBAN_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
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
   if(isFreezed(channel))
     {

	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }
   Kine::ClientName tempnick = origin.getNickname();
   ptr->sendBans(tempnick,getNickname());
   return;
}

CHAN_FUNC (Service::parseINFO)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_INFO_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
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
   if(isFreezed(channel))
     {

	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   origin.sendMessage(GETLANG(chan_INFO_START,channel),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_OWNER,ptr->getOwner()),getNickname());
   origin.sendMessage(GETLANG(chan_INFO_REGDATE,ptr->getRegistrationDate()),getNickname());

   if( ptr->getPrivate() )
     {
	origin.sendMessage(GETLANG(chan_INFO_PRIVATE, ptr->getPrivate() ? "ON" : "OFF"),getNickname());

	// Privacy is on, only IRCops and people with access may see the information.
	String modes = origin.getModes();
	std::string::size_type pos = modes.find("o");

	if( ( (pos < 0)  || (pos > modes.length()) ) && ptr->getAccess( origin.getNickname() ) < 1 )
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

CHAN_FUNC (Service::parseADDUSER)
{
   Kine::ChannelName channel = tokens.nextToken();
   String nickname = tokens.nextToken();
   String level = tokens.nextToken();
   if(channel=="" | nickname=="" | level=="")
     {
	origin.sendMessage(GETLANG(chan_ADDUSER_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(!services.isNickRegistered(nickname))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),getNickname());
	return;
     }
   if(!ptr->isRegistered())
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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

CHAN_FUNC (Service::parseTOPIC)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel.empty())
     {
	origin.sendMessage(GETLANG(chan_TOPIC_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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

CHAN_FUNC (Service::parseHELP)
{
//   String word = tokens.nextToken();
//   String parm = tokens.nextToken();
//   services.doHelp(origin,getNickname(),word,parm);

   // YES, THIS CODE COULD BE GENERALISED VERY EASILY!!! :(

   // Hmm, save the help topic first, we may need it..
   String helpTopic = tokens.rest();

   // Work out the language tag we're support to look for
   std::ostringstream tagName;
   tagName << "exordium_chan_HELP";
   
   while (!tokens.hasMoreTokens()) {
      tagName << '_' << tokens.nextToken();
   }

   // this should be elsewhere.. this does the sending stuff..
   struct Callout : public Kine::Languages::callFunction_type {
      Exordium::User& origin;
      const String& nick;
      
      bool operator()(const std::string& text)
	{
	   origin.sendMessage(text, nick);
	   return true;
	};
      
      Callout(Exordium::User& o, const String& n)
	: origin(o), nick(n)
	{};
   };
   
   // Try to find this mysterious tag in kine's tag dictionary
   Kine::Languages::tagID_type tagID;

   // Yes, btw, the getTagID() routine is case-insensitive!
   if ((tagID = Kine::langs().getTagID(tagName.str())) != 
       Kine::Languages::unknownTagID) {
      // Okay, kine knows the tag! Output the help appropriately..
	 
      // this also should be elsewhere. or better yet, 'done smarter' :)
      Callout callout(origin, getNickname());
      
      // Wee, send the help data..
      Kine::langs().get(origin.getLanguage(), tagID, callout);
      
      // Bye bye.. all done..
      return;
   }
   
   /* Oh, no help data.. time to get upset and pout, just like james does when
    * he's coding..
    * 
    * What's happening here? Well, if the topic is empty (i.e. the user did not
    * specify a topic) then we could not find the base help data. That's bad,
    * and is a pretty dumb thing to let happen :) But, it's quite possible..
    */
   if (helpTopic.empty()) {
      origin.sendMessage(GETLANG(ERROR_NO_HELP_BASE_DATA),
			 getNickname());
   } else {
      origin.sendMessage(GETLANG(ERROR_NO_HELP_TOPIC,
				 helpTopic),
			 getNickname());
   }
}

CHAN_FUNC (Service::parseBAN)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_BAN_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   
   String who = tokens.nextToken();
   std::cout << "HELLO?!?!?! its " << who << std::endl;
   /* Lets figure out if we have any options. */
   int duration = 0;
   std::cout << who[0] << std::endl;
   if(who[0]=='-')
     {
	/* Its an option, lets see what it is. */
	who = who.substr(1);
	std::cout << who << std::endl;
	std::cout << who.toLower() << std::endl;
	if(who.toLower()=="seconds")
	  {
	     int tdur = tokens.nextToken().toInt();
	     if(tdur<1 || tdur>60)
	       {
		  origin.sendMessage(GETLANG(chan_BAN_NOT_SECONDS,String::convert(tdur)),getNickname());
		  return;
	       }
	     duration = duration+tdur;
	     std::cout << "Duration is upto : " << String::convert(duration) << std::endl;
	  }
	
    }

}

CHAN_FUNC (Service::parseREGISTER)
{
   Kine::ChannelName channel = tokens.nextToken();
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
   dChan *ptr = services.findChan(channel);

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
   int owned = services.getChannel().ownedChannels(origin.getNickname());
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
   services.getChannel().registerChannel(channel,origin.getNickname());
   origin.sendMessage(GETLANG(chan_REGISTER_SUCCESS),getNickname());
   ptr->log(origin, getNickname(),"Registered the channel",channel);
   return;
}

CHAN_FUNC (Service::parseOP)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_OP_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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
//	bool foundmatch = false;
	if(ptr->getAccess(currnick)>99)
	  {
	     Kine::ClientName foo = tokens.nextToken();
	     User *fptr = services.findUser(foo);
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
		  Kine::ClientName foo = tokens.nextToken();
		  if(!ptr->isOp(foo))
		    {
		       ptr->mode(getNickname(),"+o",foo);
		       User *fptr = services.findUser(foo);
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

CHAN_FUNC (Service::parseDEOP)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_DEOP_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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
	     Kine::ClientName foo = tokens.nextToken();
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
	     User *fptr = services.findUser(foo);
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
		  Kine::ClientName foo = tokens.nextToken();
		  User *fptr = services.findUser(foo);
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

CHAN_FUNC (Service::parseVOICE)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_VOICE_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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
	     Kine::ClientName foo = tokens.nextToken();
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
	     User *fptr = services.findUser(foo);
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
		  Kine::ClientName foo = tokens.nextToken();
		  User *fptr = services.findUser(foo);
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

CHAN_FUNC (Service::parseDEVOICE)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_DEVOICE_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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
	     Kine::ClientName foo = tokens.nextToken();
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
	     User *fptr = services.findUser(foo);
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
		  Kine::ClientName foo = tokens.nextToken();
		  User *fptr = services.findUser(foo);
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

CHAN_FUNC (Service::parseKICK)
{
   Kine::ChannelName channel = tokens.nextToken();
   Kine::ClientName who = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" | who=="" | reason=="")
     {
	origin.sendMessage(GETLANG(chan_KICK_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(chan_CANNOT_LOCATE_CHAN),getNickname());
	return;
     }
   if(isFreezed(channel))
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
	     ptr->log(origin,getNickname(),"Kicked "+who+" from the channel ("+reason+")",channel);
	     User *fptr = services.findUser(who);
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

CHAN_FUNC (Service::parseACCESS)
{
   Kine::ChannelName channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(chan_ACCESS_USAGE),getNickname());
	return;
     }
   dChan *ptr = services.findChan(channel);
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
   if(isFreezed(channel))
     {
	origin.sendMessage(GETLANG(ERROR_CHANNEL_FROZEN),getNickname());
	return;
     }

   origin.sendMessage(GETLANG(chan_ACCESS_START,channel),
		      getNickname());

   int nbRes =
     services.getDatabase().dbSelect("nickid,access", "chanaccess",
				      "chanid=" +
				      String::convert(ptr->getRegisteredID()));

   CResult *myRes = services.getDatabase().dbGetResultSet();

   Kine::ClientName tnickid;
   String taccess;

   for (int i = 0; i < nbRes; i++)
     {
//	int foo = myRes->getValue(i,0).toInt();
	origin.sendMessage(services.getNick(myRes->getValue(i,0).toInt()) + " has level "
			   +myRes->getValue(i,1)+" access",getNickname());
     }
   origin.sendMessage(GETLANG(chan_ACCESS_FINISH,channel),getNickname());
   ptr->log(origin,getNickname(),"Did a channel access",channel);
   //Finished with result set! Clean up
   delete myRes;
}

CHAN_FUNC (Service::parseDROP)
{
   Kine::ChannelName channel = tokens.nextToken();
   String reason = tokens.rest();
   if(channel=="" || reason=="")
     {
        origin.sendMessage(GETLANG(chan_DROP_USAGE),getNickname());
        return;
     }
   dChan *dptr = services.findChan(channel);
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
		  dptr->log(origin,getNickname(),"Deregistered the channel",channel);
		  services.getChannel().deregisterChannel(channel);
		  String togo = "This channel has been deregistered \002"+reason;
		  services.serviceNotice(String(togo),getNickname(),channel);
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

void
  Service::handleAway(Exordium::User& origin, const String &reason)
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
	if(services.getDatabase().dbSelect("chanid","chanstatus",
					    "nickid="+origin.getOnlineIDString()+
					    " AND status=2") > 0)
	  {
	     String foo = services.getDatabase().dbGetValue();
	     String cname = services.getChannel().getChanIDName(foo.toInt());
	     String cstr = origin.getNickname()+" "+origin.getNickname();
	     services.serverMode(cname,"-o+v",cstr);

	  }

     }

}

// Handle a TOPIC command
// Context: When we enter this method we know that the channel is valid
// and that the channel is registered.
void
  Service::handleTopic(const Kine::ClientName& origin, Exordium::dChan& channel, const String &newTopic)
{

   // If the TOPIC was sent by myself(chan) we just return
   if( origin.toLower() == getNickname().toLower() )
     return;

   User *source = services.findUser( (Kine::ClientName&) origin );

   // In this case the TOPIC was sent by a server, for example returning from a netsplit.
   // Also, if the channel is freezed we revert back.
   if( source == NULL || isFreezed( channel.getName() ) )
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
  Service::handleChannelJoin( User &origin, dChan &channel, const int &status )
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
  Service::handleChannelPart( User &origin, dChan &channel )
{
   String msg = channel.getPartMsg();

   if( msg.length() > 0 )
     origin.sendMessage( msg, getNickname() );
}

void
  Service::handleChannelMode( dChan &channel, const String &modes, const String &target, const String &source )
{
   bool add=false;
   bool take=false;
   int i;
   int length = modes.length();
   Kine::ClientName targ;

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

		  User *ptr = services.findUser( targ );

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

		  User *ptr = services.findUser( targ );

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
		  User *ptr = services.findUser( targ );
		  channel.addUser(*ptr,1);
	       }
	     if(take)
	       {
                  targ = st.nextToken();

		  if(targ.toLower()==getNickname().toLower())
		    return;
		  User *ptr = services.findUser( targ );
		  channel.addUser(*ptr,0);
	       }
	  }
     }

}

//void
//  Service::handleBan


bool Service::isFreezed(Kine::ChannelName const &chan)
{

   if(services.getDatabase().dbSelect("id","chanfreeze","name='"
				       +String::convert(services.getChannel().getChanID(chan.IRCtoLower()))
				      +"' AND expires>"+String::convert(Kine::daemon().getTime().seconds)) < 1)
     return false;
   else
     return true;
}

