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

/* This is ugly, if someone has a solution please tell me :)
 * Normally, only supported DBs should be compiled in, but I dont know
 * of a way to change libservices_la_SOURCES that much, CPPFLAGS etc are easy but..
 */
#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#ifndef HAVE_MYSQL
# error "Cannot compile mysql support without mysql!"
#endif

#include "dbmysql.h"
#include <exordium/config.h>

using AISutil::String;

namespace Exordium
{

dbMySQL::~dbMySQL()
{
  delete mysql;
  mysql_free_result(mysqlres);

  delete this;
}


void dbMySQL::dbConnect()
{
  if(connected == false)
   {
      if (mysql_connect(mysql, configFwd.getSqlHostname().c_str(), configFwd.getSqlUsername().c_str(), configFwd.getSqlPassword().c_str()))
      {
         connected = true;
         dbSelectDB(configFwd.getSqlDatabase().c_str());
      }
      else
      {
#ifdef DEBUG
         std::cout << "Error connecting to database: " << mysql_error(mysql) << std::endl;
#endif
         exit(1); // should find a better way
      }
   }
#ifdef DEBUG
  else
     std::cout << "WARNING: Tried to connect to DB while already connected!" << std::cout;
#endif
}


void dbMySQL::dbDisconnect()
{
  if(connected == true)
  {
    mysql_close(mysql);   
    connected=false;
  }
#ifdef DEBUG
  else
    std::cout << "WARNING: Tried to disconnect from DB while not connected!" << std::cout;
#endif
}


void dbMySQL::dbQuery(String const &query)
{
  // Check if we are still connected
  if(mysql_ping(mysql) != 0)
  {
    connected=false;
    dbConnect();
  }


  // Free previous result
  if (mysqlres != NULL)
  {
    mysql_free_result(mysqlres);
    mysqlres = NULL;
  }


#ifdef DEBUG
  std::clog << "DEBUG: Query=" << query << std::endl;
#endif

  // If mysql_real_query returns 0 it means it succeeded
  if(mysql_real_query(mysql, query.data(), query.length()) == 0)
   {
     if(mysql_field_count(mysql) > 0)
        mysqlres = mysql_store_result(mysql);
     
     if(mysqlres != NULL)
     {
        mysqlrow = mysql_fetch_row(mysqlres);
     }
     else
        isEOF=true;          
   }
#ifdef DEBUG
  else
   {
     std::cout << "Warning: dbMySQL::dbQuery returned an error!" << std::endl;
     std::cout << mysql_error(mysql) << std::endl;
   }
#endif
   
}


String dbMySQL::dbGetValue(void)
{
  if( mysqlres != NULL )
    return mysqlrow[0];
  else
  {
    isEOF=true;
    return "";
  }
}


String dbMySQL::dbGetValue(int field)
{
  if( mysqlres != NULL )
    return mysqlrow[field];
  else
  {
    isEOF=true;
    return "";
  }
}


void dbMySQL::dbGetRow(void)
{
  if( mysqlres != NULL )
    mysqlrow = mysql_fetch_row(mysqlres);
  else
    isEOF=true;
}



// we dont clear the result with MySQL. mysqlres is allocated by the constructor.
void dbMySQL::dbClearRes(void)
{}


void dbMySQL::dbSelectDB(String const &dbName)
{
  if (mysql_select_db(mysql, dbName.c_str()) != 0) {
#ifdef DEBUG
    std::cout << "ERROR: Failed to select database: " << dbName << std::endl;
#endif
  }
}


void dbMySQL::dbLock(AISutil::String const &table)
{
  dbQuery("LOCK TABLES " + table + " WRITE");
}

void dbMySQL::dbUnlock(void)
{
  dbQuery("UNLOCK TABLES");
}



void dbMySQL::dbSelect(String const &table)
{
  dbQuery("SELECT * from "+table);
}


// Select <fields> from <table>
void dbMySQL::dbSelect(String const &fields, String const &table)
{
  dbQuery("SELECT "+fields+" FROM "+ table);
}

// Select <fields> from <table> where <whereargs>
void dbMySQL::dbSelect(String const &fields, String const &table, String const &whereargs)
{
  dbQuery("SELECT "+fields+" FROM "+ table + " WHERE " + whereargs);
}



// Select count(*) from <table>
void dbMySQL::dbCount(String const &table)
{
  dbQuery("SELECT COUNT(*) FROM "+table);
}


// Select count(*) from <table> where <whereargs>
void dbMySQL::dbCount(String const &table, String const &whereargs)
{
  dbQuery("SELECT COUNT(*) FROM "+table+ " WHERE " + whereargs);
}


void dbMySQL::dbSelect(String const &fields, String const &table, String const &whereargs,String const &orderargs)
{
  dbQuery("SELECT "+fields+" FROM "+ table + " WHERE " + whereargs + " ORDER BY " + orderargs);
}



// Insert into <table> values <values>
void dbMySQL::dbInsert(String const &table,  String const &values)
{
  dbQuery("INSERT into "+table+" VALUES "+values);
}


void dbMySQL::dbUpdate(String const &table, String const &values, String const &whereargs)
{
  dbQuery("UPDATE " + table + " SET " + values + " WHERE " + whereargs);
}



// Delete * from <table> where <whereargs>
void dbMySQL::dbDelete(String const &table, String const &whereargs)
{
  dbQuery("DELETE from " + table + " WHERE " + whereargs);
}


// Delete * from <table>
void dbMySQL::dbDelete(String const &table)
{
  dbQuery("DELETE from " + table);
}


int dbMySQL::dbResults(void)
{
   if(!eof() && mysqlres!=NULL) 
      return mysql_num_rows(mysqlres);
   else
      return 0;
}



int dbMySQL::dbNbCols(void)
{
  if(!eof() && mysqlres!=NULL)
     return mysql_num_fields(mysqlres);
  else
     return 0;
}


void dbMySQL::getFieldNames(String const &table)
{
  dbQuery("SHOW COLUMNS FROM "+table);
}



bool dbMySQL::eof(void)
{
  return isEOF;
}



int dbMySQL::affectedRows(void)
{
  return mysql_affected_rows(mysql);
}


};
