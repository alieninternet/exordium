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

#include "nick.h"
#include "tables.h"
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
   
   origin.sendMessage("Unrecognised Command", getName());
   return;
}

  NICK_FUNC (Module::parseCOMMANDS)
{
  // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength = 200;

   // Send the banner (this shouldn't be hard-coded)
   origin.sendMessage("Command list for " + getName() + ":",getName());
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
   origin.sendMessage("End of command list",getName());


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
   origin.sendMessage("Unrecognised Command", getName());
   return;
}

/* This handles the AUTH command */
NICK_FUNC (Module::parseAUTH)
{
   String gauth = tokens.nextToken();
   if(!origin.isPending())
     {
	origin.sendMessage("Error: This nickname is not pending confirmation",getName());
	return;
     }
   if(origin.getPendingCode()==gauth)
     {
	origin.sendMessage("Congratulations you have confirmed your nickname. You can now use services freely",getName());
	origin.sendMessage("You may now identify your nickname as normal",getName());
	origin.clearPendingCode();
	return;
     }
     else
     {
	origin.sendMessage("Error: The auth code you gave does not match our records",getName());
	origin.sendMessage("Please re-check the email you received from services",getName());
	return;
     }
}


/* Info */
  NICK_FUNC (Module::parseINFO)
{
   String who = tokens.nextToken().IRCtoLower();
   if(who=="")
	{
		origin.sendMessage("Usage: info nickname",getName());
		return;
	}

   if(who=="")
     {
	origin.sendMessage("Usage: /msg nick info nickname",getName());
	return;
     }
   User *ptr = services->findUser(who);
   if(ptr==0)
     {
	origin.sendMessage("Error: Could not locate that user",getName());
	return;
     }
   if(!origin.isRegistered())
     {
	origin.sendMessage("Error: That nickname is not registered",getName());
	return;
     }

   int saccess = origin.getAccess("Serv");
   int oaccess = origin.getAccess("Oper");
   if(saccess>0 || oaccess>0)
     {
	if(origin.isIdentified(origin.getNickname()))
	  {
	     bool deopAway = ptr->deopAway();
	     bool modNick = ptr->modNick();
	     std::ostringstream tol;
	     tol << "Options : ";
	     if(deopAway)
	       {
		  tol << " Deop on Away";
	       }
	     if(modNick)
	       {
		  tol << ", Nickname Enforcement";
	       }

	     origin.sendMessage("Nickname Information for \002"+who,getName());
	     origin.sendMessage("Last host : \002"+ptr->getLastHost(),getName());
	     origin.sendMessage("Last identified : \002"+ptr->getLastID(),getName());
	     origin.sendMessage("Registered : \002"+ptr->getRegDate(),getName());
	     origin.sendMessage("Email : \002"+ptr->getEmail(),getName());
	     origin.sendMessage("ICQ : \002"+ptr->getICQ(),getName());
	     origin.sendMessage("MSN : \002"+ptr->getMSN(),getName());
	     origin.sendMessage("AIM : \002"+ptr->getAIM(),getName());
	     origin.sendMessage("URL : \002"+ptr->getURL(),getName());
	     origin.sendMessage("Yahoo! : \002"+ptr->getYAHOO(),getName());
	     origin.sendMessage("Last Quit Message : \002"+ptr->getQuitMessage(),getName());
	     origin.sendMessage(tol.str(),getName());
	     return;
	  }
     }
	     origin.sendMessage("Nickname Information for \002"+who,getName());
	     origin.sendMessage("Last host : \002"+ptr->getLastHost(),getName());
	     origin.sendMessage("Last identified : \002"+ptr->getLastID(),getName());
	     origin.sendMessage("Registered : \002"+ptr->getRegDate(),getName());
	     origin.sendMessage("Email : \002"+ptr->getEmail(),getName());
	     origin.sendMessage("ICQ : \002"+ptr->getICQ(),getName());
	     origin.sendMessage("MSN : \002"+ptr->getMSN(),getName());
	     origin.sendMessage("AIM : \002"+ptr->getAIM(),getName());
	     origin.sendMessage("URL : \002"+ptr->getURL(),getName());
	     origin.sendMessage("Yahoo! : \002"+ptr->getYAHOO(),getName());
	     origin.sendMessage("Last Quit Message : \002"+ptr->getQuitMessage(),getName());


}


