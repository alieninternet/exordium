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

#ifndef _SOURCE_LIB_DATABASE_MYSQL_DBMYSQLRES_H_
#define _SOURCE_LIB_DATABASE_MYSQL_DBMYSQLRES_H_ 1

#include <exordium/database/base.h>
#include <exordium/database/result.h>

#include <aisutil/string/string.h>

#include <mysql/mysql.h>

#include <vector>

#include "dbmysql.h"

namespace Exordium {
   class CMySQLRes : public CResult {
    private:
      MYSQL_RES mysqlres;
      MYSQL_ROW mysqlrow;      
      MYSQL *mysql;
      std::vector < std::vector < std::string > > rows;
      int numrows,numfields;
      int results; // Size of our result set..
      int seekpos; // Position we are at..
    public:
      CMySQLRes(MYSQL_RES ptr);
      
      ~CMySQLRes()
	{};
      
      // Defintions
      AIS::UTil::String getValue(const int &,const int &);
      
      //functions.
      int getRows(void) 
	{ return numrows; };
	   
   };
};

#endif
