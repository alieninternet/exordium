/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __NICKNAME_H_
#define __NICKNAME_H_

#include <kineircd/str.h>

using Kine::String;

namespace Exordium {
	class Nickname {
		public:
        	 String getRegNickCount(void);
        	 String getOnlineCount(void);
        	 void updateHost(String const &, String const &);
        	 String generatePassword(String const &, String const &);
        	 int getAccess(String const &, String const &);
        	 bool isAuthorised(String const &);
        	 void addCheckidentify(String const &);
        	 void addClient (String nick, String hops, String timestamp,
                         String username, String host, String vwhost,
                         String server, String modes, String realname);
        	 int countHost(String const &);
        	 int getRequiredAccess(String const &, String const &);
  		 bool isNickRegistered (String const &);
  		 String getPass (String const &);
  		 int getOnlineNickID(String const &);    
  		 int getRegisteredNickID(String const &);
  		 void modeIdentify (String const &);
  		 void updateLastID(String const &);
  		 String getHost(String const &);
  		 String getIdent(String const &);
  		 bool isIdentified(String const &);
  		 bool isIdentified(String const &, String const &);
  		 String getNick(int const &);
  		 String getOnlineNick(int const &);
  		 String getIDList(String const &);
  		 String getpendingCode(String const &);  
  		 bool isPending(String const &);
  		 void registerNick(String const &,String const &,String const &);
  		 String genAuth(String const &);   
  		 void setLanguage(String const &,String const &);
  		 String getICQ (String const &nick);
  		 String getMSN (String const &nick);
  		 String getYAHOO (String const &nick);
  		 String getAIM (String const &nick);
  		 String getURL (String const &nick);
  		 String getLanguage(String const &nick);
  		 String getEmail (const String & nick);
  		 String getLastHost (const String &nick);
  		 String getLastID (const String &nick);  
  		 String getRegDate (const String &nick);
		 void setDeopAway(String const &, bool const &);
		 bool deopAway(String const &);
	};


};

#endif
