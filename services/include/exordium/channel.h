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

#ifndef _INCLUDE_EXORDIUM_CHANNEL_H_
# define _INCLUDE_EXORDIUM_CHANNEL_H_ 1

# include <kineircd/str.h>
#include <map>
#include <exordium/singleton.h>

using namespace std;

namespace Exordium {

   #define channelListFwd (*ChannelList::instance())

   class Channel {
    private:
      AISutil::String name;
      int ID;

      // This maps online users which are in this channel along with their status (opped, voiced)
      // 0=nothing, 1=voiced, 2=opped, 3=opped and voiced
      map<AISutil::String, unsigned short int> users;
      
    public:
      Channel(AISutil::String const &channame)
	{ name=channame; };

      ~Channel(void)
	{};
      

      // Getters
      AISutil::String getName(void) { return name; }
      int getID(void) { return ID; }

      // Setters
      void setName(AISutil::String const &channame) { name=channame; }
      void setID(int chanid) { ID=chanid; }

      // Userlist manipulation
      void addUser(AISutil::String const &name);
      void delUser(AISutil::String const &name);
      void setUserStatusVoice(AISutil::String const &name, bool voice);
      void setUserStatusOp(AISutil::String const &name, bool op);

      // Validations
      bool isVoiced(AISutil::String const &name);
      bool isOpped(AISutil::String const &name);
      bool isInChannel(AISutil::String const &name);

      // Number of users in the channel
      int getNbUsers(void) { return users.size(); }
   };


   class ChannelList:public Singleton<ChannelList>
   {
     public:
              friend class Singleton<ChannelList>;

              ~ChannelList(void);

              void addChan(Channel *newchan);
              void delChan(AISutil::String const &name);
              Channel* getChan(AISutil::String const &name);

              bool chanExists(AISutil::String const &name);
              int  nbChans(void);

     private:
              ChannelList() {}
              map<AISutil::String, Channel*> chans;
   };


}; // namespace Exordium


#endif // _INCLUDE_EXORDIUM_CHANNEL_H_
