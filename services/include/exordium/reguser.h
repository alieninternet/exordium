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
      AISutil::String name;
      AISutil::String pass;
      AISutil::String email;
      AISutil::String regdate;
      AISutil::String lastid;
      AISutil::String lasthost;
      unsigned short int privmsg;
      AISutil::String lang;
      AISutil::String icq;
      AISutil::String msn;
      AISutil::String url;
      unsigned short int deopaway;
      AISutil::String yahoo;
      AISutil::String aim;
      AISutil::String quitmsg;
      unsigned short int modnick;


      // Lists owned chans
      list<AISutil::String> ownedchans;


    public:
      RegisteredUser(AISutil::String const &nickname)
	{ name=nickname; ID=-1; }

      ~RegisteredUser(void)
	{}
      

      // Getters
      int                getID(void)       { return ID; }
      AISutil::String    getName(void)     { return name; }
      AISutil::String    getPass(void)     { return pass; }
      AISutil::String    getEmail(void)    { return email; }
      AISutil::String    getRegdate(void)  { return regdate; }
      AISutil::String    getLastID(void)   { return lastid; }
      AISutil::String    getLasthost(void) { return lasthost; }
      unsigned short int getPrivmsg(void)  { return privmsg; }
      AISutil::String    getLang(void)     { return lang; }
      AISutil::String    getIcq(void)      { return icq; }
      AISutil::String    getMsn(void)      { return msn; }
      AISutil::String    getUrl(void)      { return url; }
      AISutil::String    getYahoo(void)    { return yahoo; }
      AISutil::String    getAim(void)      { return aim; }
      AISutil::String    getQuitmsg(void)  { return quitmsg; }
      unsigned short int getModnick(void)  { return modnick; }
      unsigned short int getDeopaway(void) { return deopaway; }
      

      // Setters
      void setID       (int id)                            { ID=id; }
      void setName     (AISutil::String const &channame)   { name=channame; }
      void setPass     (AISutil::String const &mypass);
      void setEmail    (AISutil::String const &myemail);
      void setRegdate  (AISutil::String myregdate);
      void setLastID   (AISutil::String mylastid);
      void setLasthost (AISutil::String const &mylasthost);
      void setPrivmsg  (unsigned short int myprivmsg);
      void setLang     (AISutil::String const &mylang);
      void setIcq      (AISutil::String const &myicq);
      void setMsn      (AISutil::String const &mymsn);
      void setUrl      (AISutil::String const &myurl);
      void setYahoo    (AISutil::String const &myyahoo);
      void setAim      (AISutil::String const &myaim);
      void setQuitmsg  (AISutil::String const &myquitmsg);
      void setModnick  (unsigned short int mymodnick);
      void setDeopaway (unsigned short int mydeopaway);


      void addOwnedChannel(AISutil::String const &name);
      void delOwnedChannel(AISutil::String const &name);
      bool ownsChannel(AISutil::String const &name);

   };


   class RegisteredUserList:public Singleton<RegisteredUserList>
   {
      public:
               friend class Singleton<RegisteredUserList>;

               ~RegisteredUserList(void);

               void registerNick(RegisteredUser *newregnick);

               void unregisterNick(AISutil::String const &name);
               RegisteredUser* getRegisteredNick(AISutil::String const &name);

               bool isRegistered(AISutil::String const &name);
               int  nbRegUsers(void);

               // Synchronize with DB (Load or reload data)
               void SyncDB(void);

      private:
               map<AISutil::String, RegisteredUser*> regusers;

               RegisteredUserList(void) {}
   };

}; // namespace Exordium


#endif // _INCLUDE_EXORDIUM_REGUSER_H_
