/*
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "include/serv.h"
#include "exordium/services.h"
#include "exordium/nickname.h"
#include "exordium/channel.h"
#include <kineircd/str.h>
#include "exordium/sql.h"
#include "exordium/module.h"
#include <sys/time.h>

using Kine::String;
using Kine::StringTokens;
using namespace Exordium;

namespace Exordium
{

   struct Serv::functionTableStruct const
     Serv::functionTable[] =
     {
	  {"clist", &Serv::parseCLIST},
	  {"delnick", &Serv::parseDELNICK},
	  {"elist", &Serv::parseELIST},
	  {"nlist", &Serv::parseNLIST},
	  {"helpon", &Serv::parseHELPON},
	  {"help", &Serv::parseHELP},
	  {"user", &Serv::parseUSER},
	  {"raw", &Serv::parseRAW},
	  {"chan", &Serv::parseCHAN},
	  {"die", &Serv::parseDIE},
	  {"news", &Serv::parseNEWS},
	  {"synch", &Serv::parseSYNCH},
	  {"setpass", &Serv::parseSETPASS},
	  {0, 0}
     };
   void
     Serv::parseLine (String const &line, String const &requestor, String const &ch)
       {
	  return;
       }

   void
     Serv::parseLine (String const &line, String const &requestor)
       {
	  StringTokens st (line);
	  String origin = requestor;
	  String command = st.nextToken ().toLower ();
	  for (int i = 0; functionTable[i].command != 0; i++)
	    {
	       // Does this match?
	       if (command == functionTable[i].command)
		 {
		    // Check access :-)
		    int required = services.getNickname().getRequiredAccess("Serv",command.toLower());
		    int access = services.getNickname().getAccess("Serv",origin);
		    if(required>access)
		      {
			 services.serviceNotice("You do not have enough access for that command","Serv",origin);
			 String togo = origin+" tried to use \002"+command+"\002";
			 services.helpme(togo,"Serv");
			 return;
		      }

		    // Run the command and leave
		    (this->*(functionTable[i].function))(origin, st);
		    return;
		 }
	    }
	  services.serviceNotice ("Unrecognized Command", "Serv", requestor);
       }
   void
     SERV_FUNC (Serv::parseDIE)
       {
	  String reason = tokens.rest();
	  if(reason=="")
	    {
	       services.serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: die reason for shutdown here","Serv",origin);
	       return;
	    }

	  String togo = "\002"+origin+"\002 - "+reason;
	  exit(0);
	  services.shutdown(togo);
       }
   void
     SERV_FUNC (Serv::parseSYNCH)
       {
	  String tosend = "\002[\002Database Save\002]\002 Started by "+origin;
	  services.helpme(tosend,"Serv");
	  struct timeval start;
	  gettimeofday(&start, NULL);
	  String query = "COMMIT";
	  services.getDatabase().query(query);
	  struct timeval finish;
	  gettimeofday(&finish,NULL);
	  long long time = ((((long long)finish.tv_sec * 1000000) + finish.tv_usec)
			    - (((long long)start.tv_sec * 1000000) + start.tv_usec));
	  String togo = "Database synch finished - time taken "+String::convert(time)+" microseconds";
	  services.helpme(togo,"Serv");

       }
   void
     SERV_FUNC (Serv::parseSETPASS)
       {
	  String who = tokens.nextToken();
	  String newpass = tokens.nextToken();
	  if(who=="" | newpass=="")
	    {
	       String togo = "Usage: setpass nickname newpassword";
	       services.serviceNotice(togo,"Serv",origin);
	       return;
	    }
	  if(!services.getNickname().isNickRegistered(who))
	    {

	       services.serviceNotice("Error: Nickname is not registered","Serv",origin);
	       return;
	    }
	  String epass =  String::convert(services.getNickname().generatePassword(who,newpass));
	  String query = "UPDATE nicks set password='" + epass + "' where nickname='"+who+"'";
	  services.getDatabase().query(query);
	  String togo = "\002"+origin+"\002 changed password for nickname "+who+" to [HIDDEN]";
	  services.helpme(togo,"Serv");
       }

   void
     SERV_FUNC (Serv::parseRAW)
       {
	  std::string c = tokens.rest();
	  services.queueAdd(c);
	  String togo = origin+" did \002RAW\002 - "+c;
	  services.helpme(String(togo),"Serv");
       }
   void
     SERV_FUNC (Serv::parseNEWS)
       {
	  String command = tokens.nextToken();
	  if(command=="")
	    {
	       services.serviceNotice("Usage: news add/del/list","Serv",origin);
	       return;
	    }
	  if(command=="list")
	    {
	       String query = "SELECT * from news";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       while ((row = res.fetch_row()))
		 {
		    String id = ((std::string) row[0]).c_str();
		    String level = ((std::string) row[1]).c_str();
		    String expires = ((std::string) row[2]).c_str();
		    String txt = ((std::string) row[3]).c_str();
		    String togo = "ID \002[\002"+id+"\002]\002 Level \002[\002"+level+"\002]\002 Expires \002[\002"+expires+"\002]\002 Text \002[\002"+txt+"\002]\002";
		    services.serviceNotice(togo,"Serv",origin);
		 }

	    }
	  if(command=="del")
	    {
	       String id = tokens.nextToken();
	       if(id.empty())
		 {
		    services.serviceNotice("\002[\002Fatal Error\002]\002 Usage: del ID","Serv",origin);
		    return;
		 }
	       String query = "DELETE FROM news where id="+id;
	       services.getDatabase().query(query);
	       services.serviceNotice("News has been deleted","Serv",origin);
	       return;
	    }
	  if(command=="add")
	    {
	       String type = tokens.nextToken();
	       String expires = tokens.nextToken();
	       String text = tokens.rest();
	       if(type=="" | expires=="" | text=="")
		 {
		    services.serviceNotice("\002[\002Incorrect Usage\002]\002 Usage: news add type expires news text here","Serv",origin);
		    return;
		 }
	       int nexpires = expires.toInt();
	       nexpires = services.currentTime + (nexpires * 3600);
	       if(services.currentTime>nexpires)
		 {
		    services.serviceNotice("\002[\002Fatal Error\002]\002 Your expiry time cannot be in the past","Serv",origin);
		    return;
		 }
	       String query = "INSERT into news values ('','"+type+"','"+String::convert(nexpires)+"','"+text+"');";
	       services.getDatabase().query(query);
	       services.serviceNotice("New news item added successfully","Serv",origin);
	    }

       }
   void
     SERV_FUNC (Serv::parseCHAN)
       {
	  String command = tokens.nextToken();
	  String channel = tokens.nextToken();
	  if(command=="")
	    {
	       services.serviceNotice("Usage: chan add/del/mod","Serv",origin);
	       return;
	    }
	  if(command=="mod")
	    {
	       String newowner = tokens.nextToken();
	       if(channel=="" || newowner=="")
		 {
		    services.serviceNotice("Usage: chan mod #channel NewOwner","Serv",origin);
		    return;
		 }
	       if(!services.getNickname().isNickRegistered(newowner))
		 {
		    services.serviceNotice("Error: New owner's nickname is not registered","Serv",origin);
		    return;
		 }
	       String newtopic = "This channel is now owned by "+newowner;
	       services.getChannel().setTopic(channel,newtopic);
	       services.getChannel().updateTopic(channel,newtopic);
	       int chanid = services.getChannel().getChanID(channel);
	       String oldowner = services.getChannel().getChanOwner(chanid);
	       String togo = origin+" changed \002ownership\002 of "+channel+" "+oldowner+"->"+newowner;
	       services.helpme(String(togo),"Serv");
	       services.getChannel().chanDelAccess(channel,oldowner);
	       services.getChannel().chanAddAccess(channel,newowner,"500");
	       String query = "UPDATE chans set owner='" + newowner + "' where name='"+channel+"'";
	       services.getDatabase().query(query);
	       services.log(origin,"Serv",String("Changed ownership of ")+channel+" to "+newowner+" ("+oldowner+")");
	       return;
	    }
	  if(command=="del")
	    {
	       String reason = tokens.rest();
	       if(channel=="" || reason=="")
		 {
		    services.serviceNotice("Usage: chan del #channel reason","Serv",origin);
		    return;
		 }
	       String togo = origin + "\002 de-registered\002 "+channel+" for \002"+reason+"\002";
	       services.helpme(String(togo),"serv");
	       services.getChannel().deregisterChannel(channel,reason);
	       services.log(origin,"Serv",String("Deregistered ")+channel+" for "+reason);
	       return;
	    }
	  if(command=="add")
	    {
	       String thenick = tokens.nextToken();
	       if(channel=="" || thenick=="")
		 {
		    services.serviceNotice("Usage: chan add #channel owner","Serv",origin);
		    return;
		 }
	       if(services.getChannel().isChanRegistered(channel))
		 {
		    services.serviceNotice("That channel is already registered - Try mod","Serv",origin);
		    return;
		 }
	       if(!services.getNickname().isNickRegistered(thenick))
		 {
		    services.serviceNotice("Target nickname does not exist","Serv",origin);
		    return;
		 }
	       String togo = origin + "\002 registered\002 " + channel + " to "+thenick;
	       services.helpme(String(togo),"Serv");
	       services.getChannel().registerChannel(channel,thenick);
	       services.log(origin,"Serv",String("Registered ")+channel+" to "+thenick);
	       return;
	    }
       }
   void
     SERV_FUNC (Serv::parseHELP)
       {
	  String word = tokens.nextToken();
	  String parm = tokens.nextToken();
	  services.doHelp(origin,"Serv",word,parm);
	  String tolog = "Did HELP on word " + word + " parm " + parm;
	  services.log(origin,"Serv",String(tolog));
       }

   void
     SERV_FUNC (Serv::parseUSER)
       {
	  String command = tokens.nextToken();
	  String toadd = tokens.nextToken();
	  String level = tokens.nextToken();
	  int access = services.getNickname().getAccess("Serv",origin);
	  if(command=="")
	    {
	       services.serviceNotice("Commands are : add/del/mod/list","Serv",origin);
	       return;
	    }
	  if(command=="mod")
	    {
	       if(toadd=="" || level=="")
		 {
		    services.serviceNotice("Usage: user mod nickname level","Serv",origin);
		    return;
		 }
	       if(toadd==origin)
		 {
		    services.serviceNotice("Silly billy... you can't mod yourself!","Serv",origin);
		    return;
		 }
	       int ilevel = level.toInt();
	       if(ilevel>access || ilevel==access)
		 {
		    services.serviceNotice("Error: You cannot set someones access higher than, or equal to your own","Serv",origin);
		    return;
		 }
	       int taccess = services.getNickname().getAccess("Serv",toadd);
	       if(taccess>access)
		 {
		    services.serviceNotice("Error: That person has higher access than you","Serv",origin);
		    String togo = origin+" tried to modify access for a higher user than themselves ("+toadd+")";
		    services.helpme(String(togo),"Serv");
		    return;
		 }
	       if(taccess==access)
		 {
		    services.serviceNotice("Error: That person has the same access as you","Serv",origin);
		    return;
		 }
	       String togo = origin + " modified access for \002"+toadd+"\002 "+String::convert(taccess)+"->"+level;
	       services.helpme(togo,"Serv");
	       String query = "UPDATE access set access='" + level + "' WHERE nickname='" + toadd + "'";
	       services.getDatabase().query(query);
	       services.log(origin,"Serv",String("Modified access for ")+toadd+" from "+String::convert(taccess)+"->"+level);
	       return;
	    }
	  if(command=="list")
	    {
	       String query = "SELECT * from access where service='serv' order by id";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       while (( row  = res.fetch_row()))
		 {
		    String nickname = ((std::string) row[1]).c_str();
		    String access = ((std::string) row[3]).c_str();
		    String togo = "\002"+nickname+"\002 has level \002"+access;
		    services.serviceNotice(String(togo),"Serv",origin);
		 }
	       res.free_result();
	       return;
	    }
	  if(command=="del")
	    {
	       if(toadd=="")
		 {
		    services.serviceNotice("Usage is user del nickname","Serv",origin);
		    return;
		 }
	       if((services.getNickname().getAccess("Serv",toadd))==0)
		 {
		    services.serviceNotice("That person does not have access to serv","Serv",origin);
		    return;
		 }
	       int faccess = services.getNickname().getAccess("Serv",toadd);
	       if(faccess>access)
		 {
		    services.serviceNotice("You do not have enough access to perform that operation on a staff nickname","Serv",origin);
		    String togo = origin + " tried to use \002userdel\002 on a \002staff\002 nickname";
		    services.helpme(String(togo),"Serv");
		    return;
		 }
	       String query = "DELETE from access where service='serv' and nickname='" + toadd + "'";
	       services.getDatabase().query(query);
	       services.serviceNotice("Command complete","Serv",origin);
	       String togo = origin + " deleted \002 " + toadd + "\002 from Serv";
	       services.helpme(String(togo),"Serv");
	       services.log(origin,"Serv","Deleted "+toadd+" from Serv");
	       return;
	    }
	  if(command=="add")
	    {
	       if(toadd=="" || level=="")
		 {
		    services.serviceNotice("Usage: user add nickname level","Serv",origin);
		    return;
		 }
	       if(origin.toLower()==toadd.toLower())
		 {
		    services.serviceNotice("You can't adduser yourself, silly.","Serv",origin);
		    return;
		 }
	       if(!services.getNickname().isNickRegistered(toadd))
		 {
		    services.serviceNotice("That nickname is not registered","Serv",origin);
		    return;
		 }
	       if((services.getNickname().getAccess("Serv",toadd))>0)
		 {
		    services.serviceNotice("That person already has access, try mod","Serv",origin);
		    return;
		 }
	       if(level.toInt()<1)
		 {
		    services.serviceNotice("Error: You cannot add someone with less than 1 access","Serv",origin);
		    return;
		 }
	       if(level.toInt()>499)
		 {
		    services.serviceNotice("Error: You cannot add someone with higher than 499 access","Serv",origin);
		    return;
		 }
	       String query = "INSERT into access values ('','" + toadd + "','serv','" + level + "')";
	       services.getDatabase().query(query);
	       services.serviceNotice("Command completed","Serv",origin);
	       String togo = origin+" added \002"+toadd+"\002 to Serv with level \002"+level;
	       services.helpme(String(togo),"Serv");
	       String tolog = "Added "+toadd+" to Serv with level "+toadd;
	       services.log(origin,"Serv",String(tolog));
	       return;
	    }
	  services.serviceNotice("Uncognised sub-command","Serv",origin);
	  return;
       }
   void
     SERV_FUNC (Serv::parseHELPON)
       {
	  int access = services.getNickname().getAccess("Serv",origin);
	  if(access>50)
	    {
	       services.serviceNotice("You are now an IRCDome services assistant","Serv",origin);
	       String tosend = ":services.ircdome.org HELPER "+origin+" "+String::convert(access);
	       services.queueAdd(String(tosend));
	       services.log(origin,"Serv","Become a services helper at level "+String::convert(access));
	       return;
	    }
	  services.log(origin,"Serv","Failed to become a helper (not enough access)");
	  String tosend = origin+" failed to become a helper - Not enough access";
	  services.helpme(tosend,"Serv");
       }
   void
     SERV_FUNC (Serv::parseNLIST)
       {
	  String tomatch = tokens.nextToken();
	  String dest = tokens.nextToken();
	  if(tomatch=="")
	    {
	       services.serviceNotice("Usage: nlist <match> <optinal destination>","Serv",origin);
	       return;
	    }
	  String query = "SELECT nickname,lasthost,email from nicks WHERE lasthost like '"+tomatch+"'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  int f=0;
	  while ((row = res.fetch_row()))
	    {
	       f++;
	       String tnick = ((std::string) row[0]).c_str();
	       String thost = ((std::string) row[1]).c_str();
	       String temail = ((std::string) row[2]).c_str();
	       String tosend = String("\002")+tnick+"\002 with last address \002"+thost+"\002 "+temail+"\002";
	       if(dest=="")
		 {
		    services.serviceNotice(String(tosend),"Serv",origin);
		 }
	       else
		 {
		    services.serviceNotice(String(tosend),"Serv",dest);
		 }
	    }
	  services.log(origin,"Serv","Did a nlist on "+tomatch+" "+String::convert(f)+" matches found");
	  String togo = origin+" did a \002nlist\002 on "+tomatch+" "+String::convert(f)+" matches found";
	  services.helpme(togo,"Serv");
	  res.free_result();
       }
   void
     SERV_FUNC (Serv::parseELIST)
       {
	  String tomatch = tokens.nextToken();
	  String dest = tokens.nextToken();
	  if(tomatch=="")
	    {
	       services.serviceNotice("Usage: elist <match> <optional destination>","Serv",origin);
	       return;
	    }
	  if(dest=="")
	    {
	       //Return to origin.
	       String query = "SELECT nickname,lasthost,email from nicks where email like '" + tomatch + "'";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       while ((row = res.fetch_row()))
		 {
		    String nickname = ((std::string) row[0]).c_str();
		    String lasthost = ((std::string) row[1]).c_str();
		    String email = ((std::string) row[2]).c_str();
		    String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
		    services.serviceNotice(tosend,"Serv",origin);
		 }
	       services.log(origin,"Serv","Did an elist on "+tomatch);
	       String togo = origin + " did an \002elist\002 on "+tomatch;
	       services.helpme(togo,"Serv");
	       res.free_result();
	       return;
	    }
	  //Else send to given client
	  String query = "SELECT nickname,lasthost,email from nicks where email like '" + tomatch + "'";
	  MysqlRes res = services.getDatabase().query(query);
	  MysqlRow row;
	  while ((row = res.fetch_row()))
	    {
	       String nickname = ((std::string) row[0]).c_str();
	       String lasthost = ((std::string) row[1]).c_str();
	       String email = ((std::string) row[2]).c_str();
	       String tosend = "\002"+nickname+"\002 with last address \002"+lasthost+"\002 and email \002"+email+"\002";
	       services.serviceNotice(tosend,"Serv",dest);
	    }
	  services.log(origin,"Serv","Did an elist on "+tomatch+" and sent it to "+dest);
	  String togo = origin + " did an \002elist\002 on "+tomatch+" and sent the results to "+dest;
	  services.helpme(togo,"Serv");
	  res.free_result();

       }
   void
     SERV_FUNC (Serv::parseDELNICK)
       {
	  String who  = tokens.nextToken();
	  String reason = tokens.rest();
	  if(who=="" || reason=="")
	    {
	       services.serviceNotice("Usage: delnick nickname reason","Serv",origin);
	       return;
	    }
	  if(!services.getNickname().isNickRegistered(who))
	    {
	       services.serviceNotice("Error: Nickname is not registered","Serv",origin);
	       return;
	    }

	  String togo = origin+" did \002delnick\002 on "+who+" for \002"+reason;
	  services.helpme(togo,"Serv");
	  String query = "DELETE from nicks where nickname='" + who + "'";
	  services.getDatabase().query(query);
	  services.log(origin,"Serv","Deleted nickname "+who+" : "+reason);
       }

   void
     SERV_FUNC (Serv::parseCLIST)
       {
	  String who = tokens.nextToken();
	  String send = tokens.nextToken();
	  if(who=="")
	    {
	       services.serviceNotice("Usage: clist nickname <optional nick to send to>","Serv",origin);
	       return;
	    }

	  if(!services.getNickname().isNickRegistered(who))
	    {
	       services.serviceNotice("That nickname is not registered","Serv",origin);
	       return;
	    }
	  if(send=="")
	    {
	       int totalc = services.getChannel().maxChannels();
	       int userc = services.getChannel().maxChannelsUser(who);
	       int totala = services.getChannel().maxChannelsAccess();
	       String togo = origin + " did a \002clist\002 on "+who+", "+String::convert(userc)+" matches found from "+String::convert(totalc)+" channels and "+String::convert(totala)+" access entries";
	       services.helpme(togo,"Serv");
	       int theid = services.getNickname().getRegisteredNickID(who);
	       String query = "SELECT chanid,access from chanaccess where nickid='" + String::convert(theid) + "'";
	       MysqlRes res = services.getDatabase().query(query);
	       MysqlRow row;
	       while ((row = res.fetch_row()))
		 {
		    String cname = ((std::string) row[0]).c_str();
		    String caxs = ((std::string) row[1]).c_str();
		    String ccname = services.getChannel().getChanName(cname.toInt());
		    String tosend = ccname+" with "+caxs;
		    services.serviceNotice(tosend,"Serv",origin);
		 }
	       services.log(origin,"Serv","Did a clist on "+who);
	       res.free_result();
	       return;
	    }
       }

   EXORDIUM_SERVICE_INIT_FUNCTION
     {
	services.registerService(name,name,"ircdome.org","+dz",
				 "\037Serv\037ice :)");
	services.serviceJoin(name,"#Debug");
	return new Module("serv", new Serv(services));
     }

};

