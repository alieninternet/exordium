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

#include "exordium/sql.h"
#include "exordium/mysql.h"
#include "exordium/log.h"
#include <kineircd/str.h>
#include <sys/time.h>
#include <sstream>

using Kine::String;
using namespace Exordium;

Mysql mysql;

Sql::Sql(Log& l, const Config& config)
  : sock(-1),
logger(l)
{
   std::ostringstream tolog;
   tolog << "MySQL Configuration: " << config.getSqlHostname() << ' ' <<
     config.getSqlUsername() << ' ' << config.getSqlPassword() << ' ' <<
     config.getSqlDatabase();
   logger.logLine(tolog.str());

   if (!mysql.connect(config.getSqlHostname().c_str(),
		      config.getSqlUsername().c_str(),
		      config.getSqlPassword().c_str()))
     {
	logger.logLine(String("MySQL Error: ") + mysql.error());
	exit(1);
     }
   logger.logLine("--> Successfully connected to MySQL database");

   if (mysql.select_db(config.getSqlDatabase().c_str()))
     {
	logger.logLine(String("MySQL Error: ") + mysql.error());
	exit(1);
     }
   logger.logLine("--> Successfully selected services database");

   query("DELETE from onlineclients");
   query("DELETE from chanstatus");
   query("DELETE from identified");
   query("DELETE from kills");
   query("DELETE from onlineservers");
   query("DELETE from onlinechan");
}

MysqlRes Sql::query(const char* const query)
{
   struct timeval start;
   logger.logLine(String("MySQL Query Debug: ") + query);
   gettimeofday(&start, NULL);

   if (mysql.query(query))
     {
	logger.logLine(String("MySQL Error: ") + mysql.error());
     }

   struct timeval finish;
   gettimeofday(&finish, NULL);
   long long time = ((((long long)finish.tv_sec * 1000000) + finish.tv_usec) - (((long long)start.tv_sec * 1000000) + start.tv_usec));
   String tolog = String("MySQL Query Took ") + String::convert(time) +
     " microseconds";
   logger.logLine(tolog);
   return mysql.store_result();
}

MysqlRes Sql::query(const String& query)
{
   struct timeval start;
   logger.logLine(String("MySQL Query Debug: ") + query);
   gettimeofday(&start, NULL);

   if (mysql.query(query))
     {
	logger.logLine(String("MySQL Error: ") + mysql.error());
     }

   struct timeval finish;
   gettimeofday(&finish, NULL);
   long long time = ((((long long)finish.tv_sec * 1000000) + finish.tv_usec) - (((long long)start.tv_sec * 1000000) + start.tv_usec));
   String tolog = String("MySQL Query Took ") + String::convert(time) +
     " microseconds";
   logger.logLine(tolog);
   return mysql.store_result();
}

String Sql::makeSafe(const String& line)
{
   char *temp = new char[line.length() + 1];

   for (register unsigned int i = (line.length() + 1); i--;)
     {
	char ch = line.c_str()[i];
	switch(ch)
	  {
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

