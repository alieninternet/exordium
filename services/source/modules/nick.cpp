/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "include/nick.h"
#include "exordium/services.h"
#include "exordium/nickname.h"
#include <kineircd/str.h>
#include <kineircd/utils.h>
#include "exordium/sql.h"
#include "exordium/module.h"
#include <map>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;



struct Nick::functionTableStruct const
  Nick::functionTable[] = {
  {"auth", parseAUTH},
  {"identify", parseIDENTIFY},
  {"id", parseIDENTIFY},
  {"help", parseHELP},
  {"kill", parseKILL},
  {"ghost", parseGHOST},
  {"register", parseREGISTER},
  {"access", parseACCESS},
  {"set", parseSET},
  {"info", parseINFO},
  {0}
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
          functionTable[i].function (origin, st);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "Nick", requestor);
return;
}

void
NICK_FUNC (Nick::parseAUTH)
{
	String gauth = tokens.nextToken();
	if(!Nickname::isPending(origin))
	{
		Services::serviceNotice("Error: This nickname is not pending confirmation","Nick",origin);
		return;
	}
	String authcode = Nickname::getpendingCode(origin);
	Services::Debug("\002[\002Auth System\002]\002 The pending code is ->" + authcode);
	Services::Debug("\002[\002Auth System\002]\002 The given code was  ->" + gauth);
	if(authcode==gauth)
	{
		Services::serviceNotice("Congratulations you have confirmed your nickname. You can now use services freely","Nick",origin);
		Services::serviceNotice("You may now identify your nickname as normal","Nick",origin);
		String query = "DELETE from pendingnicks where nickname='" + origin + "'";
		Sql::query(query);
	}
}

