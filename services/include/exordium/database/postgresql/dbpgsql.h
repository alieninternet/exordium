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
#include <exordium/log.h>


#include <libais/string/string.h>

#ifdef HAVE_LIBPQ_FE_H
  #include <libpq-fe.h>
#elif defined(HAVE_LIBPQ_INT_H)
  #include <libpq-int.h>
#endif

using LibAIS::String;

namespace Exordium {

  /*
   * CPgSQL: Class for PostgreSql server
   *
   *
   */
  class CPgSQL:public CBase
  { 
    public:
      CPgSQL(Config &c, Log &l) : config(c), logger(l)
      { pgopts=NULL; pgtty=NULL; }

      virtual ~CPgSQL(void);

      virtual void dbConnect(void);
      virtual void dbDisconnect(void);

      virtual void dbSelectDB(LibAIS::String const &dbName);

      virtual int dbQuery(LibAIS::String const &query);

      virtual LibAIS::String dbGetValue(void);
      virtual LibAIS::String dbGetValue(int field);

      virtual void dbGetRow(void);


      virtual void dbClearRes(void);

      virtual void dbBeginTrans(void);
      virtual void dbCommit(void);
      virtual void dbRollback(void);


      // Locking is done automatically by postgresql
      virtual void dbLock(LibAIS::String const &table) {}
      virtual void dbUnlock(void) {}

    private:
      Config &config;
      Log &logger;
 
      PGconn *pgconn;
      PGresult *pgres;
      char *pgopts;
      char *pgtty;

      int currow;

  }; // class CPgSQL

}; // namespace Exordium

#endif // _PGSQL_H
