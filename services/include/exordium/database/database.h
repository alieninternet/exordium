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


#ifndef _INCLUDE_EXORDIUM_DATABASE_DATABASE_H_
# define _INCLUDE_EXORDIUM_DATABASE_DATABASE_H_ 1


# include <aisutil/string/string.h>
# include <exordium/database/base.h>
# include <exordium/database/table.h>
#include <exordium/singleton.h>

#include <map>

using namespace std;

namespace Exordium {

  class Database;

  #define databaseFwd (*Database::instance())

  struct db_supported_engines_t
  {
     bool mysql;
     bool pgsql;
     bool firebird;
  };


  /*
   * Public API for databases
   *
   *
   */
  class Database:public Singleton<Database>
  { 
    public:
      friend class Singleton<Database>;


      void dbConnect(void) { sqldrivers[selectedDriver]->dbConnect(); }
      void dbDisconnect(void) { sqldrivers[selectedDriver]->dbDisconnect(); }

      void useDriver(AISutil::String const &driver);

      // Select * from <table>
      void dbSelect(AISutil::String const &table);

      // Select <fields> from <table>
      void dbSelect(AISutil::String const &fields, AISutil::String const &table);

      // Select <fields> from <table> where <whereargs>
      void dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs);

      // Select <fields> from <table> where <whereargs> order by <orderargs>
      void dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs, AISutil::String const &orderargs);


      // Select count(*) from <table>
      void dbCount(AISutil::String const &table);

      // Select count(*) from <table> where <whereargs>
      void dbCount(AISutil::String const &table, AISutil::String const &whereargs);



      // Insert into <table> values <values>
      void dbInsert(AISutil::String const &table,  AISutil::String const &values);

      void dbUpdate(AISutil::String const &table, AISutil::String const &values, AISutil::String const &whereargs);



      // Delete * from <table> where <whereargs>
      void dbDelete(AISutil::String const &table, AISutil::String const &whereargs);

      // Delete * from <table>
      void dbDelete(AISutil::String const &table);

      int dbResults(void);

      void dbQuery(AISutil::String const &query) { sqldrivers[selectedDriver]->dbQuery(query); }

      AISutil::String dbGetValue(void) { return(sqldrivers[selectedDriver]->dbGetValue()); }
      AISutil::String dbGetValue(int field) { return(sqldrivers[selectedDriver]->dbGetValue(field)); }

      void dbGetRow(void) { sqldrivers[selectedDriver]->dbGetRow(); }

      bool eof(void) { return sqldrivers[selectedDriver]->eof(); }

      int affectedRows(void) { return sqldrivers[selectedDriver]->affectedRows(); }

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
     
      
      
    private:
      Database(void);

      // Maps a driver name with an object ptr
      map<AISutil::String, SqlBase*> sqldrivers;

      AISutil::String selectedDriver;
   
      db_supported_engines_t db_supported_engines;

      void dbSelectDB(AISutil::String const &dbName) { sqldrivers[selectedDriver]->dbSelectDB(dbName); }

  }; // class Database

}; // namespace Exordium



#endif // _INCLUDE_EXORDIUM_DATABASE_DATABASE_H_
