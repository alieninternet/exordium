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

#include "include/nick.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include <kineircd/utils.h>
#include <map>
#include <sstream>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;

struct Nick::functionTableStruct const
  Nick::functionTable[] =
{
     {"auth", &Nick::parseAUTH},
     {"identify", &Nick::parseIDENTIFY},
     {"id", &Nick::parseIDENTIFY},
     {"help", &Nick::parseHELP},
     {"kill", &Nick::parseKILL},
     {"ghost", &Nick::parseGHOST},
     {"register", &Nick::parseREGISTER},
     {"access", &Nick::parseACCESS},
     {"set", &Nick::parseSET},
     {"info", &Nick::parseINFO},
     {0, 0}
};

/* Entry point for Nick:: - Parse the given line and decide what to do with it */
void
  Nick::parseLine (StringTokens& line, User& origin, String const &ch)
{
   /* Nick doesn't really take in channel commands..... */
   return;
}

void
  Nick::parseLine (StringTokens& line, User& origin)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage ("Unrecognized Command",getName());
   return;
}

void
  NICK_FUNC (Nick::parseAUTH)
{
   String gauth = tokens.nextToken();
   if(!origin.isPending())
     {
	origin.sendMessage("Error: This nickname is not pending confirmation",getName());
	return;
     }
   String authcode = services->getpendingCode(origin.getNickname());
   services->Debug("\002[\002Auth System\002]\002 The pending code is ->" + authcode);
   services->Debug("\002[\002Auth System\002]\002 The given code was  ->" + gauth);
   if(authcode==gauth)
     {
	origin.sendMessage("Congratulations you have confirmed your nickname. You can now use services freely",getName());
	origin.sendMessage("You may now identify your nickname as normal",getName());
        services->getDatabase().dbDelete("pendingnicks", "nickname='"+origin.getNickname()+"'");
     }
}

