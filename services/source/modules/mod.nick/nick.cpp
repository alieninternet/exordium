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

#include "nick.h"
#include "tables.h"
#include "language.h"
#include <kineircd/str.h>
#include <kineircd/utils.h>
#include <kineircd/config.h>
#include <exordium/utils.h>
#include <sstream>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::NickModule;

struct Module::functionTableStruct const
  Module::functionTable[] =
{
     {"auth", &Module::parseAUTH},
     {"identify", &Module::parseIDENTIFY},
     {"id", &Module::parseIDENTIFY},
     {"help", &Module::parseHELP},
     {"kill", &Module::parseKILL},
     {"ghost", &Module::parseGHOST},
     {"register", &Module::parseREGISTER},
     {"access", &Module::parseACCESS},
     {"set", &Module::parseSET},
     {"info", &Module::parseINFO},
     {"nick", &Module::parseNICK},
     {"commands", &Module::parseCOMMANDS},
     {0, 0}
};

/* Event Handlers */
void
  Module::handleClientSignon(User& origin)
{

   if(services->isNickRegistered(origin.getNickname()))
     {
	if(!origin.isPending())
	  {
	     if(origin.modNick())
	       {
		  origin.addCheckIdentify();
	       }
	  }
     }
   return;
}


void Module::parseLine(StringTokens& line, User& origin, const bool safe)
{
   String command = line.nextToken().toLower();
   
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Does this match?
      if (command == functionTable[i].command) {
	 // Run the command and leave
	 (this->*(functionTable[i].function))(origin, line, safe);
	 return;
      }
   }
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND, command),
		      getName());
   return;
}

  NICK_FUNC (Module::parseCOMMANDS)
{
  // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength = 200;

   // Send the banner (this shouldn't be hard-coded)
   origin.sendMessage(GETLANG(COMMAND_LIST_START, getName()),
		      getName());
   // Start formulating the data..
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Add the command to the list
      list << " " << functionTable[i].command;

   // How are we for size?
      if (list.str().length() >= lineLength) {
         // Dump it and reset the string stream thingy
         origin.sendMessage(list.str(),getName());
         list.str() = " -=>";
      }
   }

   // Anything left to send still?
   if (list.str().length() > 4) {
      origin.sendMessage(list.str(),getName());
   }

   // Send the footer (this shouldn't be hard-coded)
   origin.sendMessage(GETLANG(COMMAND_LIST_END),
		      getName());


}
  NICK_FUNC (Module::parseNICK)

{
   String command = tokens.nextToken().toLower();
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Does this match?
      if (command == functionTable[i].command) {
         // Run the command and leave
         (this->*(functionTable[i].function))(origin, tokens, safe);
         return;
      }
   }
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND, command),
		      getName());
   return;
}

/* This handles the AUTH command */
NICK_FUNC (Module::parseAUTH)
{
   String gauth = tokens.nextToken();
   if(!origin.isPending())
     {
	origin.sendMessage(GETLANG(nick_ERROR_NICK_NOT_PENDING_CONFIRM),
			   getName());
	return;
     }
   if(origin.getPendingCode()==gauth)
     {
	origin.sendMessage(GETLANG(nick_NICKNAME_CONFIRMED),
			   getName());
	origin.sendMessage(GETLANG(nick_NICKNAME_CONFIRMED_NOW_ID),
			   getName());
	origin.clearPendingCode();
	return;
     }
     else
     {
	origin.sendMessage(GETLANG(nick_ERROR_BROKEN_AUTH_CODE),
			   getName());
	origin.sendMessage(GETLANG(nick_ERROR_CHECK_EMAIL),
			   getName());
	return;
     }
}


