/*
 *  * Exordium Network Services
 *  * Copyright (C) 2002 IRCDome Development Team
 *  *
 *  * This file is a part of Exordium.
 *  *
 *  * $Id$
 *  *
 *  * This program is free software; you can redistribute it and/or modify
 *  * it under the terms of the GNU General Public License as published by
 *  * the Free Software Foundation; either version 2 of the License, or
 *  * (at your option) any later version.
 *  *
 *  * This program is distributed in the hope that it will be useful,
 *  * but WITHOUT ANY WARRANTY; without even the implied
 *  * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  * See the GNU General Public License for more details.
 *  *
 *  * You should have received a copy of the GNU General Public License
 *  * along with this program; if not, write to the Free Software
 *  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,USA
 *  *
 *  * For contact details for the IRCDome Development Team please see the
 *  * enclosed README file.
 *  *
 * */


#include "include/nick.h"
#include "exordium/services.h"
#include "exordium/nickname.h"
#include <kineircd/str.h>
#include <kineircd/utils.h>
#include "exordium/sql.h"
#include "exordium/module.h"
#include <map>
#include <sstream>

using Kine::String;
using Kine::StringTokens;
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
  Nick::parseLine (String const &line, String const &requestor, String const &ch)
{
   return;
}

void
  Nick::parseLine (String const &line, String const &requestor)
{
   StringTokens st (line);
   String origin = requestor;
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
   services.serviceNotice ("Unrecognized Command", "Nick", requestor);
   return;
}

void
  NICK_FUNC (Nick::parseAUTH)
{
   String gauth = tokens.nextToken();
   if(!services.getNickname().isPending(origin))
     {
	services.serviceNotice("Error: This nickname is not pending confirmation","Nick",origin);
	return;
     }
   String authcode = services.getNickname().getpendingCode(origin);
   services.Debug("\002[\002Auth System\002]\002 The pending code is ->" + authcode);
   services.Debug("\002[\002Auth System\002]\002 The given code was  ->" + gauth);
   if(authcode==gauth)
     {
	services.serviceNotice("Congratulations you have confirmed your nickname. You can now use services freely","Nick",origin);
	services.serviceNotice("You may now identify your nickname as normal","Nick",origin);
	String query = "DELETE from pendingnicks where nickname='" + origin + "'";
	services.getDatabase().query(query);
     }
}

