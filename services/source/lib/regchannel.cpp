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

#include "exordium/regchannel.h"
#include <aisutil/string.h>
#include <exordium/reguser.h>
#include <exordium/database/database.h>

using AISutil::String;


namespace Exordium
{


void RegisteredChannel::setOwner (String const &myowner)
{
  owner=myowner;

  if(ID!=-1)
    databaseFwd.dbUpdate("chans", "owner='"+myowner+"'", "id="+ID);
}


void RegisteredChannel::setTopic (String const &mytopic)
{
  topic=mytopic;

  if(ID!=-1)
    databaseFwd.dbUpdate("chans", "topic='"+mytopic+"'", "id="+ID);
}


void RegisteredChannel::setModes (String const &mymodes)
{
   modes=mymodes;

  if(ID!=-1)
    databaseFwd.dbUpdate("chans", "modes='"+mymodes+"'", "id="+ID);
}


void RegisteredChannel::setCdesc (String const &mycdesc)
{ 
  cdesc=mycdesc;

  if(ID!=-1)
     databaseFwd.dbUpdate("chans", "cdesc='"+mycdesc+"'", "id="+ID);

}


void RegisteredChannel::setUrl   (String const &myurl)
{
  url=myurl;

  if(ID!=-1)
    databaseFwd.dbUpdate("chans", "url='"+myurl+"'", "id="+ID);

}


void RegisteredChannel::setClog  (int myclog)
{
  clog=myclog;

  if(ID!=-1)
    databaseFwd.dbUpdate("chans", "clog="+String::convert(myclog), "id="+ID);
}



int RegisteredChannel::getChanAccess(String const &nick)
{
   if(access.find(nick) != access.end())
   {
      return access[nick];
   }
   else
      return 0;
}

void RegisteredChannel::chanAddAccess(String const &nick,int accesslvl)
{
   if(access.find(nick) == access.end())
   {
      RegisteredUser *user=regUserListFwd.getRegisteredNick(nick);

      if(user!=NULL)
      {
        databaseFwd.dbInsert("chanaccess", "'"+String::convert(ID)+"', '"+ String::convert(user->getID()) + "', '"+
           String::convert(accesslvl+"'"));

        if(databaseFwd.affectedRows()>0)
          access[nick]=accesslvl;
      }
   }
}

void RegisteredChannel::chanDelAccess(String const &nick)
{
   if(access.find(nick) != access.end())
   {
      RegisteredUser *user=regUserListFwd.getRegisteredNick(nick);

      if(user!=NULL)
      {
        databaseFwd.dbDelete("chanaccess", "chanid="+String::convert(ID)+" AND nickid="+
           String::convert(user->getID()));

        if(databaseFwd.affectedRows()>0)
          access.erase(nick);
      }
   }

}

void RegisteredChannel::addChanBan(String const &mask,String const &setby,int const &expireon,String const &reason)
{
  databaseFwd.dbInsert("chanbans",
           "'','"+String::convert(ID)+"','"+mask+"','"+setby+"',NOW(),'"+String::convert(expireon)+"','"+reason+"'");

  if(databaseFwd.affectedRows()>0)
    {
      chanbans cb;

      databaseFwd.dbSelect("chanbans", "id", "mask='"+mask+"' AND chan='"+String::convert(ID)+"'");

      if(!databaseFwd.eof())
         cb.ID=databaseFwd.dbGetValue().toInt();
      else
         cb.ID=0;

      cb.chanid=ID;
      cb.mask=mask;
      cb.setby=setby;
      cb.seton=time(NULL);
      cb.expireon=expireon;
      cb.reason=reason;

      bans.push_back(cb);
    }
}

void RegisteredChannel::banChan(String const &, String const &, String const &)
{

}



void RegisteredChannel::RemoveBan(String const &mask)
{

   for(int i=0; i<bans.size(); i++)
   {
      if(bans[i].chanid==ID && bans[i].mask==mask)
      {
         databaseFwd.dbDelete("chanbans", "id="+String::convert(bans[i].ID));
      
         if(databaseFwd.affectedRows()>0)
           bans.erase(bans.begin()+i);

         break;
      }
   }
}


void RegisteredChannelList::SyncDB(void)
{
   map<String, RegisteredChannel*>::iterator chaniter;

   RegisteredChannel* chan;

   for(chaniter=regchans.begin(); chaniter!=regchans.end(); ++chaniter)
   {
      chan=(*chaniter).second;
      delete chan;
   }

   regchans.clear();


   databaseFwd.dbSelect("chans");

   chan=NULL;

   while(!databaseFwd.eof())
   {
      chan=new RegisteredChannel(databaseFwd.dbGetValue(1));

      chan->setID(databaseFwd.dbGetValue(0).toInt());
      chan->setOwner(databaseFwd.dbGetValue(2));
      chan->setTopic(databaseFwd.dbGetValue(3));
      chan->setModes(databaseFwd.dbGetValue(4));
      chan->setCdesc(databaseFwd.dbGetValue(5));
      chan->setUrl(databaseFwd.dbGetValue(6));
      chan->setClog(databaseFwd.dbGetValue(7).toInt());

      registerChannel(chan);

      databaseFwd.dbGetRow();
   }

   // NOTE: add chanbans

}


RegisteredChannelList::~RegisteredChannelList(void)
{
   map<String, RegisteredChannel*>::iterator chaniter;

   RegisteredChannel* chan;

   for(chaniter=regchans.begin(); chaniter!=regchans.end(); ++chaniter)
   {
      chan=(*chaniter).second;
      delete chan;
   }

   regchans.clear();
}


void RegisteredChannelList::registerChannel(RegisteredChannel *newregchan)
{
  if(newregchan!=NULL)
  {
    if(!isRegistered(newregchan->getName()))
    {
       databaseFwd.dbInsert("chans", "'','"+newregchan->getName()+"','"+newregchan->getOwner()+"','"+newregchan->getTopic()+
          "','"+newregchan->getModes()+"','"+newregchan->getCdesc()+"','"+newregchan->getUrl()+"','"+
          String::convert(newregchan->getClog())+"'");

       if(databaseFwd.affectedRows()>0)
       {
          databaseFwd.dbSelect("id", "chans", "name='"+newregchan->getName()+"'");

          if(!databaseFwd.eof())
            newregchan->setID(databaseFwd.dbGetValue().toInt());

          regchans[newregchan->getName()]==newregchan;
       }
    }
  }
}


void RegisteredChannelList::registerChannel(String const &myname, String const &myowner, String const &mytopic,
                                 String const &mymodes, String const &mycdesc, String const &myurl, int myclog)
{
  if(!isRegistered(myname))
  {
    RegisteredChannel *newregchan=new RegisteredChannel(myname);

    newregchan->setOwner(myowner);
    newregchan->setTopic(mytopic);
    newregchan->setModes(mymodes);
    newregchan->setCdesc(mycdesc);
    newregchan->setUrl(myurl);
    newregchan->setClog(myclog);

    registerChannel(newregchan);
  }
}


void RegisteredChannelList::unregisterChannel(AISutil::String const &name)
{
   if(isRegistered(name))
   {
     RegisteredChannel *myregchan=regchans[name];
     regchans.erase(name);
     delete myregchan;
   }

}


RegisteredChannel* RegisteredChannelList::getRegisteredChannel(AISutil::String const &name)
{
  if(isRegistered(name))
    return regchans[name];
  else
    return NULL;
}

RegisteredChannel* RegisteredChannelList::getRegisteredChannelByID(int id)
{
   map<AISutil::String, RegisteredChannel*>::iterator iter;

   for(iter=regchans.begin(); iter!=regchans.end(); ++iter)
   {
      if(iter->second->getID()==id)
        return iter->second;
   }

   return NULL;
}

bool RegisteredChannelList::isRegistered(AISutil::String const &name)
{
   if(regchans.find(name)!=regchans.end())
     return true;
   else
     return false;
}

int  RegisteredChannelList::nbRegChans(void)
{
   return regchans.size();
}


}

