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



#ifndef _INCLUDE_EXORDIUM_DATABASE_BASE_H_
# define _INCLUDE_EXORDIUM_DATABASE_BASE_H_ 1

#include <aisutil/string/string.h>

#include "result.h"

namespace Exordium {

   class Config;
	
  /*
   * Base class for all SQL servers
   *
   *
   */
  class CBase
  { 
    public:
      CBase(Config& c)
       : connected(false),
         config(c)
       {};

      // Virtual destructor
      virtual ~CBase(void) {}

      virtual void dbConnect(void)=0;
      virtual void dbDisconnect(void)=0;

      virtual void dbBeginTrans(void)=0;
      virtual void dbCommit(void)=0;
      virtual void dbRollback(void)=0;

      virtual void dbSelectDB(AISutil::String const &dbName)=0;

      virtual int dbQuery(AISutil::String const &query)=0;

      virtual AISutil::String dbGetValue(void)=0;
      virtual AISutil::String dbGetValue(int field)=0;

      virtual void dbGetRow(void)=0;

      virtual void dbClearRes(void)=0;

      virtual void dbLock(AISutil::String const &table)=0;
      virtual void dbUnlock(void)=0;
      virtual CResult* dbGetResultSet(void)=0;



      // Are we connected?
      bool isConnected(void) const
       { return connected; };

    protected:
      bool connected;
      Config &config;

  }; // class CBase

}; // namespace Exordium

# include <exordium/config.h>


#endif // _INCLUDE_EXORDIUM_DATABASE_BASE_H_
