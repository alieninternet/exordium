/*
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

/* This is ugly, if someone has a solution please tell me :)
 * Normally, only supported DBs should be compiled in, but I dont know
 * of a way to change libservices_la_SOURCES that much, CPPFLAGS etc are easy but..
 */

#ifdef HAVE_MYSQL

#include "exordium/database/mysql/dbmysql.h"


using namespace Exordium;

CMySQL::~CMySQL()
{
  delete mysql;
  mysql_free_result(mysqlres);

  delete this;
}


void CMySQL::dbConnect()
{
  if(connected == false)
   {
      if (mysql_connect(mysql, config.getSqlHostname().c_str(), config.getSqlUsername().c_str(), config.getSqlPassword().c_str()))
      {
         connected = true;
         dbSelectDB(config.getSqlDatabase().c_str());
      }
      else
      {
         std::cout << "Error connecting to database: " << mysql_error(mysql) << std::endl;
         exit(1); // should find a better way
      }
   }
  else
     std::cout << "WARNING: Tried to connect to DB while already connected!" << std::cout;
}


void CMySQL::dbDisconnect()
{
  if(connected == true)
  {
    mysql_close(mysql);   
    connected=false;
  }
  else
    std::cout << "WARNING: Tried to disconnect from DB while not connected!" << std::cout;
}


int CMySQL::dbQuery(String const &query)
{
  // Free previous result
  if (mysqlres != NULL)
  {
    mysql_free_result(mysqlres);
    mysqlres = NULL;
  }


  std::cout << "DEBUG: Query=" << query << std::endl;
  logger.logLine("DEBUG: Query=" + query);

  // If mysql_real_query returns 0 it means it succeeded
  if(mysql_real_query(mysql, query.c_str(), query.length()) == 0)
   {
     mysqlres = mysql_store_result(mysql);
     
     if(mysqlres != NULL)
     {
        mysqlrow = mysql_fetch_row(mysqlres);
        return mysql_num_rows(mysqlres);
     }
     else
       return 0;
   }
  else
   {
     std::cout << "Warning: CMySQL::dbQuery returned an error!" << std::endl;
     std::cout << mysql_error(mysql) << std::endl;
   }

}


String CMySQL::dbGetValue(void)
{
  if( mysqlres != NULL )
    return mysqlrow[0];
  else
    return "";
}


String CMySQL::dbGetValue(int field)
{
  if( mysqlres != NULL )
    return mysqlrow[field];
  else
    return "";
}


void CMySQL::dbGetRow(void)
{
  if( mysqlres != NULL )
    mysqlrow = mysql_fetch_row(mysqlres);
}



// we dont clear the result with MySQL. mysqlres is allocated by the constructor.
void CMySQL::dbClearRes(void)
{}


void CMySQL::dbSelectDB(String const &dbName)
{
  if (mysql_select_db(mysql, dbName.c_str()) != 0)
    std::cout << "ERROR: Failed to select database: " << dbName << std::endl; 
}


void CMySQL::dbLock(LibAIS::String const &table)
{
  dbQuery("LOCK TABLES " + table + " WRITE");
}

void CMySQL::dbUnlock(void)
{
  dbQuery("UNLOCK TABLES");
}

#endif // HAVE_MYSQL