/* Info */
  NICK_FUNC (Module::parseINFO)
{
   String who = tokens.nextToken().IRCtoLower();
   if(who=="")
	{
		origin.sendMessage(GETLANG(nick_USAGE_INFO),
				   getName());
		return;
	}
   User *ptr = services->findUser(who);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),
			   getName());
	return;
     }
   if(!origin.isRegistered())
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			   getName());
	return;
     }
   origin.sendMessage(GETLANG(nick_INFO_START, who),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_IDENTIFIED, ptr->getLastID()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_REGISTERED, ptr->getRegDate()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_ICQ, ptr->getICQ()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_MSN, ptr->getMSN()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_AIM, ptr->getAIM()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_URL, ptr->getURL()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_YAHOO, ptr->getYAHOO()),
		      getName());
   origin.sendMessage(GETLANG(nick_INFO_LAST_QUIT_REASON, 
			      ptr->getQuitMessage()),
		      getName());
   std::ostringstream tol;
   
   if(ptr->deopAway()) {
      tol << '{' << GETLANG(nick_INFO_OPTION_DEOPAWAY) << '}';
   }
   
   if(ptr->modNick()) {
      tol << '{' << GETLANG(nick_INFO_OPTION_MODNICK) << '}';
   }
   
   origin.sendMessage(GETLANG(nick_INFO_OPTIONS, tol.str()),
		      getName());
   
   
   if(origin.getAccess("Serv")>0 || origin.getAccess("Oper")>0)
     {
	if(origin.isIdentified(origin.getNickname()))
	  {
	     origin.sendMessage(GETLANG(nick_INFO_STAFF_INFO),
				getName());
	     origin.sendMessage(GETLANG(nick_INFO_LAST_HOST, ptr->getLastHost()),
				getName());
	     origin.sendMessage(GETLANG(nick_INFO_EMAIL, ptr->getEmail()),
				getName());
 	     return;
	  }
     }
}


/* Set */
  NICK_FUNC (Module::parseSET)
{
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   if(!origin.isIdentified(origin.getNickname()))
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),
			   getName());
        return;
     }
   if(command=="")
     {
	origin.sendMessage(GETLANG(nick_SET_OPTIONS_ARE,
				   "password,email,language,modnick,privmsg,"
				   "deopaway"),
			   getName());
	return;
     }
   
   if(command=="pass" || command=="password" || command=="passwd")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_PASS),
				getName());
	     return;
	  }
	if(value.length()<5)
	  {
	     origin.sendMessage(GETLANG(nick_ERROR_PASSWORD_TOO_SHORT, "4"),
				getName());
	     return;
	  }
	origin.setPassword(value);
	origin.sendMessage(GETLANG(nick_PASSWORD_CHANGED, value),
			   getName());
	origin.log(getName(),String("Changed nickname password"));
	return;
     }
   if(command=="modnick")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_MODNICK),
				getName());
	     return;
	  }

	if(value=="true")
	  {
	     origin.setModNick(true);
	     origin.sendMessage(GETLANG(nick_MODNICK_NOW_ON),
				getName());
	     return;
	  }

	if(value=="false")
	  {
	     origin.setModNick(false);
	     origin.sendMessage(GETLANG(nick_MODNICK_NOW_OFF),
				getName());
	     return;
	  }
	origin.sendMessage(GETLANG(ERROR_NOT_BOOLEAN),
			   getName());
	return;
     }

   if(command=="deopaway")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_DEOPAWAY),
				getName());
	     return;
	  }
	if(value=="true")
	  {
	     origin.setDeopAway(true);
	     origin.sendMessage(GETLANG(nick_DEOPAWAY_NOW_ON),
				getName());
	     return;
	  }
	if(value=="false")
	  {
	     origin.setDeopAway(false);
	     origin.sendMessage(GETLANG(nick_DEOPAWAY_NOW_OFF),
				getName());
	     return;
	  }
	origin.sendMessage(GETLANG(ERROR_NOT_BOOLEAN),
			   getName());
	return;
     }

   if(command=="language")
     {
	if(value=="")
	  {
	      origin.sendMessage(GETLANG(nick_USAGE_SET_LANGUAGE),
				 getName());
	     return;
	  }
	if((value=="english") || (value == "en"))
	  {
	     origin.setLanguage("en");
	     origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED, "English"),
				getName());
	     return;
	  }
	if((value=="german") || (value=="deutsch") || (value=="de"))
	   {
	      std::cout << "Setting language to GERMAN!!" << std::endl;
	      origin.setLanguage("de");
	      origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED, "Deutsch"),
				 getName());
	   }
	   
	origin.sendMessage(GETLANG(nick_ERROR_UNSUPPORTED_LANGUAGE),
			   getName());
	return;

     }
   if(command=="email")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_EMAIL),
				getName());
	     return;
	  }
	origin.setEmail(value);
	origin.sendMessage(GETLANG(nick_EMAIL_CHANGED, value),
			   getName());
	origin.log(getName(),"Changed email address to "+value);
	return;
     }

   if(command=="msn")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_MSN),
				getName());
	     return;
	  }
	origin.setInfo("msn",value);
	origin.sendMessage(GETLANG(nick_MSN_CHANGED, value),
			   getName());
        origin.log(getName(),"Changed MSN handle to "+value);
	return;
     }

   if(command=="aim")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_AIM),
				getName());
	     return;
	  }
	origin.setInfo("aim",value);
	origin.sendMessage(GETLANG(nick_AIM_CHANGED, value),
			   getName());
	origin.log(getName(),"Changed AIM handle to "+value);
	return;
     }

   if(command=="yahoo")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_YAHOO),
				getName());
	     return;
	  }
	origin.setInfo("yahoo",value);
	origin.sendMessage(GETLANG(nick_YAHOO_CHANGED, value),
			   getName());
	origin.log(getName(),"Changed Yahoo! handle to "+value);
	return;
     }

   if(command=="icq")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_ICQ),
				getName());
	     return;
	  }
	origin.setInfo("icq",value);
	origin.sendMessage(GETLANG(nick_ICQ_CHANGED, value),
			   getName());
	origin.log(getName(),"Changed ICQ address to "+value);
	return;
     }

   if(command=="url")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_URL),
				getName());
	     return;
	  }
	origin.setInfo("url",value);
	origin.sendMessage(GETLANG(nick_URL_CHANGED, value),
			   getName());
	origin.log(getName(),"Changed URL address to "+value);
	return;
     }
   if(command=="privmsg")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_PRIVMSG),
				getName());
	     return;
	  }
	if(value=="on" || value=="true" || value=="yes")
	  {
	     origin.setPrivmsg(true);
	     origin.sendMessage(GETLANG(nick_PRIVMSG_NOW_ON),
				getName());
	     return;
	  }
	if(value=="off" || value=="false" || value=="no")
	  {
	     origin.setPrivmsg(false);
	     origin.sendMessage(GETLANG(nick_PRIVMSG_NOW_OFF),
				getName());
	     return;
	  }
	origin.sendMessage(GETLANG(ERROR_NOT_BOOLEAN),
			   getName());
	return;
     }
   return;
}


