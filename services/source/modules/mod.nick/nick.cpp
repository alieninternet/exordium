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
#include "commands.h"
#include <kineircd/config.h>
#include <kineircd/languages.h>
#include <exordium/utils.h>
#include <sstream>
#include <aisutil/utils.h>

using AISutil::String;
using AISutil::StringTokens;
using AISutil::Utils;
using namespace Exordium::NickModule;

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
   
   for (int i = 0; Commands::commandTable[i].command != 0; i++) {
      // Does this match?
      if (command == Commands::commandTable[i].command) {
	 // Run the command and leave
	 (this->*(Commands::commandTable[i].function))(origin, line, safe);
	 return;
      }
   }
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND, command),
		      getNickname());
   return;
}

  NICK_FUNC (Module::parseCOMMANDS)
{
  // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength = 200;

   // Send the banner 
   origin.sendMessage(GETLANG(COMMAND_LIST_START, getNickname()),
		      getNickname());
   // Start formulating the data..
   std::ostringstream list(" -=>");
   for (int i = 0; Commands::commandTable[i].command != 0; i++) {
      // Add the command to the list
      list << " " << Commands::commandTable[i].command;

   // How are we for size?
      if (list.str().length() >= lineLength) {
         // Dump it and reset the string stream thingy
         origin.sendMessage(list.str(),getNickname());
         list.str() = " -=>";
      }
   }

   // Anything left to send still?
   if (list.str().length() > 4) {
      origin.sendMessage(list.str(),getNickname());
   }

   // Send the footer
   origin.sendMessage(GETLANG(COMMAND_LIST_END),
		      getNickname());


}
  NICK_FUNC (Module::parseNICK)

{
   String command = tokens.nextToken().toLower();
   for (int i = 0; Commands::commandTable[i].command != 0; i++) {
      // Does this match?
      if (command == Commands::commandTable[i].command) {
         // Run the command and leave
         (this->*(Commands::commandTable[i].function))(origin, tokens, safe);
         return;
      }
   }
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND, command),
		      getNickname());
   return;
}

/* This handles the AUTH command */
NICK_FUNC (Module::parseAUTH)
{
   String gauth = tokens.nextToken();
   if(!origin.isPending())
     {
	origin.sendMessage(GETLANG(nick_ERROR_NICK_NOT_PENDING_CONFIRM),
			   getNickname());
	return;
     }
   if(origin.getPendingCode()==gauth)
     {
	origin.sendMessage(GETLANG(nick_NICKNAME_CONFIRMED),
			   getNickname());
	origin.sendMessage(GETLANG(nick_NICKNAME_CONFIRMED_NOW_ID),
			   getNickname());
	origin.clearPendingCode();
	return;
     }
     else
     {
	origin.sendMessage(GETLANG(nick_ERROR_BROKEN_AUTH_CODE),
			   getNickname());
	origin.sendMessage(GETLANG(nick_ERROR_CHECK_EMAIL),
			   getNickname());
	return;
     }
}