/* Set */
  NICK_FUNC (Module::parseSET)
{
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   if(!origin.isIdentified(origin.getNickname()))
     return;
   if(command=="pass" || command=="password" || command=="passwd")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set pass newpassword";
	     origin.sendMessage(togo,getName());
	     return;
	  }
	if(value.length()<5)
	  {
	     String togo = "Error: New password must be greater than 4 letters";
	     origin.sendMessage(togo,getName());
	     return;
	  }
	String newhash = Utils::generatePassword(origin.getNickname(), value);
        services->getDatabase().dbUpdate("nicks", "password='"+newhash+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "Password has been successfully changed to "+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),String("Changed nickname password"));
	return;
     }
   if(command=="modnick")
     {
	if(value=="")
	  {
	     String togo = "Usage is set modnick true/false";
	     origin.sendMessage(togo,getName());
	  }

	if(value=="true")
	  {
	     origin.setModNick(true);
	     String togo = "Nickname enforcement is now enabled";
	     origin.sendMessage(togo,getName());
	     return;
	  }

	if(value=="false")
	  {
	     origin.setModNick(false);
	     String togo = "Nickname enforcement is now disabled";
	     origin.sendMessage(togo,getName());
	     return;
	  }
	origin.sendMessage("Error: Value must be true or false",getName());
	return;

     }

   if(command=="deopaway")
     {
	if(value=="")
	  {
	     String togo = "Usage is set deopaway true/false";
	     origin.sendMessage(togo,getName());
	  }
	if(value=="true")
	  {
	     origin.setDeopAway(true);
	     String togo = "You will now be automatically deoped on setting away";
	     origin.sendMessage(togo,getName());
	     return;
	  }
	if(value=="false")
	  {
	     origin.setDeopAway(false);
	     String togo = "You will no longer be automatically deoped on setting away";
	     origin.sendMessage(togo,getName());
	     return;
	  }
	origin.sendMessage("Error: Value must be true or false",getName());
	return;
     }

   if(command=="language")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set language LANGUAGE";
	     origin.sendMessage(togo,getName());
	     return;
	  }
	if(value=="english")
	  {
	     origin.setLanguage("english");
	     origin.sendMessage("Helpfiles interface has been changed to english",getName());
	     return;
	  }
	origin.sendMessage("Error: Unsupported Language!",getName());
	return;

     }
   if(command=="email")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set email email@Address";
	     origin.sendMessage(togo,getName());
	     return;
	  }
        services->getDatabase().dbUpdate("nicks", "email='"+value+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "Email has been changed to \002"+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),"Changed email address to "+value);
	return;
     }

   if(command=="msn")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set msn MSN handle";
	     origin.sendMessage(togo,getName());
	     return;
	  }
        services->getDatabase().dbUpdate("nicks", "msn='"+value+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "MSN has been changed to \002"+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),"Changed MSN address to "+value);
	return;
     }

   if(command=="aim")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set aim handle";
	     origin.sendMessage(togo,getName());
	     return;
	  }
        services->getDatabase().dbUpdate("nicks", "aim='"+value+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "AIM has been changed to \002"+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),"Changed AIM address to "+value);
	return;
     }

   if(command=="yahoo")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set yahoo! handle";
	     origin.sendMessage(togo,getName());
	     return;
	  }
        services->getDatabase().dbUpdate("nicks", "yahoo='"+value+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "Yahoo! has been changed to \002"+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),"Changed Yahoo! address to "+value);
	return;
     }

   if(command=="icq")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set icq ICQNUMBER";
	     origin.sendMessage(togo,getName());
	     return;
	  }
        services->getDatabase().dbUpdate("nicks", "icq='"+value+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "ICQ has been changed to \002"+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),"Changed ICQ address to "+value);
	return;
     }

   if(command=="url")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set url www.peoplechat.org";
	     origin.sendMessage(togo,getName());
	     return;
	  }
        services->getDatabase().dbUpdate("nicks", "url='"+value+"'", "nickname='"+origin.getNickname()+"'");
	String togo = "URL has been changed to \002"+value;
	origin.sendMessage(togo,getName());
	services->log(origin,getName(),"Changed URL address to "+value);
	return;
     }
   if(command=="privmsg")
     {
	if(value=="on")
	  {
             services->getDatabase().dbUpdate("nicks", "privmsg=1", "nickname='"+origin.getNickname()+"'");
	     origin.sendMessage("I will now use the private message interface",getName());
	     return;
	  }
	if(value=="off")
	  {
             services->getDatabase().dbUpdate("nicks", "privmsg=0", "nickname='"+origin.getNickname()+"'");
	     origin.sendMessage("I will now use the private message interface",getName());
	     return;
	  }
	origin.sendMessage("Error: Value must be on or off",getName());
	return;
     }
   origin.sendMessage("Error: Unsupported command",getName());
   return;
}


