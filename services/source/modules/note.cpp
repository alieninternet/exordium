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

#include "include/note.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include "exordium/sql.h"
#include <map>

using LibAIS::String;
using LibAIS::StringTokens;
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
	       String query = "DELETE from notes where nto='" + origin.getNickname() + "'";
	       services.getDatabase().query(query);
	       origin.sendMessage("All notes erased.",getName());
	       services.log(origin,getName(),"Erased all notes");
	       return;
	    }
	  String query = "SELECT id from notes where nto='" + origin.getNickname() + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  int j = 0;
	  while ((row = res.fetch_row()))
	    {
	       j++;
	       if(j==num.toInt())
		 {
		    String togo = String("Note #\002")+num+"\002 deleted";
		    origin.sendMessage(togo,getName());
		    String ntext = ((std::string) row[0]).c_str();
		    String query = "DELETE from notes where id='" + ntext + "'";
		    services.getDatabase().query(query);
		    services.log(origin,"Note","Deleted a single note");
		    res.free_result();
		    return;
		 }

	    }
       }
   void
     NOTE_FUNC (Note::parseREAD)
       {
	  String num = tokens.nextToken();
	  if(num.toLower()=="all")
	    {
	       String query = "SELECT nfrom,nsent,note from notes where nto='" + origin.getNickname() + "'";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       int j = 0;
	       while ((row = res.fetch_row()))
		 {
		    j++;
		    String nfrom = ((std::string) row[0]).c_str();
		    String nsent = ((std::string) row[1]).c_str();
		    String ntext = ((std::string) row[2]).c_str();
		    String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
		    origin.sendMessage(togo,getName());
		    String tofo = ntext;
		    origin.sendMessage(tofo,getName());
		 }
	       services.log(origin,"Note","Read all notes");
	       res.free_result();
	       return;
	    }
	  String query = "SELECT nfrom,nsent,note from notes where nto='" + origin.getNickname() + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  int j = 0;
	  while ((row = res.fetch_row()))
	    {
	       j++;
	       if(j==num.toInt())
		 {
		    String nfrom = ((std::string) row[0]).c_str();
		    String nsent = ((std::string) row[1]).c_str();
		    String ntext = ((std::string) row[2]).c_str();
		    String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
		    origin.sendMessage(togo,getName());
		    String tofo = ntext;
		    origin.sendMessage(tofo,getName());
		    services.log(origin,"Note","Read a single note");
		    res.free_result();
		    return;
		 }
	    }
	  res.free_result();
	  origin.sendMessage("No such note!",getName());
	  return;
       }
   void
     NOTE_FUNC (Note::parseLIST)
       {
	  String query = "select nfrom,nsent,note from notes where nto='" + origin.getNickname() + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  int j = 0;
	  while ((row = res.fetch_row()))
	    {
	       j++;
	       String nfrom = ((std::string) row[0]).c_str();
	       String nsent = ((std::string) row[1]).c_str();
	       String ntext = ((std::string) row[2]).c_str();
	       String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
	       origin.sendMessage(togo,getName());
	    }
	  if(j==0)
	    {
	       origin.sendMessage("You have no notes stored",getName());
	       return;
	    }
	  String tofo = String("To read a note, type /msg Note read Number");
	  origin.sendMessage(tofo,getName());
	  services.log(origin, "Note", "Listed their notes");
	  res.free_result();
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
	       if(!services.getChannel().isChanRegistered(nto))
		 {
		    String tofo = String("That channel is not registered");
		    origin.sendMessage(String(tofo),getName());
		    return;
		 }
	       int chanid = services.getChannel().getChanID(nto);
	       String query = "SELECT nickid from chanaccess where chanid='" + String::convert(chanid) + "'";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       int j = 0;
	       while ((row = res.fetch_row()))
		 {
		    String nnid = ((std::string) row[0]).c_str();
		    int nid = nnid.toInt();
		    String nick = services.getNick(nid);
		    if(nick.toLower()!=origin.getNickname().toLower())
		      {
			 j++;
			 String txt = String("\002")+nto+"\002: "+note;
			 services.sendNote(origin.getNickname(),nick,txt);
		      }
		 }
	       String toao = String("Your note was successfully sent to \002")+String::convert(j)+"\002 people on "+nto;
	       origin.sendMessage(String(toao),getName());
	       services.log(origin,"Note","Sent a channel note to "+nto);
	       res.free_result();
	       return;
	    }

	  if(!services.isNickRegistered(nto))
	    {
	       String togo = String("Error: Destination nickname is not registered");
	       origin.sendMessage(togo,getName());
	       return;
	    }
	  services.sendNote(origin.getNickname(),nto,note);
	  origin.sendMessage("Your note was successfully sent to \002"+nto+"\002",getName());
	  services.log(origin,"Note","Sent a private note to "+nto);
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
	return new Note(services);
     }

   // Module information structure
   const Note::moduleInfo_type Note::moduleInfo = {
      "Note Service",
	0, 0
   };


   // Start the service
   void Note::start(void)
     {
	services.registerService(getName(), getName(), 
				 getConfigData().getHostname(), "+dz",
				 getConfigData().getDescription());
	services.serviceJoin(getName(),"#Debug");
     }
}

