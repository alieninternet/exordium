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

#ifndef _INCLUDE_EXORDIUM_CHANNEL_H_
# define _INCLUDE_EXORDIUM_CHANNEL_H_ 1

# include <aisutil/string.h>
# include <kineircd/name.h>

namespace Exordium {
   class Services;
   
   class Channel {
    private:
      Services& services;
      
    public:
      Channel(Services& s)
	: services(s)
	{};

      ~Channel(void)
	{};
      
      AIS::Util::String getChanCount(void);
      void setChanLog(AIS::Util::String const &,bool const &);
      bool ChanLog(AIS::Util::String const &);
      int getChanID(AIS::Util::String const &);   
      int getOnlineChanID(AIS::Util::String const &);  
      bool isChanRegistered(AIS::Util::String const &);
      int getChanAccess(AIS::Util::String const &, AIS::Util::String const &);
      bool ifChanExists(AIS::Util::String const &);
      int maxChannels(void);
      int maxChannelsUser(AIS::Util::String const &);
      int maxChannelsAccess(void);
      Kine::Name getChanName(int const &);
      AIS::Util::String getChanIDName(int const &);
      void setTopic(AIS::Util::String const &,AIS::Util::String const &);
      void updateTopic(AIS::Util::String const &, AIS::Util::String const &);
      AIS::Util::String getChanOwner(int const &);
      void chanAddAccess(AIS::Util::String const &,AIS::Util::String const &,AIS::Util::String const &);
      void chanDelAccess(AIS::Util::String const &, AIS::Util::String const &);
      void deregisterChannel(const Kine::Name&);
      void synchChannels(void);
      void synchChannel(AIS::Util::String const &, AIS::Util::String const &, AIS::Util::String const &);
      void registerChannel(const Kine::Name&, AIS::Util::String const &);
      int ownedChannels(AIS::Util::String const &); 
      void addChanBan(int const &,AIS::Util::String const &,AIS::Util::String const &,int const &,AIS::Util::String const &);
      void banChan(AIS::Util::String const &, AIS::Util::String const &, AIS::Util::String const &);
      void RemoveBan(AIS::Util::String const &, AIS::Util::String const &, AIS::Util::String const &);
      AIS::Util::String getModes(void);
      int getNbUsers(AIS::Util::String const &);
   };
}; // namespace Exordium

// Complete the forwarded definition
# include <exordium/services.h>

#endif // _INCLUDE_EXORDIUM_CHANNEL_H_
