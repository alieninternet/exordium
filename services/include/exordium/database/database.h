/* $Id$
 *
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */


#ifndef _INCLUDE_EXORDIUM_DATABASE_DATABASE_H_
# define _INCLUDE_EXORDIUM_DATABASE_DATABASE_H_ 1


# include <aisutil/string/string.h>
# include <exordium/database/base.h>
# include <exordium/database/result.h>
# include <exordium/database/table.h>
# include <exordium/config.h>


namespace Exordium {

  struct db_supported_engines_t
  {
     bool mysql;
     bool pgsql;
  };


  class Config;

  /*
   * Public API for databases
   *
   *
   */
  class CDatabase
  { 
    public:
      CDatabase(Config &c);


     ~CDatabase(void)
       {
	  dbDisconnect();
	  delete database;
       }



      void dbConnect(void) { database->dbConnect(); }
      void dbDisconnect(void) { database->dbDisconnect(); }


      // Select * from <table>
     int dbSelect(AIS::Util::String const &table);

      // Select <fields> from <table>
     int dbSelect(AIS::Util::String const &fields, AIS::Util::String const &table);

      // Select <fields> from <table> where <whereargs>
     int dbSelect(AIS::Util::String const &fields, AIS::Util::String const &table, AIS::Util::String const &whereargs);

      // Select <fields> from <table> where <whereargs> order by <orderargs>
     int dbSelect(AIS::Util::String const &fields, AIS::Util::String const &table, AIS::Util::String const &whereargs, AIS::Util::String const &orderargs);


      // Select count(*) from <table>
     int dbCount(AIS::Util::String const &table);

      // Select count(*) from <table> where <whereargs>
     int dbCount(AIS::Util::String const &table, AIS::Util::String const &whereargs);



      // Insert into <table> values <values>
     void dbInsert(AIS::Util::String const &table,  AIS::Util::String const &values);

     // (Delayed - Not important) Insert into <table> values <values>
     void dbDelayedInsert(AIS::Util::String const &table, AIS::Util::String const &values);
     
     void dbUpdate(AIS::Util::String const &table, AIS::Util::String const &values, AIS::Util::String const &whereargs);


      // Delete * from <table> using <usingargs> WHERE <whereargs>
     void dbDelete(AISutil::String const &table, AISutil::String const &usingargs, AISutil::String const &whereargs);
     
      // Delete * from <table> where <whereargs>
      void dbDelete(AISutil::String const &table, AISutil::String const &whereargs);

      // Delete * from <table>
      void dbDelete(AISutil::String const &table);

      // Return a result set
      CResult* dbGetResultSet(void);



      int dbQuery(AISutil::String const &query) { return(database->dbQuery(query)); }

      AISutil::String dbGetValue(void) { return(database->dbGetValue()); }
      AISutil::String dbGetValue(int field) { return(database->dbGetValue(field)); }

      void dbGetRow(void) { database->dbGetRow(); }


      /* Affirm a table's structure. Return true if the table exists. If the
       * table does not exist, or has missing fields, then the database engine
       * will create the table or add the missing fields respectively. If the
       * specific engine cannot create/modify tables on the fly or there's any
       * other reason it was unable to affirm the table structure is identical
       * to that described in the given table structure, then false will be
       * returned.
       */
      const bool affirmTable(const DatabaseTable& table)
       { return true; /* <=- temporary; waiting for the new database code*/ };
     
      
      enum
      {
        db_mysql  = 1,
        db_pgsql  = 2,
      };
      
      typedef int db_engines_t;

    private:
      Config &config;

      CBase *database;

      // Ensure a certain 'sanity', do not overuse the preprocessor
      db_engines_t db_engines;
     
      db_supported_engines_t db_supported_engines;

      void dbBeginTrans(void) { database->dbBeginTrans(); }
      void dbCommit(void) { database->dbCommit(); }
      void dbRollback(void) { database->dbRollback(); }

      void dbSelectDB(AISutil::String const &dbName) { database->dbSelectDB(dbName); }


  }; // class CDatabase

}; // namespace Exordium



#endif // _INCLUDE_EXORDIUM_DATABASE_DATABASE_H_
