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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "exordium/channel.h"
#include "kineircd/str.h"

using AISutil::String;

#define NORMAL 0
#define VOICED 0x00000001
#define OPPED  0x00000002

namespace Exordium
{


void Channel::addUser(AISutil::String const &name)
{
  if(!isInChannel(name))
  {
     users[name] = NORMAL;
  }
}

void Channel::setUserStatusOp(AISutil::String const &name, bool op)
{
   if(isInChannel(name))
   {
     if(op==true)
       users[name] &= OPPED;
     else
       users[name] |= OPPED;
   }
}


void Channel::setUserStatusVoice(AISutil::String const &name, bool voice)
{
   if(isInChannel(name))
   {
     if(voice==true)
       users[name] &= VOICED;
     else
       users[name] |= VOICED;
   }
}


void Channel::delUser(AISutil::String const &name)
{
  if(isInChannel(name))
  {
     users.erase(name);
  }
}


bool Channel::isVoiced(AISutil::String const &name)
{
  if(isInChannel(name))
  {
     if(users[name] & VOICED)
       return true;
     else
       return false;
  }
}

bool Channel::isOpped(AISutil::String const &name)
{
  if(isInChannel(name))
  {
     if(users[name] & OPPED)
       return true;
     else
       return false;
  }
}


bool Channel::isInChannel(AISutil::String const &name)
{
  if(users.find(name) != users.end())
    return true;
  else
    return false;
}



ChannelList::~ChannelList(void)
{
  map<String, Channel*>::iterator chaniter;

  for(chaniter=chans.begin(); chaniter!=chans.end(); ++chaniter)
  {
     delete (*chaniter).second;
  }

  chans.clear();
}

void ChannelList::addChan(Channel *newchan)
{
  if(newchan!=NULL)
    if(!chanExists(newchan->getName()))
      chans[newchan->getName()]=newchan;
}

void ChannelList::delChan(AISutil::String const &name)
{
  if(chanExists(name))
  {
    Channel *chan=chans[name];
    chans.erase(name);
    delete chan;
  }
  
// NOTE to self: add else for inconsistency

}


Channel* ChannelList::getChan(AISutil::String const &name)
{
  if(chanExists(name))
    return chans[name];
  else
    return NULL;
}

bool ChannelList::chanExists(AISutil::String const &name)
{
  if(chans.find(name)!=chans.end())
    return true;
  else
    return false;
}


int  ChannelList::nbChans(void)
{
  return chans.size();
}


}