/* Access */
  NICK_FUNC (Module::parseACCESS)
{
   String nickname = tokens.nextToken();
   if(nickname=="")
     {
	origin.sendMessage(GETLANG(nick_USAGE_ACCESS),
			   getName());
	return;
     }
   User *ptr = services->findUser(nickname);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(nick_ERROR_NOT_ONLINE),
			   getName());
	return;
     }
   origin.sendMessage(GETLANG(nick_ACCESS_LIST_START, nickname),
		      getName());
   origin.sendMessage(ptr->getIDList(),getName());
}


/* Register */
  NICK_FUNC (Module::parseREGISTER)
{
   String password = tokens.nextToken();
   String email = tokens.nextToken();
   if(origin.isRegistered())
     {
	origin.sendMessage(GETLANG(nick_ERROR_ALREADY_REGISTERED),
			   getName());
	return;
     }
   if(password=="" | email=="")
     {
	origin.sendMessage(GETLANG(nick_USAGE_REGISTER),
			   getName());
	return;
     }
   if(password.length()<4)
     {
	origin.sendMessage(GETLANG(nick_ERROR_PASSWORD_TOO_SHORT, "4"),
			   getName());
	return;
     }
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_1),
		      getName());
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_2),
		      getName());
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_3),
		      getName());
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_4, email),
		      getName());
   origin.sendMessage(GETLANG(nick_REGISTRATION_THANKYOU),
		      getName());
   origin.registerNick(password,email);
   origin.log(getName(),"Registered nickname with email "+email);
   String authcode = origin.genAuth();
   String subject = "Your nickname registration";
   String emailtext =
     "This is a sample email - Needs replacing with something much better\n"
     "\n"
     "\n"
     "In order to successfully register your nickname, you need to type\n"
     "the following next time you are on "+Kine::config().getNetworkName()+".\n"
     "\n"
     "\n"
     "/msg nick AUTH "+authcode+"\n"
     "\n";
   services->sendEmail(email,subject,emailtext);
}


/* Kill */
  NICK_FUNC (Module::parseKILL)
{
   String tokill = tokens.nextToken();
   String password = tokens.nextToken();
   if(tokill=="" | password=="")
     {
	origin.sendMessage(GETLANG(nick_USAGE_KILL),
			   getName());
	return;
     }
   User *ptr = services->findUser(tokill);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(nick_ERROR_NOT_ONLINE),
			   getName());
	return; /* yoda says so yes yes yes */
     }
     else
     { 
	if(ptr->isPending())
	  {
	     origin.sendMessage(GETLANG(nick_ERROR_NICK_PENDING_NO_KILL),
				getName());
	     return;
	  }
	if(!ptr->isRegistered())
	  {
	     origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
				getName());
	     return;
	  }
	if(ptr->getPass() == password)
	  {
	     services->killnick(tokill,getName(),"Kill requested by "+origin.getNickname());
	     origin.log(getName(),"Requested a kill on "+tokill);
	     return;
	  }
	origin.sendMessage(GETLANG(nick_ERROR_INCORRECT_PASSWORD),
			   getName());
