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
		static String getChanCount(void);
	        static void internalDeVoice(String const &, String const &);
	        static void internalDeOp(String const &, String const &);
	        static void internalVoice(String const &, String const &);
	        static void internalOp(String const &, String const &);
	        static void internalAdd(String const &, String const &);
	        static void internalDel(String const &, String const &);
	        static int getChanID(String const &);   
	        static int getOnlineChanID(String const &);  
	        static bool isChanRegistered(String const &);
 	        static int getChanAccess(String const &, String const &);
	        static bool ifChanExists(String const &);
	        static int maxChannels(void);
	        static int maxChannelsUser(String const &);
	        static int maxChannelsAccess(void);
	        static String getChanName(int const &);
	        static void setTopic(String const &,String const &);
	        static void updateTopic(String const &, String const &);
	        static String getChanOwner(int const &);
	        static void chanAddAccess(String const &,String const &,String const &);
	        static void chanDelAccess(String const &, String const &);
	        static void deregisterChannel(String const &, String const &reason);
   	        static void synchChannels(void);
	        static void synchChannel(String const &, String const &, String const &);
	        static void registerChannel(String const &, String const &);
		static int ownedChannels(String const &); 
		static void addChanBan(int const &,String const &,String const &,int const &,String const &);
		static void banChan(String const &, String const &, String const &);
		static void RemoveBan(String const &, String const &, String const &);


	};


};

#endif
