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
      AISutil::String mask;
      AISutil::String setby;
      time_t seton;
      int expireon;
      AISutil::String reason;
   };


   class RegisteredChannel {
    private:
      int ID;
      AISutil::String name;
      AISutil::String owner;
      AISutil::String topic;
      AISutil::String modes;
      AISutil::String cdesc;
      AISutil::String url;
      int clog;

      // Maps chanaccess (Username, access level)
      map<AISutil::String, int> access;
  
      // Lists chanbans
      vector<chanbans> bans;


    public:
      RegisteredChannel(AISutil::String const &channame)
	{ name=channame; ID=-1; }

      ~RegisteredChannel(void)
	{}
      

      // Getters
      int              getID(void)      { return ID; }
      AISutil::String  getName(void)    { return name; }
      AISutil::String  getOwner(void)   { return owner; }
      AISutil::String  getTopic(void)   { return topic; }
      AISutil::String  getModes(void)   { return modes; }
      AISutil::String  getCdesc(void)   { return cdesc; }
      AISutil::String  getUrl(void)     { return url; }
      int              getClog(void)    { return clog; }


      // Setters
      void setID(int id) { ID=id; }
      void setName(AISutil::String const &channame) { name=channame; }
      void setOwner (AISutil::String const &myowner);
      void setTopic (AISutil::String const &mytopic);
      void setModes (AISutil::String const &mymodes);
      void setCdesc (AISutil::String const &mycdesc);
      void setUrl   (AISutil::String const &myurl);
      void setClog  (int myclog);


      // Channel access map manipulation
      int getChanAccess(AISutil::String const &nick);
      void chanAddAccess(AISutil::String const &nick, int accesslvl);
      void chanDelAccess(AISutil::String const &nick);

      // Channel bans list manipulation
      void addChanBan(AISutil::String const &,AISutil::String const &,int const &,AISutil::String const &);
      void banChan(AISutil::String const &, AISutil::String const &, AISutil::String const &);
      void RemoveBan(AISutil::String const &);
   };


   class RegisteredChannelList:public Singleton<RegisteredChannelList>
   {
      public:
               friend class Singleton<RegisteredChannelList>;

               ~RegisteredChannelList(void);

               void registerChannel(RegisteredChannel *newregchan);
               void registerChannel(AISutil::String const &myname, AISutil::String const &myowner,
                                    AISutil::String const &mytopic, AISutil::String const &mymodes,
                                    AISutil::String const &mycdesc, AISutil::String const &myurl,
                                    int myclog);

               void unregisterChannel(AISutil::String const &name);
               RegisteredChannel* getRegisteredChannel(AISutil::String const &name);
               RegisteredChannel* getRegisteredChannelByID(int id);

               bool isRegistered(AISutil::String const &name);
               int  nbRegChans(void);

               // Synchronize with DB (Load or reload data)
               void SyncDB(void);


      private:
               map<AISutil::String, RegisteredChannel*> regchans;

               RegisteredChannelList(void) {}
   };


}; // namespace Exordium


#endif // _INCLUDE_EXORDIUM_REGCHANNEL_H_
