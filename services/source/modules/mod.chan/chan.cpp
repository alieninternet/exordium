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
using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::ChanModule;

struct Module::functionTableStruct const
  Module::functionTable[] =
{
     {"access", &Module::parseACCESS},
     {".access", &Module::parseACCESS},
     {"ban", &Module::parseBAN},
     {".ban", &Module::parseBAN},
     {".kick", &Module::parseKICK},
     {"kick", &Module::parseKICK},
     {".op", &Module::parseOP},
     {"op", &Module::parseOP},
     {".voice", &Module::parseVOICE},
     {"voice", &Module::parseVOICE},
     {"register", &Module::parseREGISTER},
     {"help", &Module::parseHELP},
     {"deop", &Module::parseDEOP},
     {".deop", &Module::parseDEOP},
     {"devoice", &Module::parseDEVOICE},
     {".devoice", &Module::parseDEVOICE},
     {"topic", &Module::parseTOPIC},
     {".topic", &Module::parseTOPIC},
     {"adduser", &Module::parseADDUSER},
     {".adduser", &Module::parseADDUSER},
     {"info", &Module::parseINFO},
     {".info", &Module::parseINFO},
     {"listban", &Module::parseLISTBAN},
     {".listban", &Module::parseLISTBAN},
     {".set", &Module::parseSET},
     {"set", &Module::parseSET},
     {"seen", &Module::parseSEEN},
     {".seen", &Module::parseSEEN},
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
   origin.sendMessage("Unrecognized Command",getName());
}

CHAN_FUNC (Module::parseCOMMANDS)