/* Access */
  NICK_FUNC (Module::parseACCESS)
{
   String nickname = tokens.nextToken();
   if(nickname!="")
     {
        int nbRes = services->getDatabase().dbSelect("idas", "nicksidentified", "nick='"+origin.getOnlineIDString()+"'");

	int i=0;
        for (int j=0; j<nbRes; j++)
	  {
	     i++;
	     String id = services->getDatabase().dbGetValue();;
	     String idnick = services->getNick(id.toInt());
	     String tosend = String("\002")+String::convert(i)+"\002. "+idnick;
	     origin.sendMessage(tosend,getName());
             services->getDatabase().dbGetRow();
	  }
	return;
     }
   int onlineID = origin.getOnlineID();
   
   int nbRes = services->getDatabase().dbSelect("idas", "nicksidentified", "nick='"+String::convert(onlineID)+"'");

   int i=0;
   for(int j=0; j<nbRes; j++)
     {
	i++;
	String id =  services->getDatabase().dbGetValue();
	String idnick = services->getNick(id.toInt());
	String tosend = String("\002")+String::convert(i)+"\002. "+idnick;
	origin.sendMessage(tosend,getName());
        services->getDatabase().dbGetRow();
	return;
     }

}


/* Register */
  NICK_FUNC (Module::parseREGISTER)
{
   String password = tokens.nextToken();
   String email = tokens.nextToken();
   if(origin.isRegistered())
     {
	origin.sendMessage ("Error - This nickname is already registered", getName());
	return;
     }
   if(password=="" | email=="")
     {
	origin.sendMessage ("Usage: register password email@address", getName());
	return;
     }
   if(password.length()<4)
     {
	origin.sendMessage ("Error - Your password needs to be greater than four letters!", getName());
	return;
     }
   origin.sendMessage("Here on "+Kine::config().getNetworkName()+" we require email validation before allowing our users", getName());
   origin.sendMessage("to register nicknames, this is a simple process and only takes a few minutes to complete",getName());
   origin.sendMessage("You will shortly receive an email with further instructions",getName());
   origin.sendMessage("This will be delivered to the account :\002"+email+"\002",getName());
   origin.sendMessage("Thank you for using "+Kine::config().getNetworkName()+"!",getName());
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
	origin.sendMessage("Usage: kill nickname password",getName());
	return;
     }
   User *ptr = services->findUser(tokill);
   if(ptr==0)
     {
	origin.sendMessage("Error: That user is not online.",getName());
	return; /* yoda says so yes yes yes */
     }
     else
     { 
	if(ptr->isPending())
	  {
	     origin.sendMessage("Error: That nickname is in a pending state and may not be killed",getName());
	     return;
	  }
	if(!ptr->isRegistered())
	  {
	     origin.sendMessage("Error: That nickname is not registered!",getName());
	     return;
	  }
	String nickpass = ptr->getPass();
	if(nickpass == password)
	  {
	     services->killnick(tokill,getName(),"Kill requested by "+origin.getNickname());
	     origin.log(getName(),"Requested a kill on "+tokill);
	     return;
	  }
	origin.sendMessage("Error: Incorrect password",getName());
	int access = origin.getAccess("Serv");
	if(access>0)
	  {
	     String temp1 = origin.getHost();
	     String temp2 = origin.getIdent();
	     String thehost = String(temp2)+"@"+String(temp1);
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
		origin.sendMessage("Usage: ghost nickname password",getName());
		return;
	   }
	  if (!services->isNickRegistered( toghost))
	    {
	       String tosend = String("Error: Target nickname is not registered");
	       origin.sendMessage(tosend,getName());
	       return;
	    }
	  String nickpass = Utils::generatePassword(toghost, password);
	  
	  User *ptr = services->findUser(toghost);

	  if (ptr == 0) {
	     /* ?? */
	     return;
	  }
	  
	  String givepass = ptr->getPass();
	  if(nickpass == givepass)
	    {
	       /* Okay, well, this is not really a service. This is a user..
		* so technically this is utterly the wrong call.. this will
		* change, I suspect, when kine grows up a little more..
		* The mode, btw, was +id
		*/

services->registerService(toghost,"ghost","ghosts.peoplechat.org",
					"Ghosted by "+origin.getNickname());
	       String tosend = String("Ghost successful for ")+toghost;
	       origin.sendMessage(tosend,getName());
	       services->log(origin,getName(),String("Successfully ghosted ")+toghost);
	       return;
	    }
	  String tosend = String("\002Incorrect\002 Password");
	  services->log(origin,getName(),String("Failed ghost"));
	  origin.sendMessage(tosend,getName());
	  return;
       }


