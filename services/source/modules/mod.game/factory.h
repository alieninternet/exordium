/* $Id: 
 * 
 * Exordium Network Services
 * Copyright (C) 2002 IRCDome Development Team
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _SOURCE_MODULES_GAME_FACTORY_H_
# define _SOURCE_MODULES_GAME_FACTORY_H_ 1

# include <dlfcn.h>
# include <map>

# include "channelgame.h"

class ChannelGame;

namespace Exordium {
  namespace GameModule {
    typedef ChannelGame* (*GameCallback)(Exordium::GameModule::Module& module,
        const AIS::Util::String& channel, Exordium::User& caller);

    class Factory {
      private:
        typedef std::map<const AIS::Util::String, GameCallback> CallbackMap;
        CallbackMap callbacks;
  
        Factory() { }
        Factory(const Factory&) { };
        Factory& operator = (const Factory&) { return *this; }

      public:
        static Factory& Instance()
        {
          static Factory factory;
          return factory;
        }
        ~Factory() { }

        bool registerType(const AIS::Util::String& name, GameCallback createFn)
        {
          return callbacks.insert(CallbackMap::value_type(name, createFn)).second;
        };

        bool registerModule(const AIS::Util::String& fileName) 
        {
          // Try to load the module
          void* const handle = dlopen(fileName.c_str(), RTLD_NOW);
  
          // Check if that loaded okay
          if (handle == 0)
          {
            // Set the error string appropriately
            std::cerr << "Could not load " << fileName.c_str() << ": " << dlerror()
              << std::endl;
            return false;
          }
  
          CHANNEL_GAME_CREATOR_FUNC_NO_EXTERN((* const initfunc)) = 
            ((CHANNEL_GAME_CREATOR_FUNC_NO_EXTERN((*)))
             (dlsym(handle, "game_init")));
  
          if(initfunc == 0)
          {
            std::cerr << "Could not load " << fileName << 
              ": Module does not contain an initialisation function\n";
            return false;
          }
  
          // Extract the name of the game from the filename
          AIS::Util::String::size_type pos = fileName.find("game_",0) + 5;

          AIS::Util::String name = fileName.substr(pos, 
              (fileName.length() - pos - 3));

#ifdef DEBUG
          std::cerr << "Game to load is called " << name << std::endl;
#endif
          return registerType(name, initfunc);
        }

        bool unregisterType(const AIS::Util::String& name)
        {
          return callbacks.erase(name);
        };
  
        bool unregisterModule(const AIS::Util::String& name)
        {
        };

        const std::list<AIS::Util::String> listModules(void)
        {
          std::list<AIS::Util::String> retList;
          for(CallbackMap::const_iterator iter = callbacks.begin(); 
              iter != callbacks.end(); iter++)
          {
            retList.push_back((*iter).first);
          }

          return retList;
        };

        ChannelGame* createGame(const AIS::Util::String& name, 
            Exordium::GameModule::Module& module, 
            const AIS::Util::String& channel, Exordium::User& caller)
        {
          CallbackMap::const_iterator iter = callbacks.find(name);
          if(iter == callbacks.end())
          {
#ifdef DEBUG
            std::cerr << "Game " << name << " not found\n";
#endif
            return 0;
          }
  
          ChannelGame* game = iter->second(module,channel,caller);
          return game;
        };
     };
  }; // Namespace GameModule
}; // Namespace Exordium

// Complete the forwarded declaration
# include "channelgame.h"
   
#endif // _SOURCE_MODULES_GAME_FACTORY_H_
