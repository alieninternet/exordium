/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/nickname.h"
#include <kineircd/str.h>
#include "exordium/services.h"
#include "exordium/sql.h"
#include "exordium/utils.h"

using namespace Exordium;
using Kine::String;
using Kine::StringMask;
using Kine::Password;
namespace Exordium
{

void
Nickname::updateHost (String const &nick, String const &host)
{
        String query = "UPDATE nicks set lasthost='" + host + "' where nickname='" + nick + "'";
        Sql::query(String(query));
        return;
}

String 
Nickname::getRegNickCount(void)
{ 
String query = "SELECT count(*) from nicks";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
	res.free_result();
        return foo;
}
res.free_result();
return String("0");
}

String 
Nickname::getOnlineCount(void)
{
String query = "SELECT count(*) from onlineclients";
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
	res.free_result();
        return foo;
}
res.free_result();

return String("0");
}

int
Nickname::getAccess(String const &service, String const &who)
{
        String query = "SELECT access from access where nickname='" + who + "' and service='" + service + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String axs = ((std::string) row[0]).c_str();
		res.free_result();
                return axs.toInt();
        }
res.free_result();
return 0;
}

String 
Nickname::generatePassword(String const &nickname, String const &password)
{
 String pw =
        Utils::SHA1::digestToStr(Password::makePassword(nickname,
                                                            password),
                                 Services::PasswordStrBase,
                                 Services::PasswordStrBaseLongPad);
return pw;
}

bool
Nickname::isAuthorised(String const &server)
{
String query = "SELECT id from serverlist where name='" + server + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
        if((int)row[0]>0)
		{
		res.free_result();
                return true;
		}
}
res.free_result();
return false;
}

void
Nickname::addCheckidentify(String const &nick)
{   
        Services::serviceNotice("The nickname you are using requires identification, Please /msg nick@ircdome.org identify <password>","Nick",nick);
	int rfac = Services::random(60);
        String query = "INSERT into kills values('','"+nick+"','" + String::convert(Services::currentTime + 120 + rfac) + "')";
        Sql::query(query);
 
}   

void    
Nickname::addClient (String nick, String hops, String timestamp, String username,
               String host, String vwhost, String server, String modes,
               String realname)
{
  Sql::query (String ("insert into onlineclients values('','") + nick +String ("','") + hops + String ("','") + timestamp +String ("','") + username + String ("','") + host + String ("','") + vwhost + String ("','") + server + String ("','") + modes + String ("','") + Sql::makeSafe(realname) +String ("')"));
               
}            

int            
Nickname::countHost(String const &hostname)
{
String query = "select id from onlineclients where hostname='"+hostname+"'";
MysqlRes res = Sql::query(query);
int num = res.num_fields();
res.free_result();
return num;
}
 

int
Nickname::getRequiredAccess(String const &service, String const &command)
{       
        String query = "SELECT access from commands where service='"+ service + "' AND command='" + command + "'";
        MysqlRes res = Sql::query(query);
        MysqlRow row;
        while ((row = res.fetch_row()))
        {       
                String access = ((std::string) row[0]).c_str();
		res.free_result();
                return access.toInt();
        }
res.free_result();
//If no entry.... return >max
return 999;
}

bool
Nickname::isNickRegistered (String const &nick)
{
        String query = String("select id from nicks where nickname='") + nick + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
        if((int)row[0]>0)   
		{
		res.free_result();
                return true;
		}
        }
res.free_result();
return false;
}

String
Nickname::getPass (String const &nick)
{  
        String query = "select password from nicks where nickname='" + nick + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String password = ((std::string) row[0]).c_str ();
		res.free_result();
                return password;
        }
res.free_result();
return String("");
}  

int
Nickname::getOnlineNickID(String const &nick)
{
String query = "select id from onlineclients where nickname='" + nick + "'";
String debug = "--->Fetching online identification for "+nick;
Services::Debug(debug);
MysqlRes res = Sql::query(String(query));
MysqlRow row;
while ((row = res.fetch_row()))
{
                String id = ((std::string) row[0]).c_str();
		res.free_result();
                Services::Debug("---->Which is "+id);
                return id.toInt();
}
res.free_result();
return 0;
}

int
Nickname::getRegisteredNickID(String const &nick)
{
        String debug = "--->Fetching registered identification for "+nick;
        Services::Debug(debug);
        String query = "select id from nicks where nickname='" + nick + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String id = ((std::string) row[0]).c_str();
		res.free_result();
                Services::Debug("---->Which is "+id);
                return id.toInt();
        }
res.free_result();
return 0;
}

void
Nickname::modeIdentify (String const &nick)
{
        String tosend = String(":services.ircdome.org SVSMODE ")+nick+" +r";
        Services::queueAdd(String(tosend));
        return;
}

void
Nickname::updateLastID (String const &nick)
{
        String query = String("UPDATE nicks set lastid=NOW() 
        where nickname='") + nick + "'";
        Sql::query(String(query));
        return;
}

String
Nickname::getHost(String const &nick)
{
        String query = String("SELECT hostname from onlineclients 
        where nickname='") + nick + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String host = ((std::string) row[0]).c_str();
		res.free_result();
                return host;
        }
res.free_result();
return String("");
}

String
Nickname::getIdent(String const &nick)
{
        String query = "SELECT username from onlineclients where nickname=
        '" + nick + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String ident = ((std::string) row[0]).c_str();
		res.free_result();
                return ident;
        }
res.free_result();
return String("");
}