/* Info */
  NICK_FUNC (Module::parseINFO)
{
   Kine::ClientName who = tokens.nextToken();
   who = who.IRCtoLower();
   if(who.empty())
	{
		origin.sendMessage(GETLANG(nick_USAGE_INFO),
				   getNickname());
		return;
	}
   User *ptr = services->findUser(who);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),
			   getNickname());
	return;
     }
   if(!origin.isRegistered())
     {
	origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			   getNickname());
	return;
     }
   origin.sendMessage(GETLANG(nick_INFO_START, who),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_IDENTIFIED, ptr->getLastID()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_REGISTERED, ptr->getRegDate()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_ICQ, ptr->getICQ()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_MSN, ptr->getMSN()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_AIM, ptr->getAIM()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_URL, ptr->getURL()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_YAHOO, ptr->getYAHOO()),
		      getNickname());
   origin.sendMessage(GETLANG(nick_INFO_LAST_QUIT_REASON, 
			      ptr->getQuitMessage()),
		      getNickname());
   std::ostringstream tol;
   
   if(ptr->deopAway()) {
      tol << '{' << GETLANG(nick_INFO_OPTION_DEOPAWAY) << '}';
   }
   
   if(ptr->modNick()) {
      tol << '{' << GETLANG(nick_INFO_OPTION_MODNICK) << '}';
   }
   
   origin.sendMessage(GETLANG(nick_INFO_OPTIONS, tol.str()),
		      getNickname());
   origin.log(getNickname(),"Requested Nickname information on "+ptr->getNickname());
   
   
   if(origin.getAccess("Serv")>0 || origin.getAccess("Oper")>0)
     {
	if(origin.isIdentified(origin.getNickname()))
	  {
	     origin.sendMessage(GETLANG(nick_INFO_STAFF_INFO),
				getNickname());
	     origin.sendMessage(GETLANG(nick_INFO_LAST_HOST, ptr->getLastHost()),
				getNickname());
	     origin.sendMessage(GETLANG(nick_INFO_EMAIL, ptr->getEmail()),
				getNickname());
	     origin.log(getNickname(),"Was sent staff information with nick information report on "+ptr->getNickname());
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
			   getNickname());
        return;
     }
   if(command=="")
     {
	origin.sendMessage(GETLANG(nick_SET_OPTIONS_ARE,
				   "password,email,language,modnick,privmsg,"
				   "deopaway"),
			   getNickname());
	return;
     }
   
   if(command=="pass" || command=="password" || command=="passwd")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_PASS),
				getNickname());
	     return;
	  }
	if(value.length()<5)
	  {
	     origin.sendMessage(GETLANG(nick_ERROR_PASSWORD_TOO_SHORT, "4"),
				getNickname());
	     return;
	  }
	origin.setPassword(value);
	origin.sendMessage(GETLANG(nick_PASSWORD_CHANGED, value),
			   getNickname());
	origin.log(getNickname(),String("Changed nickname password to [HIDDEN]"));
	return;
     }
   if(command=="modnick")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_MODNICK),
				getNickname());
	     return;
	  }

	if(AISutil::Utils::toBool(value)==1)
	  {
	     origin.setModNick(true);
	     origin.sendMessage(GETLANG(nick_MODNICK_NOW_ON),
				getNickname());
	     origin.log(getNickname(),"Changed nickname enforcement to enabled");
	     return;
	  }

	if(AISutil::Utils::toBool(value)==0)
	  {
	     origin.setModNick(false);
	     origin.sendMessage(GETLANG(nick_MODNICK_NOW_OFF),
				getNickname());
	     origin.log(getNickname(),"Changed nickname enforcement to disabled");
	     return;
	  }
	origin.sendMessage(GETLANG(ERROR_NOT_BOOLEAN),
			   getNickname());
	return;
     }

   if(command=="deopaway")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_DEOPAWAY),
				getNickname());
	     return;
	  }
	if(AISutil::Utils::toBool(value)==1)
	  {
	     origin.setDeopAway(true);
	     origin.sendMessage(GETLANG(nick_DEOPAWAY_NOW_ON),
				getNickname());
	     origin.log(getNickname(),"Changed deoponaway to enabled");
	     return;
	  }
	if(AISutil::Utils::toBool(value)==0)
	  {
	     origin.setDeopAway(false);
	     origin.sendMessage(GETLANG(nick_DEOPAWAY_NOW_OFF),
				getNickname());
	     origin.log(getNickname(),"Changed deoponaway to disabled");
	     return;
	  }
	origin.sendMessage(GETLANG(ERROR_NOT_BOOLEAN),
			   getNickname());
	return;
     }

   if (command == "language") {
      // Was a language actually specified?
      if (value.empty()) {
	 // Tell the user how to use this command
	 origin.sendMessage(GETLANG(nick_USAGE_SET_LANGUAGE),
			    getNickname());
	 
	 // Compile a list of known language codes the user can use
	 std::ostringstream output;

	 /* Well, first off, the default language code ("i-default") is 
	  * normally known. See RFC 2277 for details.
	  */
	 if (Kine::langs().getDefaultLanguage() != 0) {
	    output << '\037' << Kine::Languages::defaultLanguageCode << 
	      "\037 (" <<
	      Kine::langs().getDefaultLanguage()->getLanguageName() << ')';
	 }
	 
	 // Okay, iterate over the known languages
	 for (Kine::Languages::languageDataMap_type::const_iterator it =
	      Kine::langs().getLanguageDataMap().begin();
	      it != Kine::langs().getLanguageDataMap().end();
	      ++it) {
	    /* If we need to, prefix the output with a comma. This method of
	     * list separation isn't very internationalised, but in the scope
	     * of internationalisation of IRC services, does anyone outside of
	     * western culture not know what a comma delimetered list looks
	     * like *and* is on the internet?
	     */
	    if (!output.str().empty()) {
	       output << ", ";
	    }
	    
	    // Output the language code + its name
	    output << '\037' << it->second->getLanguageCode() << "\037 (" <<
	      it->second->getLanguageName() << ')';
	 }
	 
	 /* Okay, finally, show the user the list. BTW, this will definately
	  * break if/when the language list grows to such a size that the
	  * length of this string is longer than the allowable PRIVMSG/NOTICE
	  * payload size (~ 400 chars, depending on the protocol)
	  */
	 origin.sendMessage(GETLANG(nick_KNOWN_LANGUAGE_LIST,
				    output.str()),
			    getNickname());
	 
	 return;
      }
      
      // If we're going to look up the language (most likely) we need this..
      const Kine::Languages::LanguageData* lang = 0;

      /* If the user wants to use the "default language", let them use it
       * without going to very much effort at all..
       * 
       * The current default code is "i-default", as per RFC 2277, but we use
       * this constant just in case that changes (so we don't have to change).
       */
      if (value == Kine::Languages::defaultLanguageCode) {
	 origin.setLanguage(Kine::Languages::defaultLanguageCode);
	 return;
      } else {
	 // Okay, let's try to look up the language, presuming it's a lang code
	 if ((lang = Kine::langs().findByCode(value)) == 0) {
	    // Okay, no other option but to whinge
	    origin.sendMessage(GETLANG(nick_ERROR_UNSUPPORTED_LANGUAGE),
			       getNickname());
	 }
	 
	 // Okay, so we found a language, set the language appropriately
	 origin.setLanguage(lang->getLanguageCode());
      }
      
      // Tell the user about their new language setting
      origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED,
				 lang->getLanguageName()),
			 getNickname());
      
      // Also, log this information..
      std::ostringstream loginfo;
      loginfo << 
	"Change interface language for " << origin.getNickname() <<
	" to '" << lang->getLanguageName() << "' (" << 
	lang->getLanguageCode() << ')';
      origin.log(getNickname(), loginfo.str());

