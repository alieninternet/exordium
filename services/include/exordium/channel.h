// This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
// $Author$                                                                                                        
// $Date$                                                                                                          
// $Id$

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
