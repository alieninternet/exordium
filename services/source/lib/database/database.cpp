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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "exordium/database/database.h"

#ifdef HAVE_MYSQL
# include <exordium/database/mysql/dbmysql.h>
#endif

#ifdef HAVE_PGSQL
# include <exordium/database/postgresql/dbpgsql.h>
#endif

using namespace Exordium;
using AISutil::String;

CDatabase::CDatabase(Config &c) : config(c)
{
#ifdef HAVE_MYSQL
   db_supported_engines.mysql = true;
#elif defined(HAVE_PGSQL)
   db_supported_engines.pgsql = true;
#endif

  String dbengine = config.getSqlEngine();

#ifdef DEBUG
   std::clog << "dbengine:" << dbengine << std::endl;
#endif

  if (dbengine == "mysql")
   {

     if(db_supported_engines.mysql == false)
      {
#ifdef DEBUG
         std::cout << "FATAL: The datbase engine specified in the configuration file is not built-in. Please change the database engine or re-build to enable it." << std::endl;
#endif
         exit(1); // YUK, gotta find a way to exit more nicely
      }      

#ifdef HAVE_MYSQL
      database =(CBase*) new CMySQL(config);
#endif

     db_engines = db_mysql;
   }

  else if(dbengine == "postgresql")

   {

     if(db_supported_engines.pgsql == false)
      {
#ifdef DEBUG
         std::cout << "FATAL: The datbase engine specified in the configuration file is not built-in. Please change the database engine or re-build to enable it." << std::endl;
#endif
         exit(1); // YUK, gotta find a way to exit more nicely
      }
      
#ifdef HAVE_PGSQL
      database =(CBase*) new CPgSQL(config);
#endif

     db_engines = db_pgsql;
   }
  else 
   
   {
#ifdef DEBUG
     std::cout << "No Database engine selected! Please add engine=<myengine> to ircd.conf!" << std::endl;
#endif
     exit(1);
   
   }
}




int CDatabase::dbSelect(String const &table)
{
  return database->dbQuery("SELECT * FROM " + table);
}


// Select <fields> from <table>
int CDatabase::dbSelect(String const &fields, String const &table)
{
  return database->dbQuery("SELECT " + fields + " FROM " + table);
}

// Select <fields> from <table> where <whereargs>
int CDatabase::dbSelect(String const &fields, String const &table, String const &whereargs)
{
  return database->dbQuery("SELECT " + fields + " FROM " + table + " WHERE " + whereargs);
}


// Select count(*) from <table>
int CDatabase::dbCount(String const &table)
{
  database->dbQuery("SELECT COUNT(*) FROM " + table);
  return database->dbGetValue().toInt();
}


// Select count(*) from <table> where <whereargs>
int CDatabase::dbCount(String const &table, String const &whereargs)
{
  database->dbQuery("SELECT COUNT(*) FROM " + table + " WHERE " + whereargs);
  return database->dbGetValue().toInt();
}

int CDatabase::dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs,AISutil::String const &orderargs)
{
  database->dbQuery("SELECT COUNT(*) FROM " + table + " WHERE " + whereargs + " ORDER BY " + orderargs);
  return database->dbGetValue().toInt();
}


// Insert into <table> values <values>
void CDatabase::dbInsert(String const &table,  String const &values)
{
  database->dbLock(table);  
  database->dbBeginTrans();
  database->dbQuery("INSERT into " + table + " VALUES (" + values + ")");
  database->dbCommit();
  database->dbUnlock();
  database->dbClearRes();
}


void CDatabase::dbUpdate(String const &table, String const &values, String const &whereargs)
{
  database->dbLock(table);
  database->dbBeginTrans();
  database->dbQuery("UPDATE " + table + " SET " + values + " WHERE " + whereargs);
  database->dbCommit();
  database->dbUnlock();
  database->dbClearRes();
}



// Delete * from <table> where <whereargs>
void CDatabase::dbDelete(String const &table, String const &whereargs)
{
  database->dbLock(table);
  database->dbBeginTrans();
  database->dbQuery("DELETE  FROM " + table + " WHERE " + whereargs);
  database->dbCommit();
  database->dbUnlock();
  database->dbClearRes();
}



// Delete * from <table>
void CDatabase::dbDelete(String const &table)
{
  database->dbLock(table);
  database->dbBeginTrans();
  database->dbQuery("DELETE FROM " + table);
  database->dbCommit();
  database->dbUnlock();
  database->dbClearRes();
}
