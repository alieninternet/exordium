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
#include <kineircd/str.h>

#include "language.h"

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::NoteModule;


const Module::functionTableStruct Module::functionTable[] = {
     { "send",		&Module::parseSEND },
     { "list",		&Module::parseLIST },
     { "read",		&Module::parseREAD },
     { "del",		&Module::parseDEL },
     { "delete",	&Module::parseDEL },
     { "erase",		&Module::parseDEL },
     { 0, 0 }
};


NOTE_FUNC(Module::parseDEL)
{
   String num = tokens.nextToken();
   
   if (num == "all") {
      services->getDatabase().dbDelete("notes",
				       "nto='" + origin.getNickname() + "'");
      origin.sendMessage(GETLANG(note_DEL_NOTES_ERASED), getName());
      services->log(origin,getName(),"Erased all notes");
      return;
   }
   
   int nbRes = services->getDatabase().dbSelect("id", "notes",
						"nto='" + 
						origin.getNickname() + "'");
   
   CResult *myRes = services->getDatabase().dbGetResultSet();
   
   int j = 0;
   
   for (int i = 0; i < nbRes; i++) {
      j++;
      
      if (j == num.toInt()) {
	 origin.sendMessage(GETLANG(note_DEL_NOTE_ERASED,num),
			    getName());
	 String ntext = myRes->getValue(i,0);
	 services->getDatabase().dbDelete("notes", "id='" + ntext + "'");
	 services->log(origin, "Note", "Deleted a single note");
	 return;
      }
   }
   delete myRes;
}


NOTE_FUNC(Module::parseREAD)
{
   String num = tokens.nextToken();
   
   if(num.toLower() == "all") {
      int nbRes = services->getDatabase().dbSelect("nfrom,nsent,note", "notes",
						   "nto='" + 
						   origin.getNickname() + "'");
      CResult *myRes = services->getDatabase().dbGetResultSet();
      int j = 0;
      
      for (int i = 0; i < nbRes; i++) {
	 j++;
	 String nfrom = myRes->getValue(i,0);
	 String nsent = myRes->getValue(i,1);
	 String ntext = myRes->getValue(i,2);
	 origin.sendMessage(GETLANG(note_READ_NOTE_ONE,String::convert(j),
				    nfrom,
				    nsent),getName());
	 origin.sendMessage(GETLANG(note_READ_NOTE_TWO,ntext), getName());
      }
      services->log(origin, "Note", "Read all notes");
      delete myRes;
      return;
   }

   int nbRes = services->getDatabase().dbSelect("nfrom,nsent,note", "notes", "nto='"+origin.getNickname()+"'");
   CResult *myRes = services->getDatabase().dbGetResultSet();
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
	 origin.sendMessage(togo,getName());
	 origin.sendMessage(GETLANG(note_READ_NOTE_TWO,ntext),getName());
	 services->log(origin, "Note", "Read a single note");
	 return;
      }
   }
   
   origin.sendMessage(GETLANG(note_READ_NOTE_NO_SUCH_NOTE), getName());
   return;

}


NOTE_FUNC(Module::parseLIST) {
   int nbRes = services->getDatabase().dbSelect("nfrom,nsent,note", "notes",
						"nto='" + 
						origin.getNickname() + "'");
   CResult *myRes = services->getDatabase().dbGetResultSet();
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
       origin.sendMessage(togo, getName());
   }
   delete myRes;
   if(j == 0) {
      origin.sendMessage(GETLANG(note_LIST_NOTES_NO_NOTES), getName());
      return;
   }
   origin.sendMessage(GETLANG(note_LIST_INSTRUCTIONS), getName());
   services->log(origin, "Note", "Listed their notes");
}


NOTE_FUNC(Module::parseSEND) {
   String nto = tokens.nextToken();
   String note = tokens.rest();
   
   if (nto == "" || note=="") {
      origin.sendMessage(GETLANG(note_SEND_USAGE),getName());
      return;
   }
   
   String it = (nto[0]);
   if(nto[0] == '#') {
      //Channel Note
      if(!services->getChannel().isChanRegistered(nto)) {
	 origin.sendMessage("That channel is not registered",getName());
	 return;
      }
      
      int chanid = services->getChannel().getChanID(nto);
      
      int nbRes = services->getDatabase().dbSelect("nickid", "chanaccess", 
						   "chanid='" + 
						   String::convert(chanid) + 
 						   "'");
      CResult *myRes = services->getDatabase().dbGetResultSet();
      
      int j = 0;
      for (int i = 0; i < nbRes; i++) {
	 String nnid = myRes->getValue(i,0);
	 int nid = nnid.toInt();
	 String nick = services->getNick(nid);
	 
	 if (nick.toLower() != origin.getNickname().toLower()) {
	    j++;
	    String txt = String("\002") + nto + "\002: " + note;
	    services->sendNote(origin.getNickname(), nick, txt);
	 }

      }
      
      String toao = String("Your note was successfully sent to \002") + 
	String::convert(j) + "\002 people on " + nto;
      origin.sendMessage(toao, getName());
      services->log(origin, "Note","Sent a channel note to " + nto);
      delete myRes;
      return;
   }
   
   if(!services->isNickRegistered(nto)) {
      origin.sendMessage("Error: Destination nickname is not registered",
			 getName());
      return;
   }
   
   services->sendNote(origin.getNickname(), nto, note);
   origin.sendMessage("Your note was successfully sent to \002"+ nto + "\002",
		      getName());
   services->log(origin, "Note", "Sent a private note to " + nto);
}


void Module::parseLine(StringTokens& line, User& origin, const bool safe)
{
   if (!origin.isIdentified(origin.getNickname())) {
      origin.sendMessage("Sorry - You must be identified to use this service",
			 getName());
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
   
   origin.sendMessage("Unrecognised Command", getName());
}


EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }


// Module information structure
const Module::moduleInfo_type Module::moduleInfo = {
   // Name
   "Note Service",
     
   // Version (major, minor)
   0, 0,
     
   // Event mask
   Exordium::Service::moduleInfo_type::Events::NONE
};


// Start the service
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;

   // Attempt to affirm our database table..
   if (!services->getDatabase().affirmTable(Tables::notesTable)) {
      services->logLine("Unable to affirm mod_note database table 'notes'",
			Log::Fatality);
      return false;
   }
   
   // Register ourself to the network
   services->registerService(getName(), getName(),
			     getConfigData().getHostname(),
			     getConfigData().getDescription());
   
   // We started okay :)
   return true;
}
