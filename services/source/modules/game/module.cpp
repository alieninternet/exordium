/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/game.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include "kineircd/str.h"
#include "exordium/module.h"
#include <map>
#include "exordium/sql.h"

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;


namespace Exordium {

struct Game::functionTableStruct const
  Game::functionTable[] = {
  {"quote", parseQUOTE},
  {".quote", parseQUOTE},
  {"help", parseHELP},
  {0}
};

void
Game::parseLine (String const &line, String const &requestor, String const &chan)
{
StringTokens st (line);
String origin = requestor;
String command = st.nextToken ().toLower ();
String ch = chan;
for (int i = 0; functionTable[i].command != 0; i++)
    {
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st, ch);
          return;
        }
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
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st, ch);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "Game", requestor);
}

void
GAME_FUNC (Game::parseHELP)
{
	String word = tokens.nextToken();
	String parm = tokens.nextToken();
	Services::doHelp(origin,"game",word,parm);
}

void
GAME_FUNC (Game::parseQUOTE)
{
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
		Services::serviceNotice("Usage: quote #channel","Game",origin);
		return;
	}
	String query = "SELECT count(*) from fortunes";
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	int j;
	while ((row = res.fetch_row()))
	{
		String numb = ((std::string) row[0]).c_str();
		Services::Debug("I have this many fortunes "+numb);
		j = Services::random(numb.toInt());
		Services::Debug("I choose "+String::convert(j));
	}
	String thequote = Services::getQuote(j);
	cout << thequote << endl;
	StringTokens st (thequote);
	bool more = false;
	more = st.hasMoreTokens();
	while(more==true)
	{
	String tq = st.nextToken('\n');
	Services::servicePrivmsg(Sql::makeSafe(tq),"Game",channel);
	more = st.hasMoreTokens();
	}

}
extern "C" Module *service_init(void) 
{
   Services::registerService("Game","game","ircdome.org", "+dz", "Network Games!");
   Services::serviceJoin("Game", "#Debug");
   return new Module("game", new Game()
);
}



};