bool
Nickname::isIdentified(String const &nick)
{
        int onlineID = getOnlineNickID(nick);
        String query = "SELECT idas from identified where nick='"+String::convert(onlineID)+"'";
        MysqlRes res = Sql::query(query);
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String temp = ((std::string) row[0]).c_str();
		res.free_result();
                if(temp.toInt() > 0)
                        return true;
        }
	res.free_result();
        return false;
}

bool
Nickname::isIdentified (String const &nick, String const &as)
{
int onlineID = getOnlineNickID(nick);
Services::Debug("--->Checking to see if "+nick+" is identified as "+as);
String query = "SELECT idas from identified where nick='"+String::convert(onlineID)+"'";
        MysqlRes res = Sql::query(query);
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String temp = ((std::string) row[0]).c_str();
                String idnick = getNick(temp.toInt());
		res.free_result();
                Services::Debug("--->Does "+temp+" equal "+idnick);
                if(idnick.toLower()==as.toLower())
                {
                        Services::Debug("---->Yes - Break");
                        return true;
                }
                Services::Debug("---->No - Loop");
        
        }
	res.free_result();
        return false;
}

String
Nickname::getNick(int const &id)
{
String query = "SELECT nickname from nicks where id='"+String::convert(id)+"'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
	res.free_result();
        return foo;
}
res.free_result();
return String("");
};

String
Nickname::getOnlineNick(int const &id)
{
String query = "SELECT nickname from onlineclients where id='"+String::convert(id)+"'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
        String foo = ((std::string) row[0]).c_str();
	res.free_result();
        return foo;
}
res.free_result();
return String("");
}


String
Nickname::getIDList(String const &nick)
{
int onlineID = getOnlineNickID(nick);
String thelist = "";
String query = "SELECT idas from identified where nick='"+String::convert(onlineID)+"'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
        String rval = ((std::string) row[0]).c_str();
        String idnick = getNick(rval.toInt());
        thelist = String(thelist)+" "+String(idnick);

}
res.free_result();
return thelist;
}

String
Nickname::getpendingCode(String const &nick)
{
        String query = "SELECT auth from pendingnicks where nickname='" + nick + "'";
        MysqlRes res = Sql::query(String(query));
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String ident = ((std::string) row[0]).c_str();
		res.free_result();
                return ident;
        }
res.free_result();
return String("");
}

bool  
Nickname::isPending(String const &nick)
{
String query = "SELECT id from pendingnicks where nickname='"+nick+"'";   
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
{
        if((int)row[0]>0)
		{
		res.free_result();
                return true;
		}
}
res.free_result();
return false;
}

void  
Nickname::registerNick(String const &nick, String const &password, String const &email)
{
String gpass = Nickname::generatePassword(nick.toLower(),password);
String query = "INSERT into nicks values('','" + nick + "','" + gpass + "','" + email + "',NOW(),NOW(),'',0,'','','','')";
Sql::query(query);
}    

String
Nickname::genAuth(String const &nickname)
{   
String authcode = Nickname::generatePassword("AUTHENTICATION",nickname);
String query = "INSERT into pendingnicks values ('','"+nickname+"','" +authcode+"')";
Sql::query(query);
String tosend = "Generated new authcode for registered nickname "+nickname+"
-> "+authcode;
Services::Debug(tosend);
return authcode;
}

void
Nickname::setLanguage (String const &nick, String const &lang)
{
        String query = "UPDATE nicks set lang='" + lang + "' WHERE nickname='" + nick + "'";
        Sql::query(query);
        
}     


String
Nickname::getURL(String const &nick)
{
String query = "SELECT url from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}

String
Nickname::getMSN(String const &nick)
{
String query = "SELECT msn from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        
        }
return String("");
}

String
Nickname::getYAHOO(String const &nick)
{
String query = "SELECT yahoo from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}


String
Nickname::getAIM(String const &nick)
{
String query = "SELECT aim from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}


String
Nickname::getICQ(String const &nick)
{
String query = "SELECT icq from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}


String
Nickname::getLanguage(String const &nick)
{
        if(!isNickRegistered(nick))
        {  
                return String("english");
        }
        String query = "SELECT lang from nicks where nickname='" + nick + "'";
        MysqlRes res = Sql::query(query);
        MysqlRow row;
        while ((row = res.fetch_row()))
        {
                String lang = ((std::string) row[0]).c_str();
		res.free_result();
                return lang;
        }
res.free_result();
return String("english");
}

String
Nickname::getEmail(String const &nick)
{
String query = "SELECT email from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}

String
Nickname::getRegDate(String const &nick)
{
String query = "SELECT registered from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}

String
Nickname::getLastID(String const &nick)
{
String query = "SELECT lastid from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}
 

String
Nickname::getLastHost(String const &nick)
{
String query = "SELECT lasthost from nicks where nickname='" + nick + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
while ((row = res.fetch_row()))
        {
        String temp = ((std::string) row[0]).c_str();
	res.free_result();
        return temp;
        }
res.free_result();
return String("");
}








}