// 	if((value=="english") || (value == "en"))
// 	  {
// 	     origin.setLanguage("en");
// 	     origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED, "English"),
//				getNickname());
//	     origin.log(getNickname(),"Changed language interface to English");
//	     return;
//	  }
//	if((value=="german") || (value=="deutsch") || (value=="de"))
//	   {
//	      origin.setLanguage("de");
//	      origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED, "Deutsch"),
//				 getNickname());
//	      origin.log(getNickname(),"Changed language interface to German");
//	      return;
//	   }
//	if((value=="norsk") || (value=="norwegian") || (value=="no"))
//	  {
//	     origin.setLanguage("no");
//	     origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED, "Norske"),
//				getNickname());
//	     origin.log(getNickname(),"Changed language interface to Norweigan");
//	     return;
//	  }
//	                                         
//	if((value=="turkish") || (value=="tr") || 
//	   (value=="turkiye")) // <=- that's the country name, not the language
//	  {
//	     origin.setLanguage("tr");
//	     origin.sendMessage(GETLANG(nick_LANGUAGE_CHANGED, "Turkiye"),
//	                        // ^^^ again with the country name, james..
//	                        // Britain != British, right?
//				getNickname());
//	     origin.log(getNickname(),"Changed language interface to Turkish");
//	     return;
//	  }
	return;
     }
   
   if(command=="email")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_EMAIL),
				getNickname());
	     return;
	  }
	origin.setEmail(value);
	origin.sendMessage(GETLANG(nick_EMAIL_CHANGED, value),
			   getNickname());
	origin.log(getNickname(),"Changed email address to "+value);
	return;
     }

   if(command=="msn")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_MSN),
				getNickname());
	     return;
	  }
	origin.setInfo("msn",value);
	origin.sendMessage(GETLANG(nick_MSN_CHANGED, value),
			   getNickname());
        origin.log(getNickname(),"Changed MSN handle to "+value);
	return;
     }

   if(command=="aim")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_AIM),
				getNickname());
	     return;
	  }
	origin.setInfo("aim",value);
	origin.sendMessage(GETLANG(nick_AIM_CHANGED, value),
			   getNickname());
	origin.log(getNickname(),"Changed AIM handle to "+value);
	return;
     }

   if(command=="yahoo")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_YAHOO),
				getNickname());
	     return;
	  }
	origin.setInfo("yahoo",value);
	origin.sendMessage(GETLANG(nick_YAHOO_CHANGED, value),
			   getNickname());
	origin.log(getNickname(),"Changed Yahoo! handle to "+value);
	return;
     }

   if(command=="icq")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_ICQ),
				getNickname());
	     return;
	  }
	origin.setInfo("icq",value);
	origin.sendMessage(GETLANG(nick_ICQ_CHANGED, value),
			   getNickname());
	origin.log(getNickname(),"Changed ICQ address to "+value);
	return;
     }

   if(command=="url")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_URL),
				getNickname());
	     return;
	  }
	origin.setInfo("url",value);
	origin.sendMessage(GETLANG(nick_URL_CHANGED, value),
			   getNickname());
	origin.log(getNickname(),"Changed URL address to "+value);
	return;
     }
   if(command=="privmsg")
     {
	if(value=="")
	  {
	     origin.sendMessage(GETLANG(nick_USAGE_SET_PRIVMSG),
				getNickname());
	     return;
	  }
	if(AISutil::Utils::toBool(value)==1)
	  {
	     origin.setPrivmsg(true);
	     origin.sendMessage(GETLANG(nick_PRIVMSG_NOW_ON),
				getNickname());
	     origin.log(getNickname(),"Changed privmsg interface to enabled");
	     return;
	  }
	if(AISutil::Utils::toBool(value)==0)
	  {
	     origin.setPrivmsg(false);
	     origin.sendMessage(GETLANG(nick_PRIVMSG_NOW_OFF),
				getNickname());
	     origin.log(getNickname(),"Changed privmsg interface to disabled");
	     return;
	  }
	origin.sendMessage(GETLANG(ERROR_NOT_BOOLEAN),
			   getNickname());
	return;
     }
   return;
}


