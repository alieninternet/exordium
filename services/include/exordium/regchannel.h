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

#ifndef _INCLUDE_EXORDIUM_REGCHANNEL_H_
# define _INCLUDE_EXORDIUM_REGCHANNEL_H_ 1

# include <aisutil/string.h>
# include <exordium/singleton.h>
# include <map>
# include <vector>
# include <ctime>

using namespace std;

namespace Exordium { 

   #define regChannelListFwd (*RegisteredChannelList::instance())
  
   class chanbans
   {
     public:
      int ID;
      int chanid;
      AIS::Util::String mask;
      AIS::Util::String setby;
      time_t seton;
      int expireon;
      AIS::Util::String reason;
   };


   class RegisteredChannel {
    private:
      int ID;
      AIS::Util::String name;
      AIS::Util::String owner;
      AIS::Util::String topic;
      AIS::Util::String modes;
      AIS::Util::String cdesc;
      AIS::Util::String url;
      int clog;

      // Maps chanaccess (Username, access level)
      map<AIS::Util::String, int> access;
  
      // Lists chanbans
      vector<chanbans> bans;


    public:
      RegisteredChannel(AIS::Util::String const &channame)
	{ name=channame; ID=-1; }

      ~RegisteredChannel(void)
	{}
      

      // Getters
      int              getID(void)      { return ID; }
      AIS::Util::String  getName(void)    { return name; }
      AIS::Util::String  getOwner(void)   { return owner; }
      AIS::Util::String  getTopic(void)   { return topic; }
      AIS::Util::String  getModes(void)   { return modes; }
      AIS::Util::String  getCdesc(void)   { return cdesc; }
      AIS::Util::String  getUrl(void)     { return url; }
      int              getClog(void)    { return clog; }


      // Setters
      void setID(int id) { ID=id; }
      void setName(AIS::Util::String const &channame) { name=channame; }
      void setOwner (AIS::Util::String const &myowner);
      void setTopic (AIS::Util::String const &mytopic);
      void setModes (AIS::Util::String const &mymodes);
      void setCdesc (AIS::Util::String const &mycdesc);
      void setUrl   (AIS::Util::String const &myurl);
      void setClog  (int myclog);


      // Channel access map manipulation
      int getChanAccess(AIS::Util::String const &nick);
      void chanAddAccess(AIS::Util::String const &nick, int accesslvl);
      void chanDelAccess(AIS::Util::String const &nick);

      // Channel bans list manipulation
      void addChanBan(AIS::Util::String const &,AIS::Util::String const &,int const &,AIS::Util::String const &);
      void banChan(AIS::Util::String const &, AIS::Util::String const &, AIS::Util::String const &);
      void RemoveBan(AIS::Util::String const &);
   };


   class RegisteredChannelList:public Singleton<RegisteredChannelList>
   {
      public:
               friend class Singleton<RegisteredChannelList>;

               ~RegisteredChannelList(void);

               void registerChannel(RegisteredChannel *newregchan);
               void registerChannel(AIS::Util::String const &myname, AIS::Util::String const &myowner,
                                    AIS::Util::String const &mytopic, AIS::Util::String const &mymodes,
                                    AIS::Util::String const &mycdesc, AIS::Util::String const &myurl,
                                    int myclog);

               void unregisterChannel(AIS::Util::String const &name);
               RegisteredChannel* getRegisteredChannel(AIS::Util::String const &name);
               RegisteredChannel* getRegisteredChannelByID(int id);

               bool isRegistered(AIS::Util::String const &name);
               int  nbRegChans(void);

               // Synchronize with DB (Load or reload data)
               void SyncDB(void);


      private:
               map<AIS::Util::String, RegisteredChannel*> regchans;

               RegisteredChannelList(void) {}
   };


}; // namespace Exordium


#endif // _INCLUDE_EXORDIUM_REGCHANNEL_H_
