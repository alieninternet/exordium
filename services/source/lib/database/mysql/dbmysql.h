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


#ifndef _SOURCE_LIB_DATABASE_MYSQL_DBMYSQL_H_
# define _SOURCE_LIB_DATABASE_MYSQL_DBMYSQL_H_ 1

#include <exordium/database/base.h>

#include <aisutil/string/string.h>

#include <mysql/mysql.h>

namespace Exordium {

  /*
   * CMySQL: Class for MySql server
   *
   *
   */
  class CMySQL:public CBase
  { 
    public:
      CMySQL(Config &c)
       : CBase(c),
         mysql(new MYSQL),
         mysqlres(0)
       {};

      ~CMySQL(void);

      void dbConnect(void);
      void dbDisconnect(void);

      void dbSelectDB(AISutil::String const &dbName);

      int dbQuery(AISutil::String const &query);

      AISutil::String dbGetValue(void);
      AISutil::String dbGetValue(int field);

      void dbGetRow(void);

      void dbClearRes(void);

      void dbLock(AISutil::String const &table);
      void dbUnlock(void);

      // Commit and roolback (transaction mode) not implemented in MySQL
      void dbBeginTrans(void) {}
      void dbCommit(void) {}
      void dbRollback(void) {}


    private:
      MYSQL* const mysql;
      MYSQL_RES *mysqlres;
      MYSQL_ROW mysqlrow;

  }; // class CMySQL

}; // namespace Exordium

#endif // _SOURCE_LIB_DATABASE_MYSQL_DBMYSQL_H_
