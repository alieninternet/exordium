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


#ifndef _PGSQL_H
#define _PGSQL_H


#include <exordium/database/base.h>
#include <exordium/conf.h>


#include <aisutil/string/string.h>

#ifdef HAVE_LIBPQ_FE_H
# include <libpq-fe.h>
#elif defined(HAVE_LIBPQ_INT_H)
# include <libpq-int.h>
#endif


namespace Exordium {

  /*
   * CPgSQL: Class for PostgreSql server
   *
   *
   */
  class CPgSQL:public CBase
  { 
    public:
      CPgSQL(Config &c) 
       : CBase(c),
         pgopts(0),
         pgtty(0),
         pgres(0),
         clearres(false)
       {};

      ~CPgSQL(void);

      void dbConnect(void);
      void dbDisconnect(void);

      void dbSelectDB(AISutil::String const &dbName);

      int dbQuery(AISutil::String const &query);

      AISutil::String dbGetValue(void);
      AISutil::String dbGetValue(int field);

      void dbGetRow(void);


      void dbClearRes(void);

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

  }; // class CPgSQL

}; // namespace Exordium

#endif // _PGSQL_H
