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


#ifndef _DATABASE_H
#define _DATABASE_H


#include <libais/string/string.h>
#include <exordium/database/base.h>
#include <exordium/conf.h>
#include <exordium/log.h>


#ifdef HAVE_MYSQL
 #include "exordium/database/mysql/dbmysql.h"
#endif

#ifdef HAVE_PGSQL
 #include "exordium/database/postgresql/dbpgsql.h"
#endif




namespace Exordium {

  struct db_supported_engines_t
  {
     bool mysql;
     bool pgsql;
  };


  class Config;
  class Log;

  /*
   * Public API for databases
   *
   *
   */
  class CDatabase
  { 
    public:
      CDatabase(Config &c, Log &l);


      ~CDatabase(void)
      {
        delete database;
      }



      void dbConnect(void) { database->dbConnect(); }
      void dbDisconnect(void) { database->dbDisconnect(); }


      // Select * from <table>
      int dbSelect(LibAIS::String const &table);

      // Select <fields> from <table>
      int dbSelect(LibAIS::String const &fields, LibAIS::String const &table);

      // Select <fields> from <table> where <whereargs>
      int dbSelect(LibAIS::String const &fields, LibAIS::String const &table, LibAIS::String const &whereargs);

      // Select <fields> from <table> where <whereargs> order by <orderargs>
      int dbSelect(LibAIS::String const &fields, LibAIS::String const &table, LibAIS::String const &whereargs, LibAIS::String const &orderargs);


      // Select count(*) from <table>
      int dbCount(LibAIS::String const &table);

      // Select count(*) from <table> where <whereargs>
      int dbCount(LibAIS::String const &table, LibAIS::String const &whereargs);



      // Insert into <table> values <values>
      void dbInsert(LibAIS::String const &table,  LibAIS::String const &values);

      void dbUpdate(LibAIS::String const &table, LibAIS::String const &values, LibAIS::String const &whereargs);



      // Delete * from <table> where <whereargs>
      void dbDelete(LibAIS::String const &table, LibAIS::String const &whereargs);

      // Delete * from <table>
      void dbDelete(LibAIS::String const &table);





      int dbQuery(LibAIS::String const &query) { return(database->dbQuery(query)); }

      LibAIS::String dbGetValue(void) { return(database->dbGetValue()); }
      LibAIS::String dbGetValue(int field) { return(database->dbGetValue(field)); }

      void dbGetRow(void) { database->dbGetRow(); }


      
      enum
      {
        db_mysql  = 1,
        db_pgsql  = 2,
      };
      
      typedef int db_engines_t;

    private:
      Config &config;
      Log &logger;

      CBase *database;

      // Ensure a certain 'sanity', do not overuse the preprocessor
      db_engines_t db_engines;
     
      db_supported_engines_t db_supported_engines;

      void dbBeginTrans(void) { database->dbBeginTrans(); }
      void dbCommit(void) { database->dbCommit(); }
      void dbRollback(void) { database->dbRollback(); }

      void dbSelectDB(LibAIS::String const &dbName) { database->dbSelectDB(dbName); }


  }; // class CDatabase

}; // namespace Exordium



#endif // _DATABASE_H
