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

#ifndef _INCLUDE_EXORDIUM_REGUSER_H_
# define _INCLUDE_EXORDIUM_REGUSER_H_ 1

# include <aisutil/string.h>
# include <map>
# include <list>
# include <exordium/singleton.h>

using namespace std;

namespace Exordium { 

   #define regUserListFwd (*RegisteredUserList::instance())
  
   class RegisteredUser {
    private:
      int ID;
      AIS::Util::String name;
      AIS::Util::String pass;
      AIS::Util::String email;
      AIS::Util::String regdate;
      AIS::Util::String lastid;
      AIS::Util::String lasthost;
      unsigned short int privmsg;
      AIS::Util::String lang;
      AIS::Util::String icq;
      AIS::Util::String msn;
      AIS::Util::String url;
      unsigned short int deopaway;
      AIS::Util::String yahoo;
      AIS::Util::String aim;
      AIS::Util::String quitmsg;
      unsigned short int modnick;


      // Lists owned chans
      list<AIS::Util::String> ownedchans;


    public:
      RegisteredUser(AIS::Util::String const &nickname)
	{ name=nickname; ID=-1; }

      ~RegisteredUser(void)
	{}
      

      // Getters
      int                getID(void)       { return ID; }
      AIS::Util::String    getName(void)     { return name; }
      AIS::Util::String    getPass(void)     { return pass; }
      AIS::Util::String    getEmail(void)    { return email; }
      AIS::Util::String    getRegdate(void)  { return regdate; }
      AIS::Util::String    getLastID(void)   { return lastid; }
      AIS::Util::String    getLasthost(void) { return lasthost; }
      unsigned short int getPrivmsg(void)  { return privmsg; }
      AIS::Util::String    getLang(void)     { return lang; }
      AIS::Util::String    getIcq(void)      { return icq; }
      AIS::Util::String    getMsn(void)      { return msn; }
      AIS::Util::String    getUrl(void)      { return url; }
      AIS::Util::String    getYahoo(void)    { return yahoo; }
      AIS::Util::String    getAim(void)      { return aim; }
      AIS::Util::String    getQuitmsg(void)  { return quitmsg; }
      unsigned short int getModnick(void)  { return modnick; }
      unsigned short int getDeopaway(void) { return deopaway; }
      

      // Setters
      void setID       (int id)                            { ID=id; }
      void setName     (AIS::Util::String const &channame)   { name=channame; }
      void setPass     (AIS::Util::String const &mypass);
      void setEmail    (AIS::Util::String const &myemail);
      void setRegdate  (AIS::Util::String myregdate);
      void setLastID   (AIS::Util::String mylastid);
      void setLasthost (AIS::Util::String const &mylasthost);
      void setPrivmsg  (unsigned short int myprivmsg);
      void setLang     (AIS::Util::String const &mylang);
      void setIcq      (AIS::Util::String const &myicq);
      void setMsn      (AIS::Util::String const &mymsn);
      void setUrl      (AIS::Util::String const &myurl);
      void setYahoo    (AIS::Util::String const &myyahoo);
      void setAim      (AIS::Util::String const &myaim);
      void setQuitmsg  (AIS::Util::String const &myquitmsg);
      void setModnick  (unsigned short int mymodnick);
      void setDeopaway (unsigned short int mydeopaway);


      void addOwnedChannel(AIS::Util::String const &name);
      void delOwnedChannel(AIS::Util::String const &name);
      bool ownsChannel(AIS::Util::String const &name);

   };


   class RegisteredUserList:public Singleton<RegisteredUserList>
   {
      public:
               friend class Singleton<RegisteredUserList>;

               ~RegisteredUserList(void);

               void registerNick(RegisteredUser *newregnick);

               void unregisterNick(AIS::Util::String const &name);
               RegisteredUser* getRegisteredNick(AIS::Util::String const &name);

               bool isRegistered(AIS::Util::String const &name);
               int  nbRegUsers(void);

               // Synchronize with DB (Load or reload data)
               void SyncDB(void);

      private:
               map<AIS::Util::String, RegisteredUser*> regusers;

               RegisteredUserList(void) {}
   };

}; // namespace Exordium


#endif // _INCLUDE_EXORDIUM_REGUSER_H_
