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

#ifndef _INCLUDE_EXORDIUM_USER_H_
# define _INCLUDE_EXORDIUM_USER_H_ 1

# include <aisutil/string.h>
# include <map>
# include <exordium/services.h>
# include <kineircd/name.h>

namespace Exordium
{
   class User
     {
      private:
	Kine::Name nickname;
	int onlineID;
	Services& services;
	bool registered; /* Are we a registered nick or not? */
	int floodCounter; /* Flood counter */
      public:
      /* Inline functions here */
      /* getNickname()
       *
       * Return our nickname
       *
       */

	const Kine::Name& getNickname(void) const
	  { return nickname; };

      /* getOnlineID()
       *
       * Return our unique sql identifier
       *
       */
	const int getOnlineID(void) const
	  { return onlineID; };

      /* getOnlineIDString()
       *
       * Return our unique sql identifier
       * (as a String)
       *
       * This is redundant imho. - simon
       */
	const AIS::Util::String getOnlineIDString(void) const
	  { return AIS::Util::String::convert(onlineID); };

      /* setNick(String)
       *
       * Update our records to show a new nickname.....
       *
       */
	void setNick(const AIS::Util::String& nick)
	  { nickname = nick; };

      /* Take note. Their are two sendMessages, the latter allowing
       * you to override services default behaviour of using the
       * nicknames settings of notice/privmsg
       */
	void sendMessage(AIS::Util::String const &message,
			 AIS::Util::String const &service);
	void sendMessage(AIS::Util::String const &message,
			 AIS::Util::String const &service,
			 bool const &privmsg);

      /* setModNick(bool)
       *
       * Modify the value of ModNick
       *
       */
	void setModNick(bool const &);

      /* modNick()
       *
       * Return the current value of modNick
       *
       */
	bool modNick(void);

      /* setDeopAway(bool)
       *
       * Modify the value of DeopAway
       *
       */
	void setDeopAway(bool const &);

      /* deopAway()
       *
       * Return the value of DeopAway
       *
       */
	bool deopAway(void);

      /* updateHost(String)
       *
       * Update the clients hostname.
       *
       */
	void updateHost(AIS::Util::String const &);

      /* getQuitMessage()
       *
       * Return the last known quit message for this user
       *
       */
	AIS::Util::String getQuitMessage(void);
	
	int getTimeStamp(void);

      /* getAccess(String)
       *
       * Return the access for this user with the given
       * service.
       *
       */
	int getAccess(AIS::Util::String const &);

      /* addCheckIdentify()
       *
       * Force a client to identify for this nickname...
       *
       */
	void addCheckIdentify(void);

      /* countHost()
       *
       * Return the number of online people from this
       * users hostname.
       *
       */
	int countHost(void);

      /* getHost()
       *
       * Return the users hostname.
       *
       */
	AIS::Util::String getHost(void);

      /* getIdent()
       *
       * Return the users ident (username)
       *
       */
	AIS::Util::String getIdent(void);

      /* getVWHost()
       *
       * Return the users vw host
       *
       */
	AIS::Util::String getVWHost(void);

      /* isIdentified()
       *
       * Return true if we are identified as *ANY* nickname.
       *
       */
	bool isIdentified(void);

      /* isIdentified(String)
       *
       * Return true if we are identified as the given nickname.
       *
       */
	bool isIdentified(Kine::Name const &);
	
      /* getServer()
       * 
       * Return the server this user is using..
       * 
       */
	AIS::Util::String getServer(void);

      /* getModes()
       *
       * Query the DB for current user's modes
       *
       */
	AIS::Util::String getModes(void);

      /* getIDList(String)
       *
       * Return a list of nicknames we are identified as...
       *
       */
	AIS::Util::String getIDList(void);

      /* isPending()
       *
       * Return TRUE if we are pending verification :-)
       *
       */
	bool isPending(void);

      /* setLanguage(String)
       *
       * Update our clients language.......
       *
       * This could possibly be broken.. but i'll look at it later
       * its not a serious quirk (relating to the possibiltiy of someone
       * changing the language for a nick not in use... )
       */
	void setLanguage(AIS::Util::String const &);

	const bool isRegistered(); /* ARE WE registered?!?! */

      /* getLanguage()
       *
       * Retrieve users language
       */
	AIS::Util::String getLanguage(void);

      /* getPendingCode()
       *
       * Get the pending code (if any) for this nick
       */
	const void clearPendingCode(void);
	const void registerNick(AIS::Util::String const &,
				AIS::Util::String const &);
	const void log(AIS::Util::String const &, AIS::Util::String const &);
	const AIS::Util::String genAuth(void);
	// update* Functions.
	const void updateIdentified(void);
	// set* Functions.
	const void setPassword(AIS::Util::String const &);
	const void setEmail(AIS::Util::String const &);
	const void setInfo(AIS::Util::String const &,AIS::Util::String const &);
	const void setPrivmsg(bool const &);
	// get* Functions.
	const AIS::Util::String getPendingCode(void);
	const AIS::Util::String getPass(void);
	const AIS::Util::String getURL(void);
	const AIS::Util::String getMSN(void);
	const AIS::Util::String getYAHOO(void);
	const AIS::Util::String getAIM(void);
	const AIS::Util::String getICQ(void);
	const AIS::Util::String getEmail(void);
	const AIS::Util::String getRegDate(void);
	const AIS::Util::String getLastID(void);
	const AIS::Util::String getLastHost(void);
	const int getRegisteredNickID(void);

      /* Flood control */
	int getFloodCount(void)
	  {
	     return floodCounter;
	  }

	void incFloodCount(void)
	  {
	     floodCounter=floodCounter+10;
	  }

	void decFloodCount(void)
	  {
	     if(floodCounter>10)
	       floodCounter=floodCounter-10;
	  }

      /* Our constructor definition */
	User(AIS::Util::String const &nick, int const &oid, Services &s)
	  : nickname(nick), //their current nickname.. duh
	onlineID(oid), //as dictated by the sql server..
	services(s)// Gives us access to the full suite of services..
	  {
	     floodCounter = 0;
	  };

	// Destructor
	~User(void)
	  {};

	// is equal to operator
	bool operator==(const User &u) const
	  { return (onlineID == u.onlineID); };
     };
}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_USER_H_
