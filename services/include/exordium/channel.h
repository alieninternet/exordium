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

      ~Channel(void)
	{};
      
      AISutil::String getChanCount(void);
      void setChanLog(AISutil::String const &,bool const &);
      bool ChanLog(AISutil::String const &);
      void internalDeVoice(AISutil::String const &, AISutil::String const &);
      void internalDeOp(AISutil::String const &, AISutil::String const &);
      void internalVoice(AISutil::String const &, AISutil::String const &);
      void internalOp(AISutil::String const &, AISutil::String const &);
      void internalAdd(AISutil::String const &, AISutil::String const &);
      void internalDel(AISutil::String const &, AISutil::String const &);
      int getChanID(AISutil::String const &);   
      int getOnlineChanID(AISutil::String const &);  
      bool isChanRegistered(AISutil::String const &);
      int getChanAccess(AISutil::String const &, AISutil::String const &);
      bool ifChanExists(AISutil::String const &);
      int maxChannels(void);
      int maxChannelsUser(AISutil::String const &);
      int maxChannelsAccess(void);
      AISutil::String getChanName(int const &);
      AISutil::String getChanIDName(int const &);
      void setTopic(AISutil::String const &,AISutil::String const &);
      void updateTopic(AISutil::String const &, AISutil::String const &);
      AISutil::String getChanOwner(int const &);
      void chanAddAccess(AISutil::String const &,AISutil::String const &,AISutil::String const &);
      void chanDelAccess(AISutil::String const &, AISutil::String const &);
      void deregisterChannel(AISutil::String const &);
      void synchChannels(void);
      void synchChannel(AISutil::String const &, AISutil::String const &, AISutil::String const &);
      void registerChannel(AISutil::String const &, AISutil::String const &);
      int ownedChannels(AISutil::String const &); 
      void addChanBan(int const &,AISutil::String const &,AISutil::String const &,int const &,AISutil::String const &);
      void banChan(AISutil::String const &, AISutil::String const &, AISutil::String const &);
      void RemoveBan(AISutil::String const &, AISutil::String const &, AISutil::String const &);
      AISutil::String getModes(void);
      int getNbUsers(AISutil::String const &);
   };
}; // namespace Exordium

// Complete the forwarded definition
# include <exordium/services.h>

#endif // _INCLUDE_EXORDIUM_CHANNEL_H_
