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


#ifndef _INCLUDE_EXORDIUM_DATABASE_POSTGRESQL_DBPGSQL_H_
# define _INCLUDE_EXORDIUM_DATABASE_POSTGRESQL_DBPGSQL_H_ 1


#include <exordium/database/base.h>

#include <kineircd/str.h>

#ifdef HAVE_LIBPQ_FE_H
# include <libpq-fe.h>
#elif defined(HAVE_LIBPQ_INT_H)
# include <libpq-int.h>
#endif


namespace Exordium {

  /*
   * dbPgSQL: Class for PostgreSql server
   *
   *
   */
  class dbPgSQL:public SqlBase
  { 
    public:
      dbPgSQL(void) 
       : pgres(0),
         pgopts(0),
         pgtty(0),
         clearres(false)
       {};

      ~dbPgSQL(void);


      void dbConnect(void);
      void dbDisconnect(void);

      void dbSelectDB(AISutil::String const &dbName);

      void dbSelect(AISutil::String const &table);
      void dbSelect(AISutil::String const &fields, AISutil::String const &table);
      void dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs);
      void dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs,AISutil::String const &orderargs);

      void dbCount(AISutil::String const &table);
      void dbCount(AISutil::String const &table, AISutil::String const &whereargs);

      void dbInsert(AISutil::String const &table,  AISutil::String const &values);

      void dbUpdate(AISutil::String const &table, AISutil::String const &values, AISutil::String const &whereargs);

      void dbDelete(AISutil::String const &table, AISutil::String const &whereargs);
      void dbDelete(AISutil::String const &table);

      void dbQuery(AISutil::String const &query);

      AISutil::String dbGetValue(void);
      AISutil::String dbGetValue(int field);

      void dbGetRow(void);

      void dbClearRes(void);
      int dbResults(void);
      int dbNbCols(void);

      void getFieldNames(AISutil::String const &table);

      bool eof(void);

      int affectedRows(void);


      void dbBeginTrans(void);
      void dbCommit(void);
      void dbRollback(void);

      // Locking is done automatically by postgresql
      void dbLock(AISutil::String const &table) {}
      void dbUnlock(void) {}

    private:
      PGconn *pgconn;
      PGresult *pgres;
      char *pgopts;
      char *pgtty;
 
      int currow;
      bool clearres;

  }; // class dbPgSQL

}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_DATABASE_POSTGRESQL_DBPGSQL_H_