/* Access */
  NICK_FUNC (Module::parseACCESS)
{
   Kine::ClientName nickname = tokens.nextToken();
   if(nickname=="")
     {
	origin.sendMessage(GETLANG(nick_USAGE_ACCESS),
			   getNickname());
	return;
     }
   User *ptr = services->findUser(nickname);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(nick_ERROR_NOT_ONLINE),
			   getNickname());
	return;
     }
   origin.sendMessage(GETLANG(nick_ACCESS_LIST_START, nickname),
		      getNickname());
   origin.sendMessage(ptr->getIDList(),getNickname());
   origin.log(getNickname(),"Requested an access list on "+ptr->getNickname());
}


/* Register */
  NICK_FUNC (Module::parseREGISTER)
{
   String password = tokens.nextToken();
   String email = tokens.nextToken();
   if(origin.isRegistered())
     {
	origin.sendMessage(GETLANG(nick_ERROR_ALREADY_REGISTERED),
			   getNickname());
	return;
     }
   if(password=="" | email=="")
     {
	origin.sendMessage(GETLANG(nick_USAGE_REGISTER),
			   getNickname());
	return;
     }
   if(password.length()<4)
     {
	origin.sendMessage(GETLANG(nick_ERROR_PASSWORD_TOO_SHORT, "4"),
			   getNickname());
	return;
     }
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_1),
		      getNickname());
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_2),
		      getNickname());
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_3),
		      getNickname());
   origin.sendMessage(GETLANG(nick_REGISTRATION_EMAIL_NOTICE_4, email),
		      getNickname());
   origin.sendMessage(GETLANG(nick_REGISTRATION_THANKYOU),
		      getNickname());
   origin.registerNick(password,email);
   origin.log(getNickname(),"Registered nickname with email "+email);
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
   Kine::ClientName tokill = tokens.nextToken();
   String password = tokens.nextToken();
   if(tokill=="" | password=="")
     {
	origin.sendMessage(GETLANG(nick_USAGE_KILL),
			   getNickname());
	return;
     }
   User *ptr = services->findUser(tokill);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(nick_ERROR_NOT_ONLINE),
			   getNickname());
	return; /* yoda says so yes yes yes */
     }
     else
     { 
	if(ptr->isPending())
	  {
	     origin.sendMessage(GETLANG(nick_ERROR_NICK_PENDING_NO_KILL),
				getNickname());
	     return;
	  }
	if(!ptr->isRegistered())
	  {
	     origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
				getNickname());
	     return;
	  }
	if(ptr->getPass() == password)
	  {
	     services->killnick(tokill,getNickname(),"Kill requested by "+origin.getNickname());
	     origin.log(getNickname(),"Requested a kill on "+tokill);
	     return;
	  }
	origin.sendMessage(GETLANG(nick_ERROR_INCORRECT_PASSWORD),
			   getNickname());