/* Info */
void
NICK_FUNC (Nick::parseINFO)
{
String who = tokens.nextToken();
if(who=="")
{
	Services::serviceNotice("Usage: /msg nick info nickname","Nick",origin);
	return;
}

if(!Nickname::isNickRegistered(who))
{
	Services::serviceNotice("Error: That nickname is not registered","nick",origin);
	return;
}

int saccess = Nickname::getAccess("Serv",origin);
int oaccess = Nickname::getAccess("Oper",origin);
if(saccess>0 || oaccess>0)
{
	if(Nickname::isIdentified(origin,origin))
	{
		String lhost = Nickname::getLastHost(who);
		String lid = Nickname::getLastID(who);
		String lreg = Nickname::getRegDate(who);
		String lemail = Nickname::getEmail(who);
		String licq = Nickname::getICQ(who);
		String lmsn = Nickname::getMSN(who);
		String laim = Nickname::getAIM(who);
		String lurl = Nickname::getURL(who);
		String lyah = Nickname::getYAHOO(who);
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
		Services::serviceNotice(toa,"Nick",origin);
		Services::serviceNotice(tob,"Nick",origin);
		Services::serviceNotice(toc,"Nick",origin);
		Services::serviceNotice(tod,"Nick",origin);
		Services::serviceNotice(toe,"Nick",origin);
		Services::serviceNotice(tof,"Nick",origin);
		Services::serviceNotice(tog,"Nick",origin);
		Services::serviceNotice(toi,"Nick",origin);
		Services::serviceNotice(toh,"Nick",origin);
		Services::serviceNotice(toj,"Nick",origin);
		return;
	}
}

String lhost = Nickname::getLastHost(who);
String lid = Nickname::getLastID(who);
String lreg = Nickname::getRegDate(who);
String licq = Nickname::getICQ(who);
String lmsn = Nickname::getMSN(who);
String laim = Nickname::getAIM(who);
String lurl = Nickname::getURL(who);
String lyah = Nickname::getYAHOO(who);
String toa = "Nickname Information Report (NON-STAFF) for \002"+who;
String tob = "Last Host : \002<HIDDEN>";
String toc = "Last Identified : \002"+lid;
String tod = "Registered : \002"+lreg;
String toe = "ICQ Number : \002"+licq;
String tof = "MSN : \002"+lmsn;
String toh = "AIM : \002"+laim;
String tog = "URL : \002"+lurl;
String toi = "Yahoo! : \002"+lyah;
Services::serviceNotice(toa,"Nick",origin);
Services::serviceNotice(tob,"Nick",origin);
Services::serviceNotice(toc,"Nick",origin);
Services::serviceNotice(tod,"Nick",origin);
Services::serviceNotice(toe,"Nick",origin);
Services::serviceNotice(tof,"Nick",origin);
Services::serviceNotice(tog,"Nick",origin);
Services::serviceNotice(toh,"Nick",origin);
Services::serviceNotice(toi,"Nick",origin);

}
/* Set */
void
NICK_FUNC (Nick::parseSET)
{
	String command = tokens.nextToken();
	String value = tokens.nextToken();
	if(!Nickname::isIdentified(origin,origin))
		return;
	if(command=="pass")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set pass newpassword";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		if(value.length()<5)
		{
			String togo = "Error: New password must be greater than 4 letters";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String newhash = Nickname::generatePassword(origin,value);
		String query = "UPDATE nicks set password='" + newhash + "' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "Password has been successfully changed to "+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick",String("Changed nickname password"));
		return;
	}
	if(command=="language")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set language LANGUAGE";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		if(value=="english")
		{
			Nickname::setLanguage(origin,"english");
			Services::serviceNotice("Helpfiles interface has been changed to english","Nick",origin);
			return;
		}

	}
	if(command=="email")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set email email@Address";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String query = "UPDATE nicks set email='"+value+"' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "Email has been changed to \002"+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick","Changed email address to "+value);
		return;
	}

	if(command=="msn")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set msn MSN handle";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String query = "UPDATE nicks set msn='"+value+"' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "MSN has been changed to \002"+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick","Changed MSN address to "+value);
		return;
	}

	if(command=="aim")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set aim handle";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String query = "UPDATE nicks set aim='"+value+"' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "AIM has been changed to \002"+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick","Changed AIM address to "+value);
		return;
	}

	if(command=="yahoo")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set yahoo! handle";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String query = "UPDATE nicks set yahoo='"+value+"' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "Yahoo! has been changed to \002"+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick","Changed Yahoo! address to "+value);
		return;
	}


	if(command=="icq")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set icq ICQNUMBER";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String query = "UPDATE nicks set icq='"+value+"' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "ICQ has been changed to \002"+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick","Changed ICQ address to "+value);
		return;
	}

	if(command=="url")
	{
		if(value=="")
		{
			String togo = "Usage is /msg Nick set url www.ircdome.org";
			Services::serviceNotice(togo,"Nick",origin);
			return;
		}
		String query = "UPDATE nicks set url='"+value+"' WHERE nickname='"+origin+"'";
		Sql::query(query);
		String togo = "URL has been changed to \002"+value;
		Services::serviceNotice(togo,"Nick",origin);
		Services::log(origin,"Nick","Changed URL address to "+value);
		return;
	}
	if(command=="privmsg")
	{
		if(value=="on")
		{
			String query = "UPDATE nicks set privmsg=1 where nickname='"+origin+"'";
			Sql::query(query);
			Services::serviceNotice("I will now use the private message interface","Nick",origin);
			return;
		}
		if(value=="off")
		{
			String query = "UPDATE nicks set privmsg=0 where nickname='"+origin+"'";
			Sql::query(query);
			Services::serviceNotice("I will now use the private message interface","Nick",origin);
			return;
		}

	}
}
/* Access */
void
NICK_FUNC (Nick::parseACCESS)
{
	String nickname = tokens.nextToken();
	if(nickname!="")
	{
		int onlineID = Nickname::getOnlineNickID(nickname);
		String query = "SELECT idas from identified where nick='" + String::convert(onlineID)+"'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		int i=0;
		while ((row = res.fetch_row()))
			{
				i++;
				String id = ((std::string) row[0]).c_str();
				String idnick = Nickname::getNick(id.toInt());
				String tosend = String("\002")+String::convert(i)+"\002. "+idnick;
				Services::serviceNotice(tosend,"Nick",origin);
			}
		res.free_result();
	return;
	}
		int onlineID = Nickname::getOnlineNickID(origin);
		String query = "SELECT idas from identified where nick='" + String::convert(onlineID)+"'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		int i=0;
		while ((row = res.fetch_row()))
			{
				i++;
				String id = ((std::string) row[0]).c_str();
				String idnick = Nickname::getNick(id.toInt());
				String tosend = String("\002")+String::convert(i)+"\002. "+idnick;
				Services::serviceNotice(tosend,"Nick",origin);
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
	if(Nickname::isNickRegistered(origin))
	{
		String tosend = "Error - This nickname is already registered";
		Services::serviceNotice (tosend, "Nick", origin);
		return;
	}
	if(password=="")
	{
		String tosend = "Error - No password: Usage is /msg nick register password email@Address";
		Services::serviceNotice (tosend, "Nick", origin);
		return;
	}
	if(password.length()<4)
	{
		String tosend = "Error - Small Length: Your password needs to be greater than 4 letters";
		Services::serviceNotice (tosend, "Nick", origin);
		return;
	}
	Services::serviceNotice("Here on IRCDome we require email validation before allowing our users", "Nick", origin);
	Services::serviceNotice("to register nicknames, this is a simple process and only takes a few minutes to complete","Nick",origin);
	Services::serviceNotice("You will shortly receive an email with further instructions","Nick",origin);
	Services::serviceNotice("This will be delivered to the account :\002"+email+"\002","Nick",origin);
	Services::serviceNotice("Thank you for using IRCDome!","Nick",origin);
	Nickname::registerNick(origin,password,email);
	Services::log(origin,"Nick","Registered nickname with email "+email);
	String authcode = Nickname::genAuth(origin);
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
	Services::sendEmail(email,subject,emailtext);
}

/* Kill */
void
NICK_FUNC (Nick::parseKILL)
{
	String tokill = tokens.nextToken();
	String password = tokens.nextToken();
	if(Nickname::isPending(tokill))
	{
		String tosend = "Error - You cannot kill a nickname which is pending verification";
		Services::serviceNotice(tosend,"Nick",origin);
		return;
	}
	if(!Nickname::isNickRegistered(tokill))
	{
		String tosend = "Error - "+tokill+" is not a registered nickname";
		Services::serviceNotice(tosend,"Nick",origin);
		return;
	}
	String nickpass = Nickname::generatePassword(tokill,password);
	String givepass = Nickname::getPass(tokill);
	if(nickpass == givepass)
	{
		String reason = "Kill requested by "+origin;
		Services::killnick(tokill,"Nick",reason);
		Services::log(origin,"Nick","Requested a kill on "+tokill);
		return;
	}
	String tosend = "Error - Incorrect password for nickname "+tokill;
	Services::serviceNotice(tosend,"Nick",origin);
	int access = Nickname::getAccess("Serv",origin);
	if(access>0)
	{
		String temp1 = Nickname::getHost(origin);
		String temp2 = Nickname::getIdent(origin);
		String thehost = String(temp2)+"@"+String(temp1);
		String togo = String("\002Failed\002 kill for nickname ")+origin+" by \002"+origin+"!"+thehost;
		Services::helpme(String(togo),"Serv");
	}

}
/* Do help... */
void
NICK_FUNC (Nick::parseHELP)
{
	String word = tokens.nextToken();
	String parm = tokens.nextToken();
	Services::doHelp(origin,"nick", word, parm);
}

/* Ghost... */
void
NICK_FUNC (Nick::parseGHOST)
{
	String toghost = tokens.nextToken();
	String password = tokens.nextToken();
	if (!Nickname::isNickRegistered( toghost))
	{
		String tosend = String("Error: Target nickname is not registered");
		Services::serviceNotice(tosend,"nick",origin);
		return;
	}
	String nickpass = Nickname::generatePassword(toghost,password);
	String givepass = Nickname::getPass(toghost);
	if(nickpass == givepass)
	{
		Services::registerService(toghost,"ghost","ghosts.ircdome.org",
			"+id","Ghosted by "+origin);
		String tosend = String("Ghost successful for ")+toghost;
		Services::serviceNotice(tosend,"nick",origin);
		Services::log(origin,"Nick",String("Successfully ghosted ")+toghost);
		return;
	}
	String tosend = String("\002Incorrect\002 Password");
	Services::log(origin,"Nick",String("Failed ghost"));
	Services::serviceNotice(tosend,"Nick",origin);
	return;
}
/* Parse an identification request */

void
NICK_FUNC (Nick::parseIDENTIFY)
{
	String password = tokens.nextToken();
	if (Nickname::isPending(origin))
	{
		String tosend = "Error - You cannot identify a nickname which is pending verification";
		Services::serviceNotice (tosend, "Nick", origin);
		return;
	}
	if (!Nickname::isNickRegistered (origin))
	{
		String tosend = String ("Error: That nickname is not registered");
		Services::serviceNotice (tosend, "Nick", origin);
		return;
	}
	if(!Services::SecurePrivmsg)
	{
		String tosend = String ("For security reasons you must use /msg nick@ircdome.org to identify");
		Services::serviceNotice (tosend, "Nick", origin);
		return;
	}
	String nickpass = Nickname::generatePassword(origin,password);
	String givepass = Nickname::getPass(origin);
	if(nickpass == givepass)
	{
		int oid = Nickname::getOnlineNickID(origin);
		int nid = Nickname::getRegisteredNickID(origin);
		String query = String("INSERT into identified values('','") + 
			String::convert(oid) + "','" + String::convert(nid) + "')";
		Sql::query(query);
		query = String("DELETE from kills where nick='") +
			origin + "'";
		Sql::query(String(query));
		Nickname::modeIdentify(origin);
		Nickname::updateLastID(origin);
		String temp1 = Nickname::getHost(origin);
		String temp2 = Nickname::getIdent(origin);
		String togo = String(temp2)+"@"+String(temp1);
		Nickname::updateHost(origin,togo);
		String tosend = "You have been successfully identified to services as the nickname \002"+origin+"\002(\002"+togo+"\002)";
		Services::serviceNotice(tosend,"Nick",origin);
		return;
	}
	Services::serviceNotice(String("\002Incorrect\002 Password"),"Nick",origin);
	int access = Nickname::getAccess("Serv",origin);
	if(access>0)
	{
		String temp1 = Nickname::getHost(origin);
		String temp2 = Nickname::getIdent(origin);
		String thehost = String(temp2)+"@"+String(temp1);
		String togo = String("\002Failed\002 identify for nickname ")+origin+" by \002"+origin+"!"+thehost;
		Services::helpme(String(togo),"Serv");
	}
return;
}



EXORDIUM_SERVICE_INIT_FUNCTION {
	Services::registerService(name,name,"ircdome.org", "+dz",
				"Nickname Registration Services");
	Services::serviceJoin(name,"#Debug");
   return new Module("nick", new Nick(name));
}


