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

#ifndef __CHANNEL_H_
# define __CHANNEL_H_

# include <kineircd/str.h>

namespace Exordium {
   class Services;
   
   class Channel {
    private:
      Services& services;
      
    public:
      Channel(Services& s)
	: services(s)
	{};
      
      Kine::String getChanCount(void);
      void setChanLog(Kine::String const &,bool const &);
      bool ChanLog(Kine::String const &);
      void internalDeVoice(Kine::String const &, Kine::String const &);
      void internalDeOp(Kine::String const &, Kine::String const &);
      void internalVoice(Kine::String const &, Kine::String const &);
      void internalOp(Kine::String const &, Kine::String const &);
      void internalAdd(Kine::String const &, Kine::String const &);
      void internalDel(Kine::String const &, Kine::String const &);
      int getChanID(Kine::String const &);   
      int getOnlineChanID(Kine::String const &);  
      bool isChanRegistered(Kine::String const &);
      int getChanAccess(Kine::String const &, Kine::String const &);
      bool ifChanExists(Kine::String const &);
      int maxChannels(void);
      int maxChannelsUser(Kine::String const &);
      int maxChannelsAccess(void);
      Kine::String getChanName(int const &);
      Kine::String getChanIDName(int const &);
      void setTopic(Kine::String const &,Kine::String const &);
      void updateTopic(Kine::String const &, Kine::String const &);
      Kine::String getChanOwner(int const &);
      void chanAddAccess(Kine::String const &,Kine::String const &,Kine::String const &);
      void chanDelAccess(Kine::String const &, Kine::String const &);
      void deregisterChannel(Kine::String const &, Kine::String const &reason);
      void synchChannels(void);
      void synchChannel(Kine::String const &, Kine::String const &, Kine::String const &);
      void registerChannel(Kine::String const &, Kine::String const &);
      int ownedChannels(Kine::String const &); 
      void addChanBan(int const &,Kine::String const &,Kine::String const &,int const &,Kine::String const &);
      void banChan(Kine::String const &, Kine::String const &, Kine::String const &);
      void RemoveBan(Kine::String const &, Kine::String const &, Kine::String const &);
   };
}; // namespace Exordium

// Complete the forwarded definition
# include "exordium/services.h"

#endif
