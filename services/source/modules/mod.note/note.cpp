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

#include "note.h"
#include "tables.h"
#include <exordium/channel.h>
#include <exordium/database/database.h>
#include <exordium/database/base.h>

#include "language.h"

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::NoteModule;


const Service::functionTableStruct Service::functionTable[] = {
     { "send",		&Service::parseSEND },
     { "list",		&Service::parseLIST },
     { "read",		&Service::parseREAD },
     { "del",		&Service::parseDEL },
     { "delete",	&Service::parseDEL },
     { "erase",		&Service::parseDEL },
     { 0, 0 }
};


NOTE_FUNC(Service::parseDEL)
{
   String num = tokens.nextToken();
   
   if (num == "all") {
      services.getDatabase().dbDelete("notes",
				       "nto='" + origin.getNickname() + "'");
      origin.sendMessage(GETLANG(note_DEL_NOTES_ERASED), getNickname());
      services.log(origin,getNickname(),"Erased all notes");
      return;
   }
   
   int nbRes = services.getDatabase().dbSelect("id", "notes",
						"nto='" + 
						origin.getNickname() + "'");
   
   CResult *myRes = services.getDatabase().dbGetResultSet();
   
   int j = 0;
   
   for (int i = 0; i < nbRes; i++) {
      j++;
      
      if (j == num.toInt()) {
	 origin.sendMessage(GETLANG(note_DEL_NOTE_ERASED,num),
			    getNickname());
	 String ntext = myRes->getValue(i,0);
	 services.getDatabase().dbDelete("notes", "id='" + ntext + "'");
	 services.log(origin, "Note", "Deleted a single note");
	 return;
      }
   }
   delete myRes;
}


NOTE_FUNC(Service::parseREAD)
{
   String num = tokens.nextToken();
   
   if(num.toLower() == "all") {
      int nbRes = services.getDatabase().dbSelect("nfrom,nsent,note", "notes",
						   "nto='" + 
						   origin.getNickname() + "'");
      CResult *myRes = services.getDatabase().dbGetResultSet();
      int j = 0;
      
      for (int i = 0; i < nbRes; i++) {
	 j++;
	 String nfrom = myRes->getValue(i,0);
	 String nsent = myRes->getValue(i,1);
	 String ntext = myRes->getValue(i,2);
	 origin.sendMessage(GETLANG(note_READ_NOTE_ONE,String::convert(j),
				    nfrom,
				    nsent),getNickname());
	 origin.sendMessage(GETLANG(note_READ_NOTE_TWO,ntext), getNickname());
      }
      services.log(origin, "Note", "Read all notes");
      delete myRes;
      return;
   }

   int nbRes = services.getDatabase().dbSelect("nfrom,nsent,note", "notes", "nto='"+origin.getNickname()+"'");
   CResult *myRes = services.getDatabase().dbGetResultSet();
   int j = 0;
   for (int i = 0; i < nbRes; i++) {
      j++;
      if (j == num.toInt()) {
	 String nfrom = myRes->getValue(i,0);
	 String nsent = myRes->getValue(i,1);
	 String ntext = myRes->getValue(i,2);
	 String togo = GETLANG(note_READ_NOTE_ONE,
			       String::convert(j),
			       nfrom,
			       nsent);
	 origin.sendMessage(togo,getNickname());
	 origin.sendMessage(GETLANG(note_READ_NOTE_TWO,ntext),getNickname());
	 services.log(origin, "Note", "Read a single note");
	 return;
      }
   }
   
   origin.sendMessage(GETLANG(note_READ_NOTE_NO_SUCH_NOTE), getNickname());
   return;

}


NOTE_FUNC(Service::parseLIST) {
   int nbRes = services.getDatabase().dbSelect("nfrom,nsent,note", "notes",
						"nto='" + 
						origin.getNickname() + "'");
   CResult *myRes = services.getDatabase().dbGetResultSet();
   int j = 0;
   for(int i=0; i<nbRes; i++) {
      j++;
      String nfrom = myRes->getValue(i,0);
      String nsent = myRes->getValue(i,1);
      String ntext = myRes->getValue(i,2);
      String togo = GETLANG(note_READ_NOTE_ONE,
			    String::convert(j),
			    nfrom,
			    nsent);
       origin.sendMessage(togo, getNickname());
   }
   delete myRes;
   if(j == 0) {
      origin.sendMessage(GETLANG(note_LIST_NOTES_NO_NOTES), getNickname());
      return;
   }
   origin.sendMessage(GETLANG(note_LIST_INSTRUCTIONS), getNickname());
   services.log(origin, "Note", "Listed their notes");
}


NOTE_FUNC(Service::parseSEND) {
   String nto = tokens.nextToken();
   String note = tokens.rest();
   
   if (nto == "" || note=="") {
      origin.sendMessage(GETLANG(note_SEND_USAGE),getNickname());
      return;
   }
   
   String it = (nto[0]);
   if(nto[0] == '#') {
      //Channel Note
      if(!services.getChannel().isChanRegistered(nto)) {
	 origin.sendMessage(GETLANG(ERROR_CHANNEL_NOT_REGISTERED),getNickname());
	 return;
      }
      
      int chanid = services.getChannel().getChanID(nto);
      
      int nbRes = services.getDatabase().dbSelect("nickid", "chanaccess", 
						   "chanid='" + 
						   String::convert(chanid) + 
 						   "'");
      CResult *myRes = services.getDatabase().dbGetResultSet();
      
      int j = 0;
      for (int i = 0; i < nbRes; i++) {
	 String nnid = myRes->getValue(i,0);
	 int nid = nnid.toInt();
	 String nick = services.getNick(nid);
	 
	 if (nick.toLower() != origin.getNickname().toLower()) {
	    j++;
	    String txt = String("\002") + nto + "\002: " + note;
	    services.sendNote(origin.getNickname(), nick, txt);
	 }

      }
      
      origin.sendMessage(GETLANG(note_SEND_CHANNEL_SUCCESS,String::convert(j),nto), getNickname());
      services.log(origin, "Note","Sent a channel note to " + nto);
      delete myRes;
      return;
   }
   
   if(!services.isNickRegistered(nto)) {
      origin.sendMessage(GETLANG(ERROR_NICK_NOT_REGISTERED),
			 getNickname());
      return;
   }
   
   services.sendNote(origin.getNickname(), nto, note);
   origin.sendMessage(GETLANG(note_SEND_USER_SUCCESS,nto),
		      getNickname());
   services.log(origin, "Note", "Sent a private note to " + nto);
}


void Service::parseLine(StringTokens& line, User& origin, const bool safe)
{
   if (!origin.isIdentified(origin.getNickname())) {
      origin.sendMessage(GETLANG(ERROR_NICK_NOT_IDENTIFIED),
			 getNickname());
      return;
   }
   
   String command = line.nextToken().toLower();
   
   for (int i = 0; functionTable[i].command != 0; i++) {
      // Does this match?
      if (command == functionTable[i].command) {
	 // Run the command and leave
	 (this->*(functionTable[i].function))(origin, line);
	 return;
      }
   }
   
   origin.sendMessage(GETLANG(ERROR_UNKNOWN_COMMAND), getNickname());
}