/* Info */
void
  NICK_FUNC (Nick::parseINFO)
{
   String who = tokens.nextToken();
   if(who=="")
     {
	services.serviceNotice("Usage: /msg nick info nickname","Nick",origin);
	return;
     }

   if(!services.getNickname().isNickRegistered(who))
     {
	services.serviceNotice("Error: That nickname is not registered","nick",origin);
	return;
     }

   int saccess = services.getNickname().getAccess("Serv",origin);
   int oaccess = services.getNickname().getAccess("Oper",origin);
   if(saccess>0 || oaccess>0)
     {
	if(services.getNickname().isIdentified(origin,origin))
	  {
	     String lhost = services.getNickname().getLastHost(who);
	     String lid = services.getNickname().getLastID(who);
	     String lreg = services.getNickname().getRegDate(who);
	     String lemail = services.getNickname().getEmail(who);
	     String licq = services.getNickname().getICQ(who);
	     String lmsn = services.getNickname().getMSN(who);
	     String laim = services.getNickname().getAIM(who);
	     String lurl = services.getNickname().getURL(who);
	     String lyah = services.getNickname().getYAHOO(who);
	     String lqui = services.getNickname().getQuitMessage(who);
	     bool deopAway = services.getNickname().deopAway(who);
	     bool modNick = services.getNickname().modNick(who);
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
	     if(deopAway) {
	       tol << " Deop on Away";
	     }
	     if(modNick) {
	       tol << ", Nickname Enforcement";
	     }
						 
	     services.serviceNotice(toa,"Nick",origin);
	     services.serviceNotice(tob,"Nick",origin);
	     services.serviceNotice(toc,"Nick",origin);
	     services.serviceNotice(tod,"Nick",origin);
	     services.serviceNotice(toe,"Nick",origin);
	     services.serviceNotice(tof,"Nick",origin);
	     services.serviceNotice(tog,"Nick",origin);
	     services.serviceNotice(toi,"Nick",origin);
	     services.serviceNotice(toh,"Nick",origin);
	     services.serviceNotice(toj,"Nick",origin);
	     services.serviceNotice(tok,"Nick",origin);
	     services.serviceNotice(tol.str(),"Nick",origin);
	     return;
	  }
     }

   String lhost = services.getNickname().getLastHost(who);
   String lid = services.getNickname().getLastID(who);
   String lreg = services.getNickname().getRegDate(who);
   String licq = services.getNickname().getICQ(who);
   String lmsn = services.getNickname().getMSN(who);
   String laim = services.getNickname().getAIM(who);
   String lurl = services.getNickname().getURL(who);
   String lyah = services.getNickname().getYAHOO(who);
   String toa = "Nickname Information Report (NON-STAFF) for \002"+who;
   String tob = "Last Host : \002<HIDDEN>";
   String toc = "Last Identified : \002"+lid;
   String tod = "Registered : \002"+lreg;
   String toe = "ICQ Number : \002"+licq;
   String tof = "MSN : \002"+lmsn;
   String toh = "AIM : \002"+laim;
   String tog = "URL : \002"+lurl;
   String toi = "Yahoo! : \002"+lyah;
   services.serviceNotice(toa,"Nick",origin);
   services.serviceNotice(tob,"Nick",origin);
   services.serviceNotice(toc,"Nick",origin);
   services.serviceNotice(tod,"Nick",origin);
   services.serviceNotice(toe,"Nick",origin);
   services.serviceNotice(tof,"Nick",origin);
   services.serviceNotice(tog,"Nick",origin);
   services.serviceNotice(toh,"Nick",origin);
   services.serviceNotice(toi,"Nick",origin);

}
/* Set */
void
  NICK_FUNC (Nick::parseSET)
{
   String command = tokens.nextToken();
   String value = tokens.nextToken();
   if(!services.getNickname().isIdentified(origin,origin))
     return;
   if(command=="pass" || command=="password" || command=="passwd")
     {
	if(value=="")
	  {
	     String togo = "Usage is /msg Nick set pass newpassword";
	     services.serviceNotice(togo,"Nick",origin);
	     return;
	  }
	if(value.length()<5)
	  {
	     String togo = "Error: New password must be greater than 4 letters";
	     services.serviceNotice(togo,"Nick",origin);
	     return;
	  }
	String newhash = services.getNickname().generatePassword(origin,value);
	String query = "UPDATE nicks set password='" + newhash + "' WHERE nickname='"+origin+"'";
	services.getDatabase().query(query);
	String togo = "Password has been successfully changed to "+value;
	services.serviceNotice(togo,"Nick",origin);
	services.log(origin,"Nick",String("Changed nickname password"));
	return;
     }
   if(command=="modnick")
     {
	if(value=="")
	  {
	     String togo = "Usage is set modnick true/false";
	     services.serviceNotice(togo,"Nick",origin);
	  }
	
	if(value=="true")
	  {
	     services.getNickname().setModNick(origin,true);
	     String togo = "Nickname enforcement is now enabled";
	     services.serviceNotice(togo,"Nick",origin);
	     return;
	  }
	
	if(value=="false")
	  {
	     services.getNickname().setModNick(origin,false);
	     String togo = "Nickname enforcement is now disabled";
	     services.serviceNotice(togo,"Nick",origin);
	     return;
	  }
	services.serviceNotice("Error: Value must be true or false","Nick",origin);
	return;
	
     }
   
   if(command=="deopaway")
     {
	if(value=="")
	  {
	     String togo = "Usage is set deopaway true/false";
	     services.serviceNotice(togo,"Nick",origin);
	  }
	if(value=="true")
	  {
	     services.getNickname().setDeopAway(origin,true);
	     String togo = "You will now be automatically deoped on setting away";
	     services.serviceNotice(togo,"Nick",origin);
	     return;
	  }
	if(value=="false")
	  {
	     services.getNickname().setDeopAway(origin,false);
	     String togo = "You will no longer be automatically deoped on setting away";
	     services.serviceNotice(togo,"Nick",origin);
	     return;
	  }
	services.serviceNotice("Error: Value must be true or false","Nick",origin);
	return;
     }
   
	if(command=="language")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set language LANGUAGE";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     if(value=="english")
	       {
		  services.getNickname().setLanguage(origin,"english");
		  services.serviceNotice("Helpfiles interface has been changed to english","Nick",origin);
		  return;
	       }
	     services.serviceNotice("Error: Unsupported Language!","nick",origin);
	     return;

	  }
	if(command=="email")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set email email@Address";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     String query = "UPDATE nicks set email='"+value+"' WHERE nickname='"+origin+"'";
	     services.getDatabase().query(query);
	     String togo = "Email has been changed to \002"+value;
	     services.serviceNotice(togo,"Nick",origin);
	     services.log(origin,"Nick","Changed email address to "+value);
	     return;
	  }

	if(command=="msn")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set msn MSN handle";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     String query = "UPDATE nicks set msn='"+value+"' WHERE nickname='"+origin+"'";
	     services.getDatabase().query(query);
	     String togo = "MSN has been changed to \002"+value;
	     services.serviceNotice(togo,"Nick",origin);
	     services.log(origin,"Nick","Changed MSN address to "+value);
	     return;
	  }

	if(command=="aim")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set aim handle";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     String query = "UPDATE nicks set aim='"+value+"' WHERE nickname='"+origin+"'";
	     services.getDatabase().query(query);
	     String togo = "AIM has been changed to \002"+value;
	     services.serviceNotice(togo,"Nick",origin);
	     services.log(origin,"Nick","Changed AIM address to "+value);
	     return;
	  }

	if(command=="yahoo")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set yahoo! handle";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     String query = "UPDATE nicks set yahoo='"+value+"' WHERE nickname='"+origin+"'";
	     services.getDatabase().query(query);
	     String togo = "Yahoo! has been changed to \002"+value;
	     services.serviceNotice(togo,"Nick",origin);
	     services.log(origin,"Nick","Changed Yahoo! address to "+value);
	     return;
	  }

	if(command=="icq")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set icq ICQNUMBER";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     String query = "UPDATE nicks set icq='"+value+"' WHERE nickname='"+origin+"'";
	     services.getDatabase().query(query);
	     String togo = "ICQ has been changed to \002"+value;
	     services.serviceNotice(togo,"Nick",origin);
	     services.log(origin,"Nick","Changed ICQ address to "+value);
	     return;
	  }

	if(command=="url")
	  {
	     if(value=="")
	       {
		  String togo = "Usage is /msg Nick set url www.ircdome.org";
		  services.serviceNotice(togo,"Nick",origin);
		  return;
	       }
	     String query = "UPDATE nicks set url='"+value+"' WHERE nickname='"+origin+"'";
	     services.getDatabase().query(query);
	     String togo = "URL has been changed to \002"+value;
	     services.serviceNotice(togo,"Nick",origin);
	     services.log(origin,"Nick","Changed URL address to "+value);
	     return;
	  }
	if(command=="privmsg")
	  {
	     if(value=="on")
	       {
		  String query = "UPDATE nicks set privmsg=1 where nickname='"+origin+"'";
		  services.getDatabase().query(query);
		  services.serviceNotice("I will now use the private message interface","Nick",origin);
		  return;
	       }
	     if(value=="off")
	       {
		  String query = "UPDATE nicks set privmsg=0 where nickname='"+origin+"'";
		  services.getDatabase().query(query);
		  services.serviceNotice("I will now use the private message interface","Nick",origin);
		  return;
	       }
	     services.serviceNotice("Error: Value must be on or off","Nick",origin);
	     return;
	  }
   services.serviceNotice("Error: Unsupported command","Nick",origin);
   return;
     }
