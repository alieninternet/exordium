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

#include "exordium/reguser.h"
#include <aisutil/string.h>
#include <exordium/database/database.h>

using AISutil::String;

using namespace std;

namespace Exordium
{


// The password must arrive encrypted!
void RegisteredUser::setPass(String const &mypass)
{
  pass=mypass;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "password='"+mypass+"'", "ID="+ID);
}


void RegisteredUser::setEmail(String const &myemail)
{
  email=myemail;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "email='"+email+"'", "ID="+ID);

}


void RegisteredUser::setRegdate(String myregdate)
{
  regdate=myregdate;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "registered='"+myregdate+"'", "ID="+ID);
}


void RegisteredUser::setLastID(String mylastid)
{
  lastid=mylastid;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "lastid='"+lastid+"'", "ID="+ID);
}

void RegisteredUser::setLasthost(String const &mylasthost)
{
  lasthost=mylasthost;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "lasthost='"+mylasthost+"'", "ID="+ID);
}

void RegisteredUser::setPrivmsg(unsigned short int myprivmsg)
{
  privmsg=myprivmsg;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "privmsg="+myprivmsg, "ID="+ID);
}

void RegisteredUser::setLang(String const &mylang)
{
  lang=mylang;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "lang='"+mylang+"'", "ID="+ID);
}

void RegisteredUser::setIcq(String const &myicq)
{
  icq=myicq;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "icq='"+myicq+"'", "ID="+ID);
}

void RegisteredUser::setMsn(AISutil::String const &mymsn)
{
  msn=mymsn;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "msn='"+mymsn+"'", "ID="+ID);
}

void RegisteredUser::setUrl(AISutil::String const &myurl)
{
  url=myurl;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "url='"+myurl+"'", "ID="+ID);
}

void RegisteredUser::setYahoo(AISutil::String const &myyahoo)
{
  yahoo=myyahoo;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "yahoo='"+myyahoo+"'", "ID="+ID);
}

void RegisteredUser::setAim(AISutil::String const &myaim)
{
  aim=myaim;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "aim='"+myaim+"'", "ID="+ID);
}

void RegisteredUser::setQuitmsg(AISutil::String const &myquitmsg)
{
  quitmsg=myquitmsg;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "quitmsg='"+myquitmsg+"'", "ID="+ID);
}

void RegisteredUser::setModnick(unsigned short int mymodnick)
{
  modnick=mymodnick;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "modnick="+mymodnick, "ID="+ID);
}

void RegisteredUser::setDeopaway(unsigned short int mydeopaway)
{
  deopaway=mydeopaway;

  if(ID!=-1)
    databaseFwd.dbUpdate("nicks", "deopaway="+mydeopaway, "ID="+ID);
}


void RegisteredUser::addOwnedChannel(String const &name)
{
   if(!ownsChannel(name))
     ownedchans.push_back(name);
}

// NOTE to self: Not sure about this one - 
void RegisteredUser::delOwnedChannel(String const &name)
{
   //if(ownsChannel(name))

   list<String>::iterator pos=find(ownedchans.begin(), ownedchans.end(), name);

   if(pos!=ownedchans.end())
      ownedchans.erase(pos);
}

bool RegisteredUser::ownsChannel(String const &name)
{
   list<String>::iterator pos=find(ownedchans.begin(), ownedchans.end(), name);

   if(pos!=ownedchans.end())
     return true;
   else
     return false;
}

void RegisteredUserList::SyncDB(void)
{
   map<String, RegisteredUser*>::iterator useriter;

   RegisteredUser* user;

   for(useriter=regusers.begin(); useriter!=regusers.end(); ++useriter)
   {
      user=(*useriter).second;
      delete user;
   }

   regusers.clear();


   databaseFwd.dbSelect("nicks");

   user=NULL;

   while(!databaseFwd.eof())
   {
      user=new RegisteredUser(databaseFwd.dbGetValue(1));

      user->setID(databaseFwd.dbGetValue(0).toInt());
      user->setPass(databaseFwd.dbGetValue(2));
      user->setEmail(databaseFwd.dbGetValue(3));
      user->setRegdate(databaseFwd.dbGetValue(4));
      user->setLastID(databaseFwd.dbGetValue(5));
      user->setLasthost(databaseFwd.dbGetValue(6));
      user->setPrivmsg(databaseFwd.dbGetValue(7).toInt());
      user->setLang(databaseFwd.dbGetValue(8));
      user->setIcq(databaseFwd.dbGetValue(9));
      user->setMsn(databaseFwd.dbGetValue(10));
      user->setUrl(databaseFwd.dbGetValue(11));
      user->setDeopaway(databaseFwd.dbGetValue(12).toInt());
      user->setYahoo(databaseFwd.dbGetValue(13));
      user->setAim(databaseFwd.dbGetValue(14));
      user->setQuitmsg(databaseFwd.dbGetValue(15));
      user->setModnick(databaseFwd.dbGetValue(16).toInt());

      registerNick(user);

      databaseFwd.dbGetRow();
   }

   RegisteredUser *u;

   // Fill ownedchans vector
   for(useriter=regusers.begin(); useriter!=regusers.end(); ++useriter)
   {
      databaseFwd.dbSelect("name", "chans", "owner='"+ (*useriter).first +"'");

      while(!databaseFwd.eof())
      {
        u=regusers[(*useriter).first];
        u->addOwnedChannel(databaseFwd.dbGetValue());
        databaseFwd.dbGetRow();
      }
   }


}


RegisteredUserList::~RegisteredUserList(void)
{
   map<String, RegisteredUser*>::iterator useriter;

   RegisteredUser* user;

   for(useriter=regusers.begin(); useriter!=regusers.end(); ++useriter)
   {
      user=(*useriter).second;
      delete user;
   }

   regusers.clear();
}


void RegisteredUserList::registerNick(RegisteredUser *newregnick)
{
  if(newregnick!=NULL)
  {
    if(!isRegistered(newregnick->getName()))
    {
       databaseFwd.dbInsert("nicks", 
                "'','"+newregnick->getName()+"','"+newregnick->getPass().data()+"','"+newregnick->getEmail() + 
                "',NOW(),NOW(),'',0, 'english','0','None','http://www.peoplechat.org',0,'None Set','None Set','No Quit Message Recorded',1");


       if(databaseFwd.affectedRows()>0)
       {
          databaseFwd.dbSelect("id,registered,lastid", "nicks", "name='"+newregnick->getName()+"'");

          if(!databaseFwd.eof())
          {
            newregnick->setID(databaseFwd.dbGetValue().toInt());
            newregnick->setRegdate(databaseFwd.dbGetValue(1));
            newregnick->setLastID(databaseFwd.dbGetValue(2));
          }

          regusers[newregnick->getName()]==newregnick;
       }
    }
  }
}


void RegisteredUserList::unregisterNick(AISutil::String const &name)
{
   if(isRegistered(name))
   {
     RegisteredUser *myregnick=regusers[name];
     regusers.erase(name);
     delete myregnick;
   }

}


RegisteredUser* RegisteredUserList::getRegisteredNick(AISutil::String const &name)
{
  if(isRegistered(name))
    return regusers[name];
  else
    return NULL;
}


bool RegisteredUserList::isRegistered(AISutil::String const &name)
{
   if(regusers.find(name)!=regusers.end())
     return true;
   else
     return false;
}

int  RegisteredUserList::nbRegUsers(void)
{
   return regusers.size();
}


}; 

