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

using namespace Exordium;
using AISutil::String;

CPgSQL::~CPgSQL(void)
{
  if(pgres != NULL)
    PQclear(pgres);

  // 'the object which deletes himself'
  delete this;
}


void CPgSQL::dbConnect()
{
  if(connected == false)
   {
      pgconn = PQsetdbLogin(config.getSqlHostname().c_str(), String::convert(config.getSqlPort()).c_str(), pgopts, pgtty, 
                            config.getSqlDatabase().c_str(), config.getSqlUsername().c_str(), 
                            config.getSqlPassword().c_str());

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


void CPgSQL::dbDisconnect()
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


int CPgSQL::dbQuery(String const &query)
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
      return 0;
    }
    else if(PQresultStatus(pgres)==PGRES_TUPLES_OK)
    {
      clearres=true;
      return PQntuples(pgres);
    }
  }
  else
     return 0;
}




String CPgSQL::dbGetValue(void)
{
  if(PQntuples(pgres)>0)
     return PQgetvalue(pgres, currow, 0);
  else
     return "";
}


String CPgSQL::dbGetValue(int field)
{
  if(PQntuples(pgres)>0)
     return PQgetvalue(pgres, currow, field);
  else
     return "";
}

void CPgSQL::dbGetRow(void)
{
  currow++;
}


void CPgSQL::dbClearRes(void)
{
  PQclear(pgres);
}


void CPgSQL::dbBeginTrans(void)
{
  pgres = PQexec(pgconn, "BEGIN");
  PQclear(pgres);
}


void CPgSQL::dbCommit(void)
{
  pgres = PQexec(pgconn, "COMMIT");
  PQclear(pgres);
}


void CPgSQL::dbRollback(void)
{
  pgres = PQexec(pgconn, "ROLLBACK");
  PQclear(pgres);
}



// DB is selected in the connection sequence
void CPgSQL::dbSelectDB(String const &dbName)
{
}