/* Access */
   void
     NICK_FUNC (Nick::parseACCESS)
       {
	  String nickname = tokens.nextToken();
	  if(nickname!="")
	    {
	       int onlineID = services.getNickname().getOnlineNickID(nickname);
	       String query = "SELECT idas from identified where nick='" + String::convert(onlineID)+"'";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       int i=0;
	       while ((row = res.fetch_row()))
		 {
		    i++;
		    String id = ((std::string) row[0]).c_str();
		    String idnick = services.getNickname().getNick(id.toInt());
		    String tosend = String("\002")+String::convert(i)+"\002. "+idnick;
		    services.serviceNotice(tosend,"Nick",origin);
		 }
	       res.free_result();
	       return;
	    }
	  int onlineID = services.getNickname().getOnlineNickID(origin);
	  String query = "SELECT idas from identified where nick='" + String::convert(onlineID)+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  int i=0;
	  while ((row = res.fetch_row()))
	    {
	       i++;
	       String id = ((std::string) row[0]).c_str();
	       String idnick = services.getNickname().getNick(id.toInt());
	       String tosend = String("\002")+String::convert(i)+"\002. "+idnick;
	       services.serviceNotice(tosend,"Nick",origin);
	       res.free_result();
	       return;
	    }

       }
/* Register */
   void
     NICK_FUNC (Nick::parseREGISTER)
       {
	  String password = tokens.nextToken();
	  String email = tokens.nextToken();
	  if(services.getNickname().isNickRegistered(origin))
	    {
	       String tosend = "Error - This nickname is already registered";
	       services.serviceNotice (tosend, "Nick", origin);
	       return;
	    }
	  if(password=="")
	    {
	       String tosend = "Error - No password: Usage is /msg nick register password email@Address";
	       services.serviceNotice (tosend, "Nick", origin);
	       return;
	    }
	  if(password.length()<4)
	    {
	       String tosend = "Error - Small Length: Your password needs to be greater than 4 letters";
	       services.serviceNotice (tosend, "Nick", origin);
	       return;
	    }
	  services.serviceNotice("Here on IRCDome we require email validation before allowing our users", "Nick", origin);
	  services.serviceNotice("to register nicknames, this is a simple process and only takes a few minutes to complete","Nick",origin);
	  services.serviceNotice("You will shortly receive an email with further instructions","Nick",origin);
	  services.serviceNotice("This will be delivered to the account :\002"+email+"\002","Nick",origin);
	  services.serviceNotice("Thank you for using IRCDome!","Nick",origin);
	  services.getNickname().registerNick(origin,password,email);
	  services.log(origin,"Nick","Registered nickname with email "+email);
	  String authcode = services.getNickname().genAuth(origin);
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
	  services.sendEmail(email,subject,emailtext);
       }

