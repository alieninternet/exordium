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

#include "note.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include <map>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium;

namespace Exordium
{

   struct Note::functionTableStruct const
     Note::functionTable[] =
     {
	  {"send", &Note::parseSEND},
	  {"list", &Note::parseLIST},
	  {"read", &Note::parseREAD},
	  {"del", &Note::parseDEL},
	  {"delete", &Note::parseDEL},
	  {"erase", &Note::parseDEL},
	  {0, 0}
     };
   void
     NOTE_FUNC (Note::parseDEL)
       {
	  String num = tokens.nextToken();
	  if(num=="all")
	    {
               services->getDatabase().dbDelete("notes", "nto='"+origin.getNickname()+"'");
	       origin.sendMessage("All notes erased.",getName());
	       services->log(origin,getName(),"Erased all notes");
	       return;
	    }
          int nbRes = services->getDatabase().dbSelect("id", "notes", "nto='"+origin.getNickname()+"'");
	  int j = 0;

	  for(int i=0; i<nbRes; i++)
	    {
	       j++;
	       if(j==num.toInt())
		 {
		    String togo = String("Note #\002")+num+"\002 deleted";
		    origin.sendMessage(togo,getName());
		    String ntext = services->getDatabase().dbGetValue();
                    services->getDatabase().dbDelete("notes", "id='"+ntext+"'");
		    services->log(origin,"Note","Deleted a single note");
		    return;
		 }
               services->getDatabase().dbGetRow();
	    }
       }
   void
     NOTE_FUNC (Note::parseREAD)
       {
	  String num = tokens.nextToken();
	  if(num.toLower()=="all")
	    {
               int nbRes = services->getDatabase().dbSelect("nfrom,nsent,note", "notes", "nto='"+origin.getNickname()+"'");
	       int j = 0;
	       for(int i=0; i<nbRes; i++)
		 {
		    j++;
		    String nfrom = services->getDatabase().dbGetValue(0);
		    String nsent = services->getDatabase().dbGetValue(1);
		    String ntext = services->getDatabase().dbGetValue(2);
		    String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
		    origin.sendMessage(togo,getName());
		    String tofo = ntext;
		    origin.sendMessage(tofo,getName());
                    services->getDatabase().dbGetRow();
		 }
	       services->log(origin,"Note","Read all notes");
	       return;
	    }

          int nbRes = services->getDatabase().dbSelect("nfrom,nsent,note", "notes", "nto='"+origin.getNickname()+"'");

	  int j = 0;
	  for(int i=0; i<nbRes; i++)
	    {
	       j++;
	       if(j==num.toInt())
		 {
                    String nfrom = services->getDatabase().dbGetValue(0);
                    String nsent = services->getDatabase().dbGetValue(1);
                    String ntext = services->getDatabase().dbGetValue(2);
		    String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
		    origin.sendMessage(togo,getName());
		    String tofo = ntext;
		    origin.sendMessage(tofo,getName());
		    services->log(origin,"Note","Read a single note");
		    return;
		 }
                 services->getDatabase().dbGetRow();
	    }
	  origin.sendMessage("No such note!",getName());
	  return;
       }
   void
     NOTE_FUNC (Note::parseLIST)
       {
          int nbRes = services->getDatabase().dbSelect("nfrom,nsent,note", "notes", "nto='"+origin.getNickname()+"'");

	  int j = 0;
	  for(int i=0; i<nbRes; i++)
	    {
	       j++;
               String nfrom = services->getDatabase().dbGetValue(0);
               String nsent = services->getDatabase().dbGetValue(1);
               String ntext = services->getDatabase().dbGetValue(2);
	       String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
	       origin.sendMessage(togo,getName());
               services->getDatabase().dbGetRow();
	    }
	  if(j==0)
	    {
	       origin.sendMessage("You have no notes stored",getName());
	       return;
	    }
	  String tofo = String("To read a note, type /msg Note read Number");
	  origin.sendMessage(tofo,getName());
	  services->log(origin, "Note", "Listed their notes");
       }
   void
     NOTE_FUNC (Note::parseSEND)
       {
	  String nto = tokens.nextToken();
	  String note = tokens.rest();
	  if(nto=="")
	    {
	       String togo = String("Usage is: /msg note send Nickname/#Channel Your Message Here");
	       origin.sendMessage(String(togo),getName());
	       return;
	    }
	  if(note=="")
	    {
	       String togo = String("Usage is: /msg note send Nickname Your Message Here");
	       origin.sendMessage(String(togo),getName());
	       return;
	    }
	  String it = (nto[0]);
	  if(nto[0]=='#')
	    {
	       //Channel Note
	       if(!services->getChannel().isChanRegistered(nto))
		 {
		    String tofo = String("That channel is not registered");
		    origin.sendMessage(String(tofo),getName());
		    return;
		 }
	       int chanid = services->getChannel().getChanID(nto);

               int nbRes = services->getDatabase().dbSelect("nickid", "chanaccess", "chanid='"+String::convert(chanid) + "'");
	       int j = 0;
	       for(int i=0; i<nbRes; i++)
		 {
		    String nnid = services->getDatabase().dbGetValue();
		    int nid = nnid.toInt();
		    String nick = services->getNick(nid);
		    if(nick.toLower()!=origin.getNickname().toLower())
		      {
			 j++;
			 String txt = String("\002")+nto+"\002: "+note;
			 services->sendNote(origin.getNickname(),nick,txt);
		      }
                    services->getDatabase().dbGetRow();
		 }
	       String toao = String("Your note was successfully sent to \002")+String::convert(j)+"\002 people on "+nto;
	       origin.sendMessage(String(toao),getName());
	       services->log(origin,"Note","Sent a channel note to "+nto);
	       return;
	    }

	  if(!services->isNickRegistered(nto))
	    {
	       String togo = String("Error: Destination nickname is not registered");
	       origin.sendMessage(togo,getName());
	       return;
	    }
	  services->sendNote(origin.getNickname(),nto,note);
	  origin.sendMessage("Your note was successfully sent to \002"+nto+"\002",getName());
	  services->log(origin,"Note","Sent a private note to "+nto);
       }

   void
     Note::parseLine (StringTokens& line, User& origin, String const &ch)
       {
	  return;
       }

   void
     Note::parseLine (StringTokens& line, User& origin)
       {
	  StringTokens& st = line;
	  if(!origin.isIdentified(origin.getNickname()))
	    {
	       origin.sendMessage("Sorry - You must be identified to use this service",getName());
	       return;
	    }
	  String command = st.nextToken ().toLower ();
	  for (int i = 0; functionTable[i].command != 0; i++)
	    {
	       // Does this match?
	       if (command == functionTable[i].command)
		 {
		    // Run the command and leave
		    (this->*(functionTable[i].function))(origin, st);
		    return;
		 }
	    }
	  origin.sendMessage ("Unrecognized Command",getName());
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	return new Note();
     }

   // Module information structure
   const Note::moduleInfo_type Note::moduleInfo = {
      "Note Service",
	0, 0,
	Exordium::Service::moduleInfo_type::Events::NONE
   };


   // Start the service
   void Note::start(Exordium::Services& s)
     {
	// Set the services field appropriately
	services = &s;
	
	// Register ourself to the network
	services->registerService(getName(), getName(), 
				 getConfigData().getHostname(), "+dz",
				 getConfigData().getDescription());
	services->serviceJoin(getName(),"#Debug");
     }
}

