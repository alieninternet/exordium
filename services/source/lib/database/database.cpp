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


#include <exordium/database/database.h>
#include <exordium/config.h>

using AISutil::String;


#ifdef HAVE_MYSQL
  #include "mysql/dbmysql.h"
#endif

#ifdef HAVE_PGSQL
  #include "postgresql/dbpgsql.h"
#endif

namespace Exordium
{

Database::Database(void)
{

  #ifdef HAVE_MYSQL
     db_supported_engines.mysql = true;
     sqldrivers["MYSQL"]=(SqlBase*) new dbMySQL();
  #endif

  #ifdef HAVE_PGSQL
     db_supported_engines.pgsql = true;
     sqldrivers["PGSQL"]=(SqlBase*) new dbPgSQL();
  #endif

  #ifdef HAVE_FIREBIRD
     db_supported_engines.firebird = true;
     sqldrivers["FIREBIRD"]=(SqlBase*) new dbFirebird();
  #endif

  
  String dbengine = Config::instance()->getSqlEngine();

  if(sqldrivers.find(dbengine.toUpper())!=sqldrivers.end())
     selectedDriver=dbengine.toUpper();
  else // doh
  {
     //sqldrivers["TEXTFILES"]=(SqlBase*) new dbTextFiles();
     selectedDriver="TEXTFILES";
  }

}


void Database::useDriver(String const &driver)
{
  if(sqldrivers.find(driver)!=sqldrivers.end())
    selectedDriver=driver;
}


void Database::dbSelect(String const &table)
{
  sqldrivers[selectedDriver]->dbSelect("*", table);
}


// Select <fields> from <table>
void Database::dbSelect(String const &fields, String const &table)
{
  sqldrivers[selectedDriver]->dbSelect(fields, table);
}

// Select <fields> from <table> where <whereargs>
void Database::dbSelect(String const &fields, String const &table, String const &whereargs)
{
  sqldrivers[selectedDriver]->dbSelect(fields, table, whereargs);
}


void Database::dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs,AISutil::String const &orderargs)
{
  sqldrivers[selectedDriver]->dbSelect(fields, table, whereargs, orderargs);
}



// Select count(*) from <table>
void Database::dbCount(String const &table)
{
  sqldrivers[selectedDriver]->dbCount(table);
}


// Select count(*) from <table> where <whereargs>
void Database::dbCount(String const &table, String const &whereargs)
{
  sqldrivers[selectedDriver]->dbCount(table, whereargs);
}



// Insert into <table> values <values>
void Database::dbInsert(String const &table,  String const &values)
{
  sqldrivers[selectedDriver]->dbInsert(table, values);
}


void Database::dbUpdate(String const &table, String const &values, String const &whereargs)
{
  sqldrivers[selectedDriver]->dbUpdate(table, values, whereargs);
}



// Delete * from <table> where <whereargs>
void Database::dbDelete(String const &table, String const &whereargs)
{
  sqldrivers[selectedDriver]->dbDelete(table, whereargs); 
}


// Delete * from <table>
void Database::dbDelete(String const &table)
{
  sqldrivers[selectedDriver]->dbDelete(table); 
}



int Database::dbResults(void)
{
  return sqldrivers[selectedDriver]->dbResults();
}


};