/* Kill */
   void
     NICK_FUNC (Nick::parseKILL)
       {
	  String tokill = tokens.nextToken();
	  String password = tokens.nextToken();
	  if(services.getNickname().isPending(tokill))
	    {
	       String tosend = "Error - You cannot kill a nickname which is pending verification";
	       services.serviceNotice(tosend,"Nick",origin);
	       return;
	    }
	  if(!services.getNickname().isNickRegistered(tokill))
	    {
	       String tosend = "Error - "+tokill+" is not a registered nickname";
	       services.serviceNotice(tosend,"Nick",origin);
	       return;
	    }
	  String nickpass = String::convert(services.getNickname().generatePassword(tokill,password));
	  String givepass = services.getNickname().getPass(tokill);
	  if(nickpass == givepass)
	    {
	       String reason = "Kill requested by "+origin;
	       services.killnick(tokill,"Nick",reason);
	       services.log(origin,"Nick","Requested a kill on "+tokill);
	       return;
	    }
	  String tosend = "Error - Incorrect password for nickname "+tokill;
	  services.serviceNotice(tosend,"Nick",origin);
	  int access = services.getNickname().getAccess("Serv",origin);
	  if(access>0)
	    {
	       String temp1 = services.getNickname().getHost(origin);
	       String temp2 = services.getNickname().getIdent(origin);
	       String thehost = String(temp2)+"@"+String(temp1);
	       String togo = String("\002Failed\002 kill for nickname ")+origin+" by \002"+origin+"!"+thehost;
	       services.helpme(String(togo),"Serv");
	    }

       }
