// This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
// $Author$                                                                                                        
// $Date$                                                                                                          
// $Id$

#ifndef __CHANNEL_H_
#define __CHANNEL_H_

#include <kineircd/str.h>

using Kine::String;

namespace Exordium {
	class Channel {
		public:
		 String getChanCount(void);
	         void internalDeVoice(String const &, String const &);
	         void internalDeOp(String const &, String const &);
	         void internalVoice(String const &, String const &);
	         void internalOp(String const &, String const &);
	         void internalAdd(String const &, String const &);
	         void internalDel(String const &, String const &);
	         int getChanID(String const &);   
	         int getOnlineChanID(String const &);  
	         bool isChanRegistered(String const &);
 	         int getChanAccess(String const &, String const &);
	         bool ifChanExists(String const &);
	         int maxChannels(void);
	         int maxChannelsUser(String const &);
	         int maxChannelsAccess(void);
	         String getChanName(int const &);
	         String getChanIDName(int const &);
	         void setTopic(String const &,String const &);
	         void updateTopic(String const &, String const &);
	         String getChanOwner(int const &);
	         void chanAddAccess(String const &,String const &,String const &);
	         void chanDelAccess(String const &, String const &);
	         void deregisterChannel(String const &, String const &reason);
   	         void synchChannels(void);
	         void synchChannel(String const &, String const &, String const &);
	         void registerChannel(String const &, String const &);
		 int ownedChannels(String const &); 
		 void addChanBan(int const &,String const &,String const &,int const &,String const &);
		 void banChan(String const &, String const &, String const &);
		 void RemoveBan(String const &, String const &, String const &);


	};


};

#endif