//	int access = origin.getAccess("Serv");
	if(ptr->getAccess("Serv")>0 || ptr->getAccess("Oper")>0)
	  {
	     services->sendHelpme(getNickname(),String("\002Failed\002 kill for nickname ")+origin.getNickname()+" by \002"+origin.getNickname()+"!"+origin.getIdent()+"@"+origin.getHost());
	  }
     }
}


/* Do help... */
NICK_FUNC (Module::parseHELP)
{
// String word = tokens.nextToken();
// String parm = tokens.nextToken();
// services->doHelp(origin,getNickname(), word, parm);

   String command = tokens.nextToken().toLower();
   
   // this should be elsewhere.
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
   if(command.empty())
     {
	// Basic help.
	Callout callout(origin, getNickname());
	Kine::langs().get(origin.getLanguage(),Language::tagMap[Language::nick_HELP].tagID,callout);
	return;
	
     }
   
   // find the command
   for (int i = 0; Commands::commandTable[i].command != 0; i++) {
      // Does this match?
      if (command == Commands::commandTable[i].command) {
	 // this also should be elsewhere.
	 Callout callout(origin, getNickname());
	 
	 // Subcommand?
	 String subcommand = tokens.nextToken().toLower();
	 
	 if (!subcommand.empty() &&
	     (Commands::commandTable[i].subcommands != 0)) {
	    // Look for the subcommand info..
	    for (int j = 0; 
		 (*Commands::commandTable[i].subcommands)[j].command != 0;
		 j++) {
	       // match?
	       if (subcommand == 
		   (*Commands::commandTable[i].subcommands)[j].command) {
		  // Okay, send this..
		  Kine::langs().get(origin.getLanguage(),
				    *(*Commands::commandTable[i].subcommands)[j].help,
				    callout);
		  return;
	       }
	    }
	 }

	 // send the help appropriately
	 Kine::langs().get(origin.getLanguage(),
			   *Commands::commandTable[i].help,
			   callout);
	 
	 // Leave.
	 return;
      }
   }
   
   // kinda the wrong message, but good enough for now
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND, command),
		      getNickname());
   
}