/* Do help... */
   void
     NICK_FUNC (Nick::parseHELP)
       {
	  String word = tokens.nextToken();
	  String parm = tokens.nextToken();
	  services.doHelp(origin,"nick", word, parm);
       }

/* Ghost... */
   void
     NICK_FUNC (Nick::parseGHOST)
       {
	  String toghost = tokens.nextToken();
	  String password = tokens.nextToken();
	  if (!services.getNickname().isNickRegistered( toghost))
	    {
	       String tosend = String("Error: Target nickname is not registered");
	       services.serviceNotice(tosend,"nick",origin);
	       return;
	    }
	  String nickpass = String::convert(services.getNickname().generatePassword(toghost,password));
	  String givepass = services.getNickname().getPass(toghost);
	  if(nickpass == givepass)
	    {
	       services.registerService(toghost,"ghost","ghosts.ircdome.org",
					"+id","Ghosted by "+origin);
	       String tosend = String("Ghost successful for ")+toghost;
	       services.serviceNotice(tosend,"nick",origin);
	       services.log(origin,"Nick",String("Successfully ghosted ")+toghost);
	       return;
	    }
	  String tosend = String("\002Incorrect\002 Password");
	  services.log(origin,"Nick",String("Failed ghost"));
	  services.serviceNotice(tosend,"Nick",origin);
	  return;
       }
/* Parse an identification request */

   void
     NICK_FUNC (Nick::parseIDENTIFY)
       {
	  String password = tokens.nextToken();
	  if (services.getNickname().isPending(origin))
	    {
	       String tosend = "Error - You cannot identify a nickname which is pending verification";
	       services.serviceNotice (tosend, "Nick", origin);
	       return;
	    }
	  if (!services.getNickname().isNickRegistered (origin))
	    {
	       String tosend = String ("Error: That nickname is not registered");
	       services.serviceNotice (tosend, "Nick", origin);
	       return;
	    }
	  if(!services.SecurePrivmsg)
	    {
	       String tosend = String ("For security reasons you must use /msg nick@ircdome.org to identify");
	       services.serviceNotice (tosend, "Nick", origin);
	       return;
	    }
	  String nickpass = services.getNickname().generatePassword(origin,password);
	  String givepass = services.getNickname().getPass(origin);
	  if(nickpass == givepass)
	    {
	       int oid = services.getNickname().getOnlineNickID(origin);
	       int nid = services.getNickname().getRegisteredNickID(origin);
	       String query = String("INSERT into identified values('','") +
		 String::convert(oid) + "','" + String::convert(nid) + "')";
	       services.getDatabase().query(query);
	       query = String("DELETE from kills where nick='") +
		 origin + "'";
	       services.getDatabase().query(String(query));
	       services.getNickname().modeIdentify(origin);
	       services.getNickname().updateLastID(origin);
	       String temp1 = services.getNickname().getHost(origin);
	       String temp2 = services.getNickname().getIdent(origin);
	       String togo = String(temp2)+"@"+String(temp1);
	       services.getNickname().updateHost(origin,togo);
	       String tosend = "You have been successfully identified to services as the nickname \002"+origin+"\002(\002"+togo+"\002)";
	       services.serviceNotice(tosend,"Nick",origin);
	       return;
	    }
	  services.serviceNotice(String("\002Incorrect\002 Password"),"Nick",origin);
	  int access = services.getNickname().getAccess("Serv",origin);
	  if(access>0)
	    {
	       String temp1 = services.getNickname().getHost(origin);
	       String temp2 = services.getNickname().getIdent(origin);
	       String thehost = String(temp2)+"@"+String(temp1);
	       String togo = String("\002Failed\002 identify for nickname ")+origin+" by \002"+origin+"!"+thehost;
	       services.helpme(String(togo),"Serv");
	    }
	  return;
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	services.registerService(name,name,"ircdome.org", "+dz",
				 "Nickname Registration Services");
	services.serviceJoin(name,"#Debug");
	return new Module("nick", new Nick(services, name));
     }

