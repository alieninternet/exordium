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

#include "oper.h"
#include "language.h"

#include <exordium/service.h>
#include <exordium/services.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::OperModule;

const Module::functionTableStruct Module::functionTable[] =
{
     { "help",		&Module::parseHELP },
     { "join",		&Module::parseJOIN },
     { "part",		&Module::parsePART },
     { "commands",	&Module::parseCOMMANDS },
     { "zline",		&Module::parseZLINE },
     { "qline",		&Module::parseQLINE },
     { "mdeop",		&Module::parseMDEOP },
     { "global",	&Module::parseGLOBAL },
     { "kill",		&Module::parseKILL },
     { 0, 0 }
};

void Module::parseLine(StringTokens& line, User& origin, const bool safe)
{
   StringTokens& st = line;
   String command = st.nextToken ().toLower ();
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Does this match?
	if (command == functionTable[i].command)
	  {
             int required = services->getRequiredAccess(getNickname(),command.toLower());
             int access = origin.getAccess(getNickname());
             if(required>access)
               {
		  origin.sendMessage(GETLANG(oper_NOT_ENOUGH_ACCESS),getNickname());
                  services->logLine(origin.getNickname()+" tried to use \002"+command+"\002", Log::Warning);
                  return;
               }

	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage (GETLANG(ERROR_UNKNOWN_COMMAND), getNickname());
}

OPER_FUNC(Module::parseHELP)
{
   String word = tokens.nextToken();
   String parm = tokens.nextToken();
   services->doHelp(origin,getNickname(),word,parm);
}

OPER_FUNC(Module::parseGLOBAL)
{
   String txt = tokens.rest();
   if(txt=="")
     {
	origin.sendMessage(GETLANG(oper_GLOBAL_USAGE),getNickname());
	return;
     }
   int nbRes = services->getDatabase().dbSelect("servername","onlineservers");
   CResult *myRes = services->getDatabase().dbGetResultSet();
   for(int i=0;i<nbRes;i++)
     {
	services->queueAdd(":PeopleChat NOTICE $"+myRes->getValue(i,0)+" :\002[Global Announcement]\002 "+txt);
     }

   services->sendGOper(getNickname(),origin.getNickname()+" sent a \002global\002 message ("+txt+")");

}

OPER_FUNC(Module::parseMDEOP)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage(GETLANG(oper_MDEOP_USAGE), getNickname());
	return;
     }
   int cid = services->getChannel().getOnlineChanID(channel);
   int nbRes = services->getDatabase().dbSelect("nickid","chanstatus","chanid="+String::convert(cid));
   CResult *myRes = services->getDatabase().dbGetResultSet();
   dChan *dptr = services->findChan(channel);
   if(dptr==0)
     {
	origin.sendMessage(GETLANG(oper_MDEOP_NOT_FOUND),getNickname());
	return;
     }

   for (int i=0; i<nbRes; i++)
     {
	String inick = services->getOnlineNick(myRes->getValue(i,0).toInt());
	std::cout << inick << std::endl;
	services->mode(getNickname(),channel,"-ov",inick);
	User *ptr = services->findUser(inick);
	dptr->delUser(*ptr);
	dptr->addUser(*ptr,0);

     }

   services->sendGOper(getNickname(),origin.getNickname()+" performed a \002massdeop\002 in "+channel);
   delete dptr;
   delete myRes;

}
OPER_FUNC(Module::parseQLINE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage(GETLANG(oper_QLINE_OPTS), getNickname());
	return;
     }
   if(command=="del")
     {
	String mask = tokens.nextToken();
	if(mask=="")
	  {
	     origin.sendMessage(GETLANG(oper_QLINE_DEL_USAGE),getNickname());
	     return;
	  }
	services->sendGOper(getNickname(),origin.getNickname() + " removed a net wide \002qline\002 on \002"+mask+"\002");
	services->queueAdd("UNSQLINE 0 "+mask);
	origin.sendMessage(GETLANG(oper_QLINE_DEL_SUCCESS,mask),getNickname());
     }

   if(command=="add")
     {
	String mask = tokens.nextToken();
	String reason = tokens.rest();
	if(mask=="" | reason=="")
	  {
	     origin.sendMessage(GETLANG(oper_QLINE_ADD_USAGE),getNickname());
	     return;
	  }
	services->sendGOper(getNickname(),origin.getNickname() + " placed a net wide \002qline\002 on \002"+mask+"\002 (\002"+reason+"\002)");
	services->queueAdd("SQLINE "+mask+" :"+reason);
	origin.sendMessage(GETLANG(oper_QLINE_ADD_SUCCESS,mask,reason),getNickname());
	return;
     }

}

