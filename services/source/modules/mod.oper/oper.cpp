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
#include <exordium/service.h>
#include <exordium/services.h>

using AISutil::String;
using AISutil::StringTokens;
using namespace Exordium::OperModule;

const Module::functionTableStruct Module::functionTable[] =
{
     { "help",		&Module::parseHELP },
     { "jupe",		&Module::parseJUPE },
     { "join",		&Module::parseJOIN },
     { "part",		&Module::parsePART },
     { "commands",	&Module::parseCOMMANDS },
     { "zline",		&Module::parseZLINE },
     { "gline",		&Module::parseGLINE },
     { "qline",		&Module::parseQLINE },
     { "mdeop",		&Module::parseMDEOP },
     { "global",	&Module::parseGLOBAL },
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
		  origin.sendMessage("You do not have enough access for that command",getNickname());
		  String togo = origin.getNickname()+" tried to use \002"+command+"\002";
                  services->logLine(togo, Log::Warning);
                  return;
               }

	     // Run the command and leave
	     (this->*(functionTable[i].function))(origin, st);
	     return;
	  }
     }
   origin.sendMessage ("Unrecognized Command", getNickname());
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
	origin.sendMessage("Usage: global Your global here",getNickname());
	return;
     }

   /* And goo.... */
   int nbRes = services->getDatabase().dbSelect("servername","onlineservers");
   CResult *myRes = services->getDatabase().dbGetResultSet();
   for(int i=0;i<nbRes;i++)
     {
	services->queueAdd(":PeopleChat NOTICE $"+myRes->getValue(i,0)+" :\002[Global Announcement]\002 "+txt);
     }

   services->sendGOper("Oper",origin.getNickname()+" sent a \002global\002 message ("+txt+")");

}