/* Info */
void
  NICK_FUNC (Nick::parseINFO)
{
   String who = tokens.nextToken().IRCtoLower();
   User *ptr = services->findUser(who);
   if(ptr==0)
     {
	origin.sendMessage("Error: Could not locate that user (possible bug)",getName());
	return;
     }

   if(who=="")
     {
	origin.sendMessage("Usage: /msg nick info nickname",getName());
	return;
     }

   if(!services->isNickRegistered(origin.getNickname()))
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
	     String lhost = services->getLastHost(ptr->getNickname());
	     String lid = services->getLastID(ptr->getNickname());
	     String lreg = services->getRegDate(ptr->getNickname());
	     String lemail = services->getEmail(ptr->getNickname());
	     String licq = services->getICQ(who);
	     String lmsn = services->getMSN(who);
	     String laim = services->getAIM(who);
	     String lurl = services->getURL(who);
	     String lyah = services->getYAHOO(who);
	     String lqui = ptr->getQuitMessage();
	     bool deopAway = ptr->deopAway();
	     bool modNick = ptr->modNick();
	     String toa = "Nickname Information for \002"+who;
	     String tob = "Last host : \002"+lhost;
	     String toc = "Last identified : \002"+lid;
	     String tod = "Registered : \002"+lreg;
	     String toe = "Email : \002"+lemail;
	     String tof = "ICQ : \002"+licq;
	     String tog = "MSN : \002"+lmsn;
	     String toi = "AIM : \002"+laim;
	     String toh = "URL : \002"+lurl;
	     String toj = "Yahoo! : \002"+lyah;
	     String tok = "Last Quit Message : \002"+lqui;
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

	     origin.sendMessage(toa,getName());
	     origin.sendMessage(tob,getName());
	     origin.sendMessage(toc,getName());
	     origin.sendMessage(tod,getName());
	     origin.sendMessage(toe,getName());
	     origin.sendMessage(tof,getName());
	     origin.sendMessage(tog,getName());
	     origin.sendMessage(toi,getName());
	     origin.sendMessage(toh,getName());
	     origin.sendMessage(toj,getName());
	     origin.sendMessage(tok,getName());
	     origin.sendMessage(tol.str(),getName());
	     return;
	  }
     }

   String lhost = services->getLastHost(who);
   String lid = services->getLastID(who);
   String lreg = services->getRegDate(who);
   String licq = services->getICQ(who);
   String lmsn = services->getMSN(who);
   String laim = services->getAIM(who);
   String lurl = services->getURL(who);
   String lyah = services->getYAHOO(who);
   String toa = "Nickname Information Report (NON-STAFF) for \002"+who;
   String tob = "Last Host : \002<HIDDEN>";
   String toc = "Last Identified : \002"+lid;
   String tod = "Registered : \002"+lreg;
   String toe = "ICQ Number : \002"+licq;
   String tof = "MSN : \002"+lmsn;
   String toh = "AIM : \002"+laim;
   String tog = "URL : \002"+lurl;
   String toi = "Yahoo! : \002"+lyah;
   origin.sendMessage(toa,getName());
   origin.sendMessage(tob,getName());
   origin.sendMessage(toc,getName());
   origin.sendMessage(tod,getName());
   origin.sendMessage(toe,getName());
   origin.sendMessage(tof,getName());
   origin.sendMessage(tog,getName());
   origin.sendMessage(toh,getName());
   origin.sendMessage(toi,getName());

}
/* Set */
void
  NICK_FUNC (Nick::parseSET)
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
	String newhash = services->generatePassword(origin.getNickname(),value);
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
	     String togo = "Usage is /msg Nick set url www.ircdome.org";
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
void
  NICK_FUNC (Nick::parseACCESS)
{
   String nickname = tokens.nextToken();
   if(nickname!="")
     {
        int nbRes = services->getDatabase().dbSelect("idas", "identified", "nick='"+origin.getOnlineIDString()+"'");

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
   
   int nbRes = services->getDatabase().dbSelect("idas", "identified", "nick='"+String::convert(onlineID)+"'");

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
void
  NICK_FUNC (Nick::parseREGISTER)
{
   String password = tokens.nextToken();
   String email = tokens.nextToken();
   if(services->isNickRegistered(origin.getNickname()))
     {
	String tosend = "Error - This nickname is already registered";
	origin.sendMessage (tosend, getName());
	return;
     }
   if(password=="")
     {
	String tosend = "Error - No password: Usage is /msg nick register password email@Address";
	origin.sendMessage (tosend, getName());
	return;
     }
   if(password.length()<4)
     {
	String tosend = "Error - Small Length: Your password needs to be greater than 4 letters";
	origin.sendMessage (tosend, getName());
	return;
     }
   origin.sendMessage("Here on IRCDome we require email validation before allowing our users", getName());
   origin.sendMessage("to register nicknames, this is a simple process and only takes a few minutes to complete",getName());
   origin.sendMessage("You will shortly receive an email with further instructions",getName());
   origin.sendMessage("This will be delivered to the account :\002"+email+"\002",getName());
   origin.sendMessage("Thank you for using IRCDome!",getName());
   services->registerNick(origin.getNickname(),password,email);
   services->log(origin,getName(),"Registered nickname with email "+email);
   String authcode = services->genAuth(origin.getNickname());
   String subject = "Your nickname registration";
   String emailtext =
     "This is a sample email - Needs replacing with something much better\n"
     "\n"
     "\n"
     "In order to successfully register your nickname, you need to type\n"
     "the following next time you are on IRCDome.\n"
     "\n"
     "\n"
     "/msg nick AUTH "+authcode+"\n"
     "\n";
   services->sendEmail(email,subject,emailtext);
}

/* Kill */
void
  NICK_FUNC (Nick::parseKILL)
{
   String tokill = tokens.nextToken();
   String password = tokens.nextToken();
   User *ptr = services->findUser(tokill);
   if(ptr==0)
     {
	origin.sendMessage("Error - Could not find a pointer for this user - Possible Bug?",getName());
	if(ptr->isPending())
	  {
	     String tosend = "Error - You cannot kill a nickname which is pending verification";
	     origin.sendMessage(tosend,getName());
	     return;
	  }
	if(!services->isNickRegistered(tokill))
	  {
	     String tosend = "Error - "+tokill+" is not a registered nickname";
	     origin.sendMessage(tosend,getName());
	     return;
	  }
	String nickpass = String::convert(services->generatePassword(tokill,password));
	String givepass = services->getPass(tokill);
	if(nickpass == givepass)
	  {
	     String reason = "Kill requested by "+origin.getNickname();
	     services->killnick(tokill,getName(),reason);
	     services->log(origin,getName(),"Requested a kill on "+tokill);
	     return;
	  }
	String tosend = "Error - Incorrect password for nickname "+tokill;
	origin.sendMessage(tosend,getName());
	int access = origin.getAccess("Serv");
	if(access>0)
	  {
	     String temp1 = origin.getHost();
	     String temp2 = origin.getIdent();
	     String thehost = String(temp2)+"@"+String(temp1);
	     String togo = String("\002Failed\002 kill for nickname ")+origin.getNickname()+" by \002"+origin.getNickname()+"!"+thehost;
	     services->helpme(String(togo),"Serv");
	  }

     }
}

/* Do help... */
   void
     NICK_FUNC (Nick::parseHELP)
       {
	  String word = tokens.nextToken();
	  String parm = tokens.nextToken();
	  services->doHelp(origin,getName(), word, parm);
       }

/* Ghost... */
   void
     NICK_FUNC (Nick::parseGHOST)
       {
	  String toghost = tokens.nextToken();
	  String password = tokens.nextToken();
	  if (!services->isNickRegistered( toghost))
	    {
	       String tosend = String("Error: Target nickname is not registered");
	       origin.sendMessage(tosend,getName());
	       return;
	    }
	  String nickpass = String::convert(services->generatePassword(toghost,password));
	  String givepass = services->getPass(toghost);
	  if(nickpass == givepass)
	    {
	       services->registerService(toghost,"ghost","ghosts.ircdome.org",
					"+id","Ghosted by "+origin.getNickname());
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

   void
     NICK_FUNC (Nick::parseIDENTIFY)
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
	  if(!services->SecurePrivmsg)
	    {
	       String tosend = String ("For security reasons you must use /msg nick@ircdome.org to identify");
	       origin.sendMessage (tosend, getName());
	       return;
	    }
	  String nickpass = services->generatePassword(origin.getNickname(),password);
	  String givepass = services->getPass(origin.getNickname());
	  if(nickpass == givepass)
	    {
	       int oid = origin.getOnlineID();
	       int nid = services->getRegisteredNickID(origin.getNickname());

               services->getDatabase().dbInsert("identified", "'','"+String::convert(oid) + "','" + String::convert(nid) + "'");
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
	       services->helpme(String(togo),"Serv");
	    }
	  return;
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	return new Nick();
     }

   // Module information structure
   const Nick::moduleInfo_type Nick::moduleInfo =
     {
	"Nickname Service",
	  0, 0,
	  MASK_USER_SIGNON
     };

   // Start the service
   void Nick::start(Exordium::Services& s)
     {
	// Set the services field appropriately
	services = &s;
	
	// Register ourself to the network
	services->registerService(getName(), getName(), 
				 getConfigData().getHostname(), "+dz",
				 getConfigData().getDescription());
	services->serviceJoin(getName(),"#Debug");
     }
