/*
    NOTE.CPP   - Main entry point for IRCDome Network Services
    Version   - 0.1
    Date      - 8th May 2002
    Revisions - $Id$
  
    Copyright 2002 James Ian Wilkins <james@ircdome.org>

 */

#include "exordium/note.h"
#include "exordium/service.h"
#include "exordium/services.h"
#include "exordium/channel.h"
#include "exordium/nickname.h"
#include "kineircd/str.h"
#include "exordium/sql.h"
#include "exordium/module.h"
#include <map>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

namespace Exordium {

struct Note::functionTableStruct const
  Note::functionTable[] = {
  {"send", parseSEND},
  {"list", parseLIST},
  {"read", parseREAD},
  {"del", parseDEL},
  {"delete", parseDEL},
  {"erase", parseDEL},
  {0}
};
void
NOTE_FUNC (Note::parseDEL)
{
String num = tokens.nextToken();
if(num=="all")
	{
		String query = "DELETE from notes where nto='" + origin + "'";
		Sql::query(query);
		Services::serviceNotice("All notes erased.","Note",origin);
		Services::log(origin,"Note","Erased all notes");
		return;
	}
String query = "SELECT id from notes where nto='" + origin + "'";
MysqlRes res = Sql::query(query);
MysqlRow row;
int j = 0;
while ((row = res.fetch_row()))
{
	j++;
	if(j==num.toInt())
	{
		String togo = String("Note #\002")+num+"\002 deleted";
		Services::serviceNotice(togo,"Note",origin);
		String ntext = ((std::string) row[0]).c_str();
		String query = "DELETE from notes where id='" + ntext + "'";
		Sql::query(query);
		Services::log(origin,"Note","Deleted a single note");
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
		String query = "SELECT nfrom,nsent,note from notes where nto='" + origin + "'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		int j = 0;
		while ((row = res.fetch_row()))
		{
			j++;
			String nfrom = ((std::string) row[0]).c_str();
			String nsent = ((std::string) row[1]).c_str();
			String ntext = ((std::string) row[2]).c_str();
			String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
			Services::serviceNotice(togo,"Note",origin);
			String tofo = ntext;
			Services::serviceNotice(tofo,"Note",origin);
		}
		Services::log(origin,"Note","Read all notes");
		return;
	}
	String query = "SELECT nfrom,nsent,note from notes where nto='" + origin + "'";
	MysqlRes res = Sql::query(query);
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
		Services::serviceNotice(togo,"Note",origin);
		String tofo = ntext;
		Services::serviceNotice(tofo,"Note",origin);
		Services::log(origin,"Note","Read a single note");
		return;
		}
	}	
	Services::serviceNotice("No such note!","Note",origin);
	return;
}
void
NOTE_FUNC (Note::parseLIST)
{
	String query = "select nfrom,nsent,note from notes where nto='" + origin + "'";
	MysqlRes res = Sql::query(query);
	MysqlRow row;
	int j = 0;
	while ((row = res.fetch_row()))
	{
		j++;
		String nfrom = ((std::string) row[0]).c_str();
		String nsent = ((std::string) row[1]).c_str();
		String ntext = ((std::string) row[2]).c_str();
		String togo = String("Note #\002")+String::convert(j)+"\002 From: \002"+nfrom+"\002 Sent: \002"+nsent+"\002";
		Services::serviceNotice(togo,"Note",origin);
	}
	String tofo = String("To read a note, type /msg Note read Number");
	Services::serviceNotice(tofo,"Note",origin);
	Services::log(origin, "Note", "Listed their notes");
}
void
NOTE_FUNC (Note::parseSEND)
{
	String nto = tokens.nextToken();
	String note = tokens.rest();
	if(nto=="")
	{
		String togo = String("Usage is: /msg note send Nickname/#Channel Your Message Here");
		Services::serviceNotice(String(togo),"Serv",origin);
		return;
	}
	if(note=="")
	{
		String togo = String("Usage is: /msg note send Nickname Your Message Here");
		Services::serviceNotice(String(togo),"Serv",origin);
		return;
	}
	String it = (nto[0]);
	if(nto[0]=='#')
	{
		//Channel Note
		if(!Channel::isChanRegistered(nto))
		{
		String tofo = String("That channel is not registered");
		Services::serviceNotice(String(tofo),"Note",origin);
		return;
		}
		int chanid = Channel::getChanID(nto);
		String query = "SELECT nickid from chanaccess where chanid='" + String::convert(chanid) + "'";
		MysqlRes res = Sql::query(query);
		MysqlRow row;
		int j = 0;
		while ((row = res.fetch_row()))
		{
			String nnid = ((std::string) row[0]).c_str();
			int nid = nnid.toInt();
			String nick = Nickname::getNick(nid);
			if(nick.toLower()!=origin.toLower())
			{
				j++;
				String txt = String("\002")+nto+"\002: "+note;
				Services::sendNote(origin,nick,txt);
			}
		}		
		String toao = String("Your note was successfully sent to \002")+String::convert(j)+"\002 people on "+nto;
		Services::serviceNotice(String(toao),"Note",origin);
		Services::log(origin,"Note","Sent a channel note to "+nto);
		return;
	}
	
	if(!Nickname::isNickRegistered(nto))
	{
		String togo = String("Error: Destination nickname is not registered");
		Services::serviceNotice(togo,"Serv",origin);
		return;
	}
	Services::sendNote(origin,nto,note);
	Services::serviceNotice("Your note was successfully sent to \002"+nto+"\002","Note",origin);
	Services::log(origin,"Note","Sent a private note to "+nto);
}

void
Note::parseLine (String const &line, String const &requestor, String const &ch)
{
return;
}

void 
Note::parseLine (String const &line, String const &requestor)
{   
  StringTokens st (line);
  String origin = requestor;
  if(!Nickname::isIdentified(requestor,requestor))
	{
		Services::serviceNotice("Sorry - You must be identified to use this service","Note",origin);
		return;
	}
  String command = st.nextToken ().toLower ();
  for (int i = 0; functionTable[i].command != 0; i++)
    {
	Services::Debug(line);
	Services::Debug(requestor);
      // Does this match?   
      if (command == functionTable[i].command)
        {
          // Run the command and leave
          functionTable[i].function (origin, st);
          return;
        }
    }
  Services::serviceNotice ("Unrecognized Command", "Note", requestor);
}


extern "C" Module *service_init(void) {
   return new Module("note", new Note());
}

}

