/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#include "exordium/sql.h"
#include "exordium/mysql.h"
#include "exordium/services.h"
#include "exordium/log.h"
#include "kineircd/str.h"
#include <sys/time.h>

using
    Kine::String;
using namespace
    Exordium;

Mysql
    mysql;
MysqlRes Sql::query(String const &query)
{
    struct timeval
	start;
    Log::logLine(String("MySQL Query Debug: ") + query);
    gettimeofday(&start, NULL);
    if (mysql.query(query.c_str())) {
	Log::logLine(String("MySQL Error: ") + mysql.error());
	String
	    toshout = (String("MySQL Error: ") + mysql.error());
	Services::helpme(toshout, "Serv");
    }
    struct timeval
	finish;
    gettimeofday(&finish, NULL);
    int
	time =
	finish.
	tv_usec -
	start.
	tv_usec;
    String
	tolog =
	String("MySQL Query Took ") +
	String::convert(time) +
	" microseconds";
    Log::logLine(tolog);
    Services::Debug(query);
    Services::Debug(tolog);
    MysqlRes
	togo =
	mysql.
	store_result();
    return togo;
}

String
Sql::makeSafe(String const &line)
{
	char *temp = new char[line.length() + 1];
	for (register unsigned int i = (line.length() + 1); i--;)
	{
		char ch = line.c_str()[i];
		switch(ch) {
			case '\'': /* Breaks Sql queries */
				temp[i] = ' ';
				continue;
			case '\t': /* And this one too */
				temp[i] = ' ';
				continue;
			case '\\': /* This ones evil too :( */
				temp[i] = ' ';
				continue;
			case '"': /* Better safe than sorry */
				temp[i] = ' ';
				continue;
			case '\032': /* Apprantly causes problems on certain os's :) */
				temp[i] = ' ';
				continue;
			default:
				temp[i] = ch;
		}
	}
	String result(temp);
	delete(temp);
	return result;
}

void
 
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    Sql::init(String const &host, String const &user, String const &pass,
	      String const &db)
{
    String          tolog =
	String("MySQL Configuration: ") + host + " " + user + " " + pass +
	" " + db;
    Log::logLine(tolog);
    if (!mysql.connect(host.c_str(), user.c_str(), pass.c_str())) {
	Log::logLine(String("MySQL Error: ") + mysql.error());
	exit(1);
    }
    Log::logLine("--> Successfully connected to MySQL database");
    if (mysql.select_db(db.c_str())) {
	Log::logLine(String("MySQL Error: ") + mysql.error());
	exit(1);
    }
    Log::logLine("--> Successfully selected services database");
    query("DELETE from onlineclients");
    query("DELETE from chanstatus");
    query("DELETE from identified");
    query("DELETE from kills");
    query("DELETE from onlineservers");
    query("DELETE from onlinechan");
}
