/*
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "game.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include <kineircd/str.h>
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

#include "exordi8.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

namespace Exordium
{

   struct Game::functionTableStruct const
     Game::functionTable[] =
     {
	  {"quote", &Game::parseQUOTE},
	  {".quote", &Game::parseQUOTE},
	  {"help", &Game::parseHELP},
	  {"start", &Game::parseSTART},
	  {0, 0}
     };

   void
     Game::parseLine (String const &line, String const &requestor, String const &chan)
       {
	  StringTokens st (line);
	  String origin = requestor;
	  String command = st.nextToken ().toLower ();
	  String ch = chan; // why is this copied?!
	  for (int i = 0; functionTable[i].command != 0; i++)
	    {
	       // Does this match?
	       if (command == functionTable[i].command)
		 {
		    // Run the command and leave
		    (this->*(functionTable[i].function))(origin, st, ch);
		    return;
		 }
	    }

	  // feh, check if this channel is one playing a game
	  if (channelGames.find(chan.IRCtoLower()) != channelGames.end()) {
	     // Too lazy to work around the mess above
	     StringTokens tokens(line.substr(1));
	     channelGames[chan]->parseLine(origin, tokens);
	  }
	  
	  return;
       }

   void
     Game::parseLine (String const &line, String const &requestor)
       {
	  StringTokens st (line);
	  String origin = requestor;
	  String command = st.nextToken ().toLower ();
	  String ch = "";
	  for (int i = 0; functionTable[i].command != 0; i++)
	    {
	       // Does this match?
	       if (command == functionTable[i].command)
		 {
		    // Run the command and leave
		    (this->*(functionTable[i].function))(origin, st, ch);
		    return;
		 }
	    }
	  services.serviceNotice ("Unrecognized Command", "Game", requestor);
       }

   void
     GAME_FUNC (Game::parseHELP)
       {
	  String word = tokens.nextToken();
	  String parm = tokens.nextToken();
	  services.doHelp(origin,"game",word,parm);
       }

   void
     GAME_FUNC (Game::parseQUOTE)
       {
	  return;
	  String channel = "";
	  if(chan!="")
	    {
	       channel = chan;
	    }
	  else
	    {
	       channel = tokens.nextToken();
	    }

	  if(channel=="")
	    {
	       services.serviceNotice("Usage: quote #channel","Game",origin);
	       return;
	    }
	  String query = "SELECT count(*) from fortunes";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  int j;
	  while ((row = res.fetch_row()))
	    {
	       String numb = ((std::string) row[0]).c_str();
	       j = services.random(numb.toInt());
	    }
	  res.free_result();
	  String thequote = services.getQuote(j);
	  StringTokens st (thequote);
	  bool more = false;
	  more = st.hasMoreTokens();
	  while(more==true)
	    {
	       String tq = st.nextToken('\n');
	       services.servicePrivmsg(services.getDatabase().makeSafe(tq),"Game",channel);
	       more = st.hasMoreTokens();
	    }

       }

   /* parseSTART - Parse a 'start' command, to trigger the start of game
    * Original 29/08/2002 - simonb
    */
   void
     GAME_FUNC (Game::parseSTART)
       {
	  String channel = tokens.nextToken().IRCtoLower();
	  String game = tokens.nextToken().toUpper();
	  
	  // Check for exordi8
	  if (game == "EXORDI8") {
	     channelGames[channel] = new Exordi8(*this, channel, origin);
	  } else {
	     // give them an error???!!
	     return;
	  }
	  
	  services.serviceJoin("game", channel);
	  services.serviceNotice("Hello " + channel + " (" + origin + " wanted to play " + game + ')', "Game", channel);
	  
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	std::cout << "I am called " << name << std::endl;
	services.registerService("game","game","ircdome.org", "+dz", "Network Games!");
	services.serviceJoin("game", "#Debug");
	return new Module("game", new Game(services, name));

     }

};