/* Parse an identification request */
     NICK_FUNC (Module::parseIDENTIFY)
       {
	  String password = tokens.nextToken();
	  if (origin.isPending())
	    {
	       String tosend = "Error - You cannot identify a nickname which is pending verification";
	       origin.sendMessage (tosend, getName());
	       return;
	    }
	  if (!services->isNickRegistered (origin.getNickname()))
	    {
	       String tosend = String ("Error: That nickname is not registered");
	       origin.sendMessage (tosend, getName());
	       return;
	    }
	  if(!safe)
	    {
	       String tosend = String ("For security reasons you must use /ns id <password> to identify");
	       origin.sendMessage (tosend, getName());
	       return;
	    }
	  String nickpass = Utils::generatePassword(origin.getNickname(),
						    password);
	  String givepass = origin.getPass();
	  std::cout << ":" << nickpass << ":" << std::endl;
	  std::cout << ":" << givepass << ":" << std::endl;
	  if(nickpass == givepass)
	    {
	       int oid = origin.getOnlineID();
	       int nid = services->getRegisteredNickID(origin.getNickname());

               services->getDatabase().dbInsert("nicksidentified", "'','"+String::convert(oid) + "','" + String::convert(nid) + "'");
               services->getDatabase().dbDelete("kills", "nick='"+origin.getNickname()+"'");

	       services->modeIdentify(origin.getNickname());
	       services->updateLastID(origin.getNickname());
	       String temp1 = origin.getHost();
	       String temp2 = origin.getIdent();
	       String togo = String(temp2)+"@"+String(temp1);
	       origin.updateHost(togo);
	       String tosend = "You have been successfully identified to services as the nickname \002"+origin.getNickname()+"\002(\002"+togo+"\002)";
	       origin.sendMessage(tosend,getName());
	       return;
	    }
	  origin.sendMessage(String("\002Incorrect\002 Password"),getName());
	  int access = origin.getAccess("Serv");
	  if(access>0)
	    {
	       String temp1 = origin.getHost();
	       String temp2 = origin.getIdent();
	       String thehost = String(temp2)+"@"+String(temp1);
	       String togo = String("\002Failed\002 identify for nickname ")+origin.getNickname()+" by \002"+origin.getNickname()+"!"+thehost;
	       services->logLine(String(togo), Log::Warning);
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
   
   // Register ourself to the network
   services->registerService(getName(), getName(), 
			     getConfigData().getHostname(),
			     getConfigData().getDescription());
   
   // We started okay :)
   return true;
}
