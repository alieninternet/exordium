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



#ifndef _INCLUDE_EXORDIUM_DATABASE_BASE_H_
# define _INCLUDE_EXORDIUM_DATABASE_BASE_H_ 1

#include <kineircd/str.h>


namespace Exordium {

  /*
   * Base class for all SQL servers
   *
   *
   */
  class SqlBase
  { 
    public:
      SqlBase()
       : connected(false)
       {};

      // Virtual destructor
      virtual ~SqlBase(void) { isEOF=true; }

      virtual void dbConnect(void)=0;
      virtual void dbDisconnect(void)=0;

      virtual void dbBeginTrans(void)=0;
      virtual void dbCommit(void)=0;
      virtual void dbRollback(void)=0;

      virtual void dbSelectDB(AISutil::String const &dbName)=0;

      virtual void dbSelect(AISutil::String const &fields, AISutil::String const &table)=0;
      virtual void dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs)=0;
      virtual void dbSelect(AISutil::String const &fields, AISutil::String const &table, AISutil::String const &whereargs, AISutil::String const &orderargs)=0;

      virtual void dbCount(AISutil::String const &table)=0;
      virtual void dbCount(AISutil::String const &table, AISutil::String const &whereargs)=0;

      virtual void dbInsert(AISutil::String const &table,  AISutil::String const &values)=0;

      virtual void dbUpdate(AISutil::String const &table, AISutil::String const &values, AISutil::String const &whereargs)=0;

      virtual void dbDelete(AISutil::String const &table, AISutil::String const &whereargs)=0;
      virtual void dbDelete(AISutil::String const &table)=0;

      virtual void dbQuery(AISutil::String const &query)=0;

      virtual AISutil::String dbGetValue(void)=0;
      virtual AISutil::String dbGetValue(int field)=0;

      virtual void dbGetRow(void)=0;

      virtual void dbClearRes(void)=0;

      virtual void dbLock(AISutil::String const &table)=0;
      virtual void dbUnlock(void)=0;

      virtual bool eof(void)=0;
      virtual int dbResults(void)=0;
      virtual int dbNbCols(void)=0;

      virtual void getFieldNames(AISutil::String const &table)=0;

      virtual int affectedRows(void);

      // Are we connected?
      bool isConnected(void) const
       { return connected; };

    protected:
      bool connected;
      bool isEOF;

  }; // class SqlBase

}; // namespace Exordium



#endif // _INCLUDE_EXORDIUM_DATABASE_BASE_H_