//	int access = origin.getAccess("Serv");
	if(ptr->getAccess("Serv")>0 || ptr->getAccess("Oper")>0)
	  {
	     services->sendHelpme(getName(),String("\002Failed\002 kill for nickname ")+origin.getNickname()+" by \002"+origin.getNickname()+"!"+origin.getIdent()+"@"+origin.getHost());
	  }
     }
}


/* Do help... */
NICK_FUNC (Module::parseHELP)
{
 String word = tokens.nextToken();
 String parm = tokens.nextToken();
 services->doHelp(origin,getName(), word, parm);
}


/* Ghost... */
NICK_FUNC (Module::parseGHOST)
{
  String toghost = tokens.nextToken();
  String password = tokens.nextToken();
  if(toghost=="" | password=="")
   {
	origin.sendMessage(GETLANG(nick_USAGE_GHOST),
			   getName());
	return;
   }
  if (!services->isNickRegistered( toghost))
    {
       origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			  getName());
       return;
    }
   if(Utils::generatePassword(toghost,password) == password)
    {
       services->registerService(toghost,"ghost","ghosts.peoplechat.org",
					"Ghosted by "+origin.getNickname());
       origin.sendMessage(GETLANG(nick_GHOST_SUCCESSFUL, toghost),
			  getName());
       origin.log(getName(), "Successfully ghosted " + toghost);
       return;
    }
  origin.log(getName(),String("Failed ghost for "+toghost));
  origin.sendMessage(GETLANG(nick_ERROR_INCORRECT_PASSWORD),
		     getName());
  return;
}


/* Parse an identification request */
NICK_FUNC (Module::parseIDENTIFY)
{
  String password = tokens.nextToken();
  if (origin.isPending())
    {
       origin.sendMessage(GETLANG(nick_ERROR_NICK_PENDING_NO_IDENTIFY),
			  getName());
       return;
    }
  if (!origin.isRegistered())
    {
       origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			  getName());
       return;
    }
  if(!safe)
    {
       origin.sendMessage(GETLANG(nick_ERROR_INSECURE_MESSAGE),
			  getName());
       return;
     }
   if(Utils::generatePassword(origin.getNickname(),password) == origin.getPass())
    {
       origin.updateIdentified();
       std::cout << "Tag thing: " << Language::tagMap[Language::nick_IDENTIFY_SUCCESSFUL].tagID << std::endl;
       
       origin.sendMessage(GETLANG(nick_IDENTIFY_SUCCESSFUL),
			  getName());
       return;
    }
  origin.sendMessage(GETLANG(nick_ERROR_INCORRECT_PASSWORD),
		     getName());
  origin.log(getName(),"Failed identify for nickname "+origin.getNickname()+"!"+origin.getIdent()+"@"+origin.getHost());
  if(origin.getAccess("Serv")>0 || origin.getAccess("Oper")>0)
    {
       services->sendHelpme("Serv","Failed identify for nickname "+origin.getNickname()+"!"+origin.getIdent()+"@"+origin.getHost());
    }
  return;
}

EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }

// Module information structure
const Module::moduleInfo_type Module::moduleInfo =
{
   "Nickname Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::CLIENT_SIGNON |
     Exordium::Service::moduleInfo_type::Events::CLIENT_NICKCHANGE
};

// Start the service
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;
   
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

   // Register and process our language tag name -> tag ID map
   Kine::langs().registerMap(Language::tagMap);
   
   // temporary debugging thing :(
   int foofoo = 0;
   for (;;) {
      std::cout << "TagMap " << foofoo << ": tag '" <<
	Language::tagMap[foofoo].tagName << "' affirmed as TID # " <<
	Language::tagMap[foofoo].tagID << std::endl;
      
      if (Language::tagMap[++foofoo].tagName == 0) {
	 break;
      }
   }
   
   // Register ourself to the network
   services->registerService(getName(), getName(), 
			     getConfigData().getHostname(),
			     getConfigData().getDescription());
   
   // We started okay :)
   return true;
}
