
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
#include <kineircd/str.h>
#include <sys/time.h>
#include <sstream>

using
    Kine::String;
using namespace
    Exordium;

Mysql
    mysql;
MysqlRes Sql::query(String const &query)
{
    struct timeval start;
    Log::logLine(String("MySQL Query Debug: ") + query);
    gettimeofday(&start, NULL);
    if (mysql.query(query.c_str())) {
	Log::logLine(String("MySQL Error: ") + mysql.error());
	String toshout = (String("MySQL Error: ") + mysql.error());
	Services::helpme(toshout, "Serv");
    }
    struct timeval finish;
    gettimeofday(&finish, NULL);
    long long time = ((((long long)finish.tv_sec * 1000000) + finish.tv_usec) - (((long long)start.tv_sec * 1000000) + start.tv_usec));
    String tolog = String("MySQL Query Took ") + String::convert(time) + " microseconds";
    Log::logLine(tolog);
    //MysqlRes togo = mysql.store_result();
    return mysql.store_result();
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
  Sql::init(const Config &config)
{
   std::ostringstream tolog;
   tolog << "MySQL Configuration: " << config.getSqlHostname() << ' ' <<
     config.getSqlUsername() << ' ' << config.getSqlPassword() << ' ' <<
     config.getSqlDatabase();
    Log::logLine(tolog.str());
    if (!mysql.connect(config.getSqlHostname().c_str(), 
		       config.getSqlUsername().c_str(), 
		       config.getSqlPassword().c_str())) {
	Log::logLine(String("MySQL Error: ") + mysql.error());
	exit(1);
    }
    Log::logLine("--> Successfully connected to MySQL database");
    if (mysql.select_db(config.getSqlDatabase().c_str())) {
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
    query("SET autocommit=0");
}
