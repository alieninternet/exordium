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
      
      LibAIS::String getChanCount(void);
      void setChanLog(LibAIS::String const &,bool const &);
      bool ChanLog(LibAIS::String const &);
      void internalDeVoice(LibAIS::String const &, LibAIS::String const &);
      void internalDeOp(LibAIS::String const &, LibAIS::String const &);
      void internalVoice(LibAIS::String const &, LibAIS::String const &);
      void internalOp(LibAIS::String const &, LibAIS::String const &);
      void internalAdd(LibAIS::String const &, LibAIS::String const &);
      void internalDel(LibAIS::String const &, LibAIS::String const &);
      int getChanID(LibAIS::String const &);   
      int getOnlineChanID(LibAIS::String const &);  
      bool isChanRegistered(LibAIS::String const &);
      int getChanAccess(LibAIS::String const &, LibAIS::String const &);
      bool ifChanExists(LibAIS::String const &);
      int maxChannels(void);
      int maxChannelsUser(LibAIS::String const &);
      int maxChannelsAccess(void);
      LibAIS::String getChanName(int const &);
      LibAIS::String getChanIDName(int const &);
      void setTopic(LibAIS::String const &,LibAIS::String const &);
      void updateTopic(LibAIS::String const &, LibAIS::String const &);
      LibAIS::String getChanOwner(int const &);
      void chanAddAccess(LibAIS::String const &,LibAIS::String const &,LibAIS::String const &);
      void chanDelAccess(LibAIS::String const &, LibAIS::String const &);
      void deregisterChannel(LibAIS::String const &, LibAIS::String const &reason);
      void synchChannels(void);
      void synchChannel(LibAIS::String const &, LibAIS::String const &, LibAIS::String const &);
      void registerChannel(LibAIS::String const &, LibAIS::String const &);
      int ownedChannels(LibAIS::String const &); 
      void addChanBan(int const &,LibAIS::String const &,LibAIS::String const &,int const &,LibAIS::String const &);
      void banChan(LibAIS::String const &, LibAIS::String const &, LibAIS::String const &);
      void RemoveBan(LibAIS::String const &, LibAIS::String const &, LibAIS::String const &);
      LibAIS::String getModes(void);
   };
}; // namespace Exordium

// Complete the forwarded definition
# include "exordium/services.h"

#endif