OPER_FUNC(Module::parseZLINE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage(GETLANG(oper_ZLINE_OPTS), getNickname());
	return;
     }
   if(command=="del")
     {
	String ip = tokens.nextToken();
	if(ip=="")
	  {
	     origin.sendMessage(GETLANG(oper_ZLINE_DEL_USAGE),getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " removed a net wide \002zline\002 on \002"+ip;
	services->sendGOper(getNickname(),togo);
	services->queueAdd("UNSZLINE " + ip);
	origin.sendMessage(GETLANG(oper_ZLINE_DEL_SUCCESS,ip),getNickname());
     }

   if(command=="add")
     {
	String ip = tokens.nextToken();
	String reason = tokens.rest();

	if(ip=="" | reason=="")
	  {
	     origin.sendMessage(GETLANG(oper_ZLINE_ADD_USAGE),getNickname());
	     return;
	  }

	String togo = origin.getNickname() + " placed a net wide \002zline\002 on \002" + ip + "\002 for \002"+reason+"\002";
	services->sendGOper(getNickname(),origin.getNickname() + " placed a net wide \002zline\002 on \002" + ip + "\002 for \002"+reason+"\002");
	services->queueAdd("SZLINE " + ip +" :"+reason);
	origin.sendMessage(GETLANG(oper_ZLINE_ADD_SUCCESS,ip,reason),getNickname());
	return;
     }

}


OPER_FUNC(Module::parseJOIN)
{
   String chan = tokens.nextToken().IRCtoLower();
   if(chan=="")
     {
	origin.sendMessage(GETLANG(oper_JOIN_USAGE),getNickname());
	return;
     }
   if(chan[0] != '#')
     {
        origin.sendMessage(GETLANG(oper_JOIN_NO_HASH),getNickname());
        return;
     }
   else
     {
	services->serviceJoin(getNickname(), chan);
	services->serverMode(chan, "+o", getNickname());
	services->sendGOper(getNickname(),"\002"+origin.getNickname()+"\002 made "+getNickname()+" join \002"+chan+"\002");
	origin.sendMessage(GETLANG(oper_JOIN_SUCCESS,chan),getNickname());
     }
}

OPER_FUNC(Module::parsePART)
{
   String chan = tokens.nextToken().IRCtoLower();
   if(chan=="")
     {
	origin.sendMessage(GETLANG(oper_PART_USAGE),getNickname());
	return;
     }
   if(chan[0] != '#')
     {
        origin.sendMessage(GETLANG(oper_JOIN_NO_HASH),getNickname());
        return;
     }
   else
     {
	services->servicePart(getNickname(), chan);
	services->sendGOper(getNickname(),"\002"+origin.getNickname()+"\002 made "+getNickname()+" part \002"+chan+"\002");
	origin.sendMessage(GETLANG(oper_PART_SUCCESS,chan),getNickname());
     }
}

OPER_FUNC (Module::parseKILL)
{
   String tokill = tokens.nextToken();
   String reason = tokens.rest();
   if(tokill == "" | reason == "")
     {
	origin.sendMessage(GETLANG(oper_KILL_USAGE),getNickname());
	return;
     }
   User *ptr = services->findUser(tokill);
   if(ptr==0)
     {
	origin.sendMessage(GETLANG(ERROR_COULDNT_FIND_USER),getNickname());
	return;
     }
   else
     {
	services->killnick(tokill,getNickname(),reason);
	origin.sendMessage(GETLANG(oper_KILL_SUCCESS,tokill),getNickname());
	services->sendGOper(getNickname(),"\002"+origin.getNickname()+"\002 requested a kill on \002"+tokill+"\002");
     }
}

OPER_FUNC (Module::parseCOMMANDS)
{
   // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength = 200;

   // Send the banner (this shouldn't be hard-coded)
   // sendMessage(origin, "Command list for " + getNickname() + ":");
   origin.sendMessage(GETLANG(COMMAND_LIST_START,getNickname()),getNickname());
   // Start formulating the data..
   std::ostringstream list(" -=>");
   for (int i = 0; functionTable[i].command != 0; i++)
     {
	// Add the command to the list
	list << " " << functionTable[i].command;

	// How are we for size?
	if (list.str().length() >= lineLength)
	  {
	     // Dump it and reset the string stream thingy
	     origin.sendMessage(list.str(),getNickname());
	     list.str() = " -=>";
	  }
     }

   // Anything left to send still?
   if (list.str().length() > 4)
     {
	origin.sendMessage(list.str(),getNickname());
     }

   // Send the footer (this shouldn't be hard-coded)
   origin.sendMessage(GETLANG(COMMAND_LIST_END),getNickname());
}

EXORDIUM_SERVICE_INIT_FUNCTION
{ return new Module(); }

// Module information structure
const Module::moduleInfo_type Module::moduleInfo =
{
   "Operator Service",
     0, 0,
     Exordium::Service::moduleInfo_type::Events::NONE
};

// Start the service
bool Module::start(Exordium::Services& s)
{
   // Set the services field appropriately
   services = &s;

   // Register and process our language tag name -> tag ID map
   Kine::langs().registerMap(Language::tagMap);

   // Register ourself to the network
   services->registerService(getNickname(), getUsername(),
			     getHostname(), getDescription());

   // We started okay :)
   return true;
}
