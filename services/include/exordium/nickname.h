/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __NICKNAME_H_
#define __NICKNAME_H_

#include "kineircd/str.h"

using Kine::String;

namespace Exordium {
	class Nickname {
		public:
        	static String getRegNickCount(void);
        	static String getOnlineCount(void);
        	static void updateHost(String const &, String const &);
        	static String generatePassword(String const &, String const &);
        	static int getAccess(String const &, String const &);
        	static bool isAuthorised(String const &);
        	static void addCheckidentify(String const &);
        	static void addClient (String nick, String hops, String timestamp,
                         String username, String host, String vwhost,
                         String server, String modes, String realname);
        	static int countHost(String const &);
        	static int getRequiredAccess(String const &, String const &);
  		static bool isNickRegistered (String const &);
  		static String getPass (String const &);
  		static int getOnlineNickID(String const &);    
  		static int getRegisteredNickID(String const &);
  		static void modeIdentify (String const &);
  		static void updateLastID(String const &);
  		static String getHost(String const &);
  		static String getIdent(String const &);
  		static bool isIdentified(String const &);
  		static bool isIdentified(String const &, String const &);
  		static String getNick(int const &);
  		static String getOnlineNick(int const &);
  		static String getIDList(String const &);
  		static String getpendingCode(String const &);  
  		static bool isPending(String const &);
  		static void registerNick(String const &,String const &,String const &);
  		static String genAuth(String const &);   
  		static void setLanguage(String const &,String const &);
  		static String getICQ (String const &nick);
  		static String getMSN (String const &nick);
  		static String getURL (String const &nick);
  		static String getLanguage(String const &nick);
  		static String getEmail (const String & nick);
  		static String getLastHost (const String &nick);
  		static String getLastID (const String &nick);  
  		static String getRegDate (const String &nick);

	};


};

#endif