OPER_FUNC(Module::parseMDEOP)
{
   String channel = tokens.nextToken();
   if(channel=="")
     {
	origin.sendMessage("Usage: mdeop #channel", getNickname());
	return;
     }

	/* Get the id.. */
   int cid = services->getChannel().getOnlineChanID(channel);
	/* Iterate over members.. */

   int nbRes = services->getDatabase().dbSelect("nickid","chanstatus","chanid="+String::convert(cid));
   CResult *myRes = services->getDatabase().dbGetResultSet();
   dChan *dptr = services->findChan(channel);
   if(dptr==0)
     {
	origin.sendMessage("Error: Cannot locate that channel",getNickname());
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
	origin.sendMessage("Usage: qline add/del", getNickname());
	return;
     }
   if(command=="del")
     {
	String mask = tokens.nextToken();
	if(mask=="")
	  {
	     origin.sendMessage("Usage: qline del mask (either channel/nick mask)",getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " removed a net wide \002qline\002 on \002"+mask+"\002";
	services->sendGOper("Oper",togo);
	services->queueAdd("UNSQLINE 0 "+mask);
	origin.sendMessage("qline removed",getNickname());
     }

   if(command=="add")
     {
	String mask = tokens.nextToken();
	String reason = tokens.rest();
	if(mask=="" | reason=="")
	  {
	     origin.sendMessage("Usage: qline add mask reason",getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " placed a net wide \002qline\002 on \002"+mask+"\002 (\002"+reason+"\002)";
	services->sendGOper("Oper",togo);
	services->queueAdd("SQLINE "+mask+" :"+reason);
	origin.sendMessage("qline added",getNickname());
	return;
     }

}
OPER_FUNC(Module::parseGLINE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("Usage: gline add/del", getNickname());
	return;
     }
   if(command=="del")
     {
	String mask = tokens.nextToken();
	if(mask=="")
	  {
	     origin.sendMessage("Usage: gline del mask",getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " removed a net wide \002gline\002 on \002"+mask+"\002";
	services->sendGOper("Oper",togo);
	services->queueAdd("GLINE 0 -"+mask);
	origin.sendMessage("Gline removed",getNickname());
     }

   if(command=="add")
     {
	String mask = tokens.nextToken();
	String expire = tokens.nextToken();
	String reason = tokens.rest();
	if(mask=="" | expire=="" | reason=="")
	  {
	     origin.sendMessage("Usage: gline add mask expire reason",getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " placed a net wide \002gline\002 on \002"+mask+"\002 expiring in \002"+expire+"\002 seconds (\002"+reason+"\002)";
	services->sendGOper("Oper",togo);
	services->queueAdd("GLINE 0 +"+mask+" "+expire+" :"+reason);
	origin.sendMessage("Gline added",getNickname());
	return;
     }

}

OPER_FUNC(Module::parseZLINE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("Usage: zline add/del", getNickname());
	return;
     }
   if(command=="del")
     {
	String ip = tokens.nextToken();
	if(ip=="")
	  {
	     origin.sendMessage("Usage: zline del IP",getNickname());
	     return;
	  }
	String togo = origin.getNickname() + " removed a net wide \002zline\002 on \002"+ip;
	services->sendGOper("Oper",togo);
	services->queueAdd("UNSZLINE " + ip);
	origin.sendMessage("Zline removed",getNickname());
     }

   if(command=="add")
     {
	String ip = tokens.nextToken();
	String reason = tokens.rest();

	if(ip=="" | reason=="")
	  {
	     origin.sendMessage("Usage: zline add IP Reason",getNickname());
	     return;
	  }

	String togo = origin.getNickname() + " placed a net wide \002zline\002 on \002" + ip + "\002 for \002"+reason+"\002";
	services->sendGOper("Oper",togo);
	services->queueAdd("SZLINE " + ip +" :"+reason);
	origin.sendMessage("Zline added",getNickname());
	return;
     }

}
OPER_FUNC(Module::parseJUPE)
{
   String command = tokens.nextToken();
   if(command=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 jupe add/list/del",getNickname());
	return;
     }
   if(command=="add")
     {
	String nickname = tokens.nextToken();
	String reason = tokens.rest();
	if(nickname=="" | reason=="")
	  {
	     origin.sendMessage("\002[\002Incorrect Usage\002]\002 Usage: add nickname reason for jupe",getNickname());
	     return;
	  }
     }
}

OPER_FUNC(Module::parseJOIN)
{
   String chan = tokens.nextToken().IRCtoLower();
   if(chan=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 join #channel",getNickname());
	return;
     }
   if(chan[0] != '#')
     {
        origin.sendMessage("Error: Channel names must begin with the '#' symbol",getNickname());
        return;
     }
   else
     {
	services->serviceJoin(getNickname(), chan);
	services->serverMode(chan, "+o", getNickname());
	services->sendGOper(getNickname(),"\002"+origin.getNickname()+"\002 made "+getNickname()+" join \002"+chan+"\002");
     }
}

OPER_FUNC(Module::parsePART)
{
   String chan = tokens.nextToken().IRCtoLower();
   if(chan=="")
     {
	origin.sendMessage("\002[\002Incorrect Usage\002]\002 part #channel",getNickname());
	return;
     }
   if(chan[0] != '#')
     {
        origin.sendMessage("Error: Channel names must begin with the '#' symbol",getNickname());
        return;
     }
   else
     {
	services->servicePart(getNickname(), chan);
	services->sendGOper(getNickname(),"\002"+origin.getNickname()+"\002 made "+getNickname()+" part \002"+chan+"\002");
     }
}

OPER_FUNC (Module::parseCOMMANDS)
{
   // Work out the line length, we subtract 20 to be safe :)
   String::size_type lineLength = 200;

   // Send the banner (this shouldn't be hard-coded)
   // sendMessage(origin, "Command list for " + getNickname() + ":");
   origin.sendMessage("Command list for " + getNickname() + ":",getNickname());
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
   origin.sendMessage("End of command list",getNickname());
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

   // Register ourself to the network
   services->registerService(getNickname(), getUsername(),
			     getHostname(), getDescription());

   // We started okay :)
   return true;
}