{

  String::size_type lineLength = 200;
  origin.sendMessage("Command list for " + getName() + ":",getName());
  std::ostringstream list(" -=>");
  for (int i = 0; functionTable[i].command != 0; i++) {
     list << " " << functionTable[i].command;
     if (list.str().length() >= lineLength) {
          origin.sendMessage(list.str(),getName());
         list.str() = " -=>";
      }
   }
   if (list.str().length() > 4) {
      origin.sendMessage(list.str(),getName());
   }
   origin.sendMessage("End of command list",getName());



}

  CHAN_FUNC (Module::parseSEEN)
{
   origin.sendMessage("This command is disabled",getName());
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
	origin.sendMessage("Usage: set #channel option value",getName());
	return;
     }
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: Could not locate that channel",getName());
	return;
     }
   
   if(!ptr->isRegistered())
     {
        origin.sendMessage("Error: That channel is not registered",getName());
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
	     if(command=="log")
	       {
		  if(value=="true")
		    {
		       ptr->setChanLog(true);
		       origin.sendMessage("Channel logs have been enabled, the owner will receive a nightly email from this channel",getName());
		       return;
		    }
		  if(value=="false")
		    {
		       ptr->setChanLog(false);
		       origin.sendMessage("Channel logs have been disabled",getName());
		       return;
		    }
		  origin.sendMessage("Usage: set #channel log true/false",getName());
		  return;
	       } // /LOG
	     origin.sendMessage("Unsupported channel option",getName());
	     return;
	  }
	more = st.hasMoreTokens();
     }
   origin.sendMessage("You do not have enough access for that command in this channel",getName());
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
	origin.sendMessage("Usage: listban #channel",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: I cannot find that channel",getName());
	return;
     }
   
   if(!ptr->isRegistered())
     {
	origin.sendMessage("Error: Channel not registered",getName());
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
	origin.sendMessage("Usage: info #channel",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: That channel does not exist",getName());
	return;
     }
   
   if(!ptr->isRegistered())
     {
        origin.sendMessage("Error: That channel is not registered",getName());
	return;
     }
   origin.sendMessage("Channel Information for "+channel,getName());
   origin.sendMessage("Owner : "+ptr->getOwner(),getName());
   origin.sendMessage("Unique IDS : "+String::convert(ptr->getRegisteredID())+"/"+String::convert(ptr->getOnlineID()),getName());
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
	origin.sendMessage("Error: adduser #channel nickname level",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   User *uptr = services->findUser(nickname);
   if(!ptr->isRegistered())
     {
	origin.sendMessage("Error: Cannot find that channel",getName());
	return;
     }
   if(!uptr->isRegistered())
     {
	origin.sendMessage("Error: Nickname is not registered",getName());
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
	     origin.sendMessage("Error: You cannot add someone with higher, or equal access to your own",getName());
	     return;
	  }
	if(taccess>0)
	  {
	     origin.sendMessage("Error: That person already has access",getName());
	     return;
	  }
	ptr->addAccess(nickname,level);
	ptr->log(origin,getName(),"Added "+nickname+" with level "+level,channel);
	origin.sendMessage("Nickname has been added",getName());
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
	origin.sendMessage("Error: Cannot find that channel",getName());
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
   origin.sendMessage("Sorry, you do not have the required access in that channel",getName());
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
   origin.sendMessage("Disabled until next build",getName());
   return;
}


  CHAN_FUNC (Module::parseREGISTER)
{
   String channel = tokens.nextToken();
   if (channel=="")
     {
        origin.sendMessage("Usage: register #channel",getName());
	return;
     }
   if ( channel[0] != '#' )
     {
	origin.sendMessage("Error: Channel names must begin with the '#' symbol",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   
   if(ptr==0)
     {
	origin.sendMessage("Error: Channel does not exist",getName());
	return;
     }
   
   if(!origin.isRegistered())
     {
	origin.sendMessage("Error: Your nickname is not registered",getName());
	return;
     }
   if(!origin.isIdentified(origin.getNickname()))
     {
	origin.sendMessage("Error: You must be identified, and using the nickname you wish to own the channel",getName());
	return;
     }
   int owned = services->getChannel().ownedChannels(origin.getNickname());
   if(owned>0)
     {
	origin.sendMessage("Error: You are only permitted own one channel per nickname on "+Kine::config().getNetworkName(),getName());
	return;
     }
   if(ptr->isRegistered())
     {
	origin.sendMessage("Error: That channel is already registered",getName());
	return;
     }
   services->getChannel().registerChannel(channel,origin.getNickname());
   origin.sendMessage("Registration Successful",getName());
   ptr->log(origin,"Chan","Registered the channel",channel);
   return;
}


  CHAN_FUNC (Module::parseOP)
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
	origin.sendMessage("Usage: op #channel nick [nick2,nick3,..]",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: Cannot find that channel",getName());
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
	     if(fptr==0)
	       return;
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
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
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
	origin.sendMessage("Usage: deop #channel optional list of nicknames to op",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: Cannot find that channel",getName());
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
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
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
	origin.sendMessage("Usage: voice #channel optional list of nicknames to op",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: Could not find that channel",getName());
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
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
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
	origin.sendMessage("Usage: devoice #channel optional list of nicknames to op",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: Cannot find that channel",getName());
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
   origin.sendMessage("Sorry, you do not have enough access in that channel",getName());
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
	origin.sendMessage("Usage: kick #channel nickname Your reason here",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: Cannot find that channel",getName());
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
   origin.sendMessage("You do not have the required access to perform that command",getName());
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
	origin.sendMessage("Usage: access #channel",getName());
	return;
     }
   dChan *ptr = services->findChan(channel);
   if(ptr==0)
     {
	origin.sendMessage("Error: I cannot find that channel",getName());
	return;
     }
 
   if(!ptr->isRegistered())
     {
	origin.sendMessage("Error: That channel is not registered",getName());
	return;
     }
   
   origin.sendMessage("Channel access list for " + channel,
		      getName());
   
   int nbRes =
     services->getDatabase().dbSelect("*", "chanaccess",
				      "chanid=" + 
				      String::convert(ptr->getRegisteredID()));
   
   CResult *myRes = services->getDatabase().dbGetResultSet();
  
   String tnickid, taccess;
   
   for (int i = 0; i < nbRes; i++) {
      String foo = myRes->getValue(i,0);
      String baa = myRes->getValue(i,1);
      String maa = myRes->getValue(i,2);
      origin.sendMessage(foo+" "+baa+" "+maa, getName());
   }
   
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
const Module::moduleInfo_type Module::moduleInfo = {
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
   while (Tables::tables[i] != 0) {
      // Try to affirm this table..
      if (!services->getDatabase().affirmTable(*(Tables::tables[i]))) {
	 services->logLine(String("Unable to affirm mod_chan database "
				  "table '") +
			   Tables::tables[i]->name + "'",
			   Log::Fatality); 
	 return false;
      }
      
      // Next table..
      i++;
   }
   
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
