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

#ifndef HAVE_PGSQL
# error "Cannot compile PostgreSQL support without PostgreSQL!"
#endif

#include "dbpgsql.h"
#include <exordium/config.h>

using AISutil::String;

namespace Exordium
{

dbPgSQL::~dbPgSQL(void)
{
  if(pgres != NULL)
    PQclear(pgres);

  // 'the object which deletes himself'
  delete this;
}


void dbPgSQL::dbConnect()
{
  if(connected == false)
   {
      pgconn = PQsetdbLogin(configFwd.getSqlHostname().c_str(), String::convert(configFwd.getSqlPort()).c_str(), pgopts, 
                      pgtty, configFwd.getSqlDatabase().c_str(), configFwd.getSqlUsername().c_str(), 
                             configFwd.getSqlPassword().c_str());

      if(PQstatus(pgconn) == CONNECTION_BAD)
      {  
#ifdef DEBUG
        std::cout << "Error connecting to database: " << String::convert(PQerrorMessage(pgconn)) << std::endl;
#endif
        exit(1); // This is rash, but what can we do if we cant connect to DB
      }
      else
         connected = true;
   }
#ifdef DEBUG
  else
     std::cout << "WARNING: Tried to connect to DB while already connected!" << std::endl;
#endif
}


void dbPgSQL::dbDisconnect()
{
  if(connected == true)
  {
     PQfinish(pgconn); 
     connected=false;
  }
#ifdef DEBUG
  else
    std::cout << "WARNING: Tried to disconnect from DB while not connected!" << std::cout;
#endif
}


void dbPgSQL::dbQuery(String const &query)
{
#ifdef DEBUG
  std::clog << "Before clear!!!" << std::endl;
#endif
   
  // clear results of previous query
  if(clearres) 
   {
     clearres=false;
     PQclear(pgres);
   }

#ifdef DEBUG
   std::clog << "After clear!!!!" << std::endl;
   std::clog << "DEBUG: Query=" << query << std::endl;
#endif

  pgres = PQexec(pgconn, query.data());

#ifdef DEBUG
   std::clog << "After EXEC!!!" << std::endl;
#endif

  currow=0;

  if(pgres != NULL)
  {
    if(PQresultStatus(pgres)==PGRES_COMMAND_OK)
    {
      clearres=false;
      PQclear(pgres);
      isEOF=true;
    }
    else if(PQresultStatus(pgres)==PGRES_TUPLES_OK)
    {
      clearres=true;
      if(PQntuples(pgres)>0)
        isEOF=false;
      else
        isEOF=true;
      
    }
  }
}




String dbPgSQL::dbGetValue(void)
{
  if(PQntuples(pgres)>0)
     return PQgetvalue(pgres, currow, 0);
  else
     return "";
}


String dbPgSQL::dbGetValue(int field)
{
  if(PQntuples(pgres)>0)
     return PQgetvalue(pgres, currow, field);
  else
     return "";
}

void dbPgSQL::dbGetRow(void)
{
  currow++;
}


void dbPgSQL::dbClearRes(void)
{
  PQclear(pgres);
}


void dbPgSQL::dbBeginTrans(void)
{
  pgres = PQexec(pgconn, "BEGIN");
  PQclear(pgres);
}


void dbPgSQL::dbCommit(void)
{
  pgres = PQexec(pgconn, "COMMIT");
  PQclear(pgres);
}


void dbPgSQL::dbRollback(void)
{
  pgres = PQexec(pgconn, "ROLLBACK");
  PQclear(pgres);
}



// DB is selected in the connection sequence
void dbPgSQL::dbSelectDB(String const &dbName)
{
}



void dbPgSQL::dbSelect(String const &table)
{
  dbQuery("SELECT * FROM "+table);
}


// Select <fields> from <table>
void dbPgSQL::dbSelect(String const &fields, String const &table)
{
  dbQuery("SELECT "+fields+" FROM "+table);
}

// Select <fields> from <table> where <whereargs>
void dbPgSQL::dbSelect(String const &fields, String const &table, String const &whereargs)
{
  dbQuery("SELECT "+fields+" FROM "+table+ " WHERE " + whereargs);
}



// Select count(*) from <table>
void dbPgSQL::dbCount(String const &table)
{
  dbQuery("SELECT COUNT(*) FROM " + table);
}


// Select count(*) from <table> where <whereargs>
void dbPgSQL::dbCount(String const &table, String const &whereargs)
{
  dbQuery("SELECT COUNT(*) FROM " + table + " WHERE " + whereargs);
}


void dbPgSQL::dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs,AISutil::String const &orderargs)
{
  dbQuery("SELECT COUNT(*) FROM " + table + " WHERE " + whereargs + " ORDER BY " + orderargs);
}



// Insert into <table> values <values>
void dbPgSQL::dbInsert(String const &table,  String const &values)
{
  dbQuery("INSERT INTO " +table+ " VALUES " +values);
}



void dbPgSQL::dbUpdate(String const &table, String const &values, String const &whereargs)
{
  dbQuery("UPDATE " + table + " SET " + values + " WHERE " + whereargs);
}


// Delete * from <table>
void dbPgSQL::dbDelete(String const &table)
{
  dbQuery("DELETE FROM " +table);
}




// Delete * from <table> where <whereargs>
void dbPgSQL::dbDelete(String const &table, String const &whereargs)
{
  dbQuery("DELETE FROM " + table + " WHERE " + whereargs);
}




int dbPgSQL::dbNbCols(void)
{
  if(!eof() && pgres!=NULL)
     return PQnfields(pgres);
  else
     return 0;
}


void dbPgSQL::getFieldNames(String const &table)
{
  dbQuery("SHOW COLUMNS FROM "+table);
}



bool dbPgSQL::eof(void)
{
  return isEOF;
}


int dbPgSQL::dbResults(void)
{
   if(!eof() && pgres!=NULL)
      return PQntuples(pgres);
   else
      return 0;
}


int dbPgSQL::affectedRows(void)
{
  return 0; // TEMP
}

};