/* Ghost... */
NICK_FUNC (Module::parseGHOST)
{
  String toghost = tokens.nextToken();
  String password = tokens.nextToken();
  if(toghost=="" | password=="")
   {
	origin.sendMessage(GETLANG(nick_USAGE_GHOST),
			   getNickname());
	return;
   }
  if (!services->isNickRegistered( toghost))
    {
       origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			  getNickname());
       return;
    }
   if(Utils::generatePassword(toghost,password) == password)
    {
       services->registerService(toghost,"ghost","ghosts.peoplechat.org",
					"Ghosted by "+origin.getNickname());
       origin.sendMessage(GETLANG(nick_GHOST_SUCCESSFUL, toghost),
			  getNickname());
       origin.log(getNickname(), "Successfully ghosted " + toghost);
       return;
    }
  origin.log(getNickname(),String("Failed ghost for "+toghost));
  origin.sendMessage(GETLANG(nick_ERROR_INCORRECT_PASSWORD),
		     getNickname());
  return;
}


/* Parse an identification request */
NICK_FUNC (Module::parseIDENTIFY)
{
  String password = tokens.nextToken();
  if (origin.isPending())
    {
       origin.sendMessage(GETLANG(nick_ERROR_NICK_PENDING_NO_IDENTIFY),
			  getNickname());
       return;
    }
  if (!origin.isRegistered())
    {
       origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			  getNickname());
       return;
    }
  if(!safe)
    {
       origin.sendMessage(GETLANG(nick_ERROR_INSECURE_MESSAGE),
			  getNickname());
       return;
     }
   if(Utils::generatePassword(origin.getNickname(),password) == origin.getPass())
    {
       origin.updateIdentified();
       std::cout << "Tag thing: " << Language::tagMap[Language::nick_IDENTIFY_SUCCESSFUL].tagID << std::endl;
       
       origin.sendMessage(GETLANG(nick_IDENTIFY_SUCCESSFUL),
			  getNickname());
       return;
    }
  origin.sendMessage(GETLANG(nick_ERROR_INCORRECT_PASSWORD),
		     getNickname());
  origin.log(getNickname(),"Failed identify for nickname "+origin.getNickname()+"!"+origin.getIdent()+"@"+origin.getHost());
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
   services->registerService(getNickname(), getUsername(), 
			     getHostname(), getDescription());
   
   // We started okay :)
   return true;
}


// Stop the service
void Module::stop(const String* const reason)
{
   Kine::langs().deregisterMap(Language::tagMap);
   services->serviceQuit(getNickname(), ((reason == 0) ? "" : *reason));
}


/** nickLinkAdd(String,String)
 * 
 * Add a link between the two nicknames.
 * 
 * 
 */

void Module::nickLinkAdd(String const &first, String const &second)
{
   services->getDatabase().dbInsert("nicklinks","'"+String::convert(services->getStatic().getRegisteredNickID(first))
				   +"','"+String::convert(services->getStatic().getRegisteredNickID(second))+"'");
}

/** nickLinkDel(String,String)
 * 
 * Remove a link between the two nicknames.
 * 
 */

void Module::nickLinkDel(String const &first, String const &second)
{
   services->getDatabase().dbDelete("nicklinks","fromnick='"+String::convert(services->getStatic().getRegisteredNickID(first))
				   +"','"+String::convert(services->getStatic().getRegisteredNickID(second))+"'");
}
