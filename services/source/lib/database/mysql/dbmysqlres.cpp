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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#ifndef HAVE_MYSQL
# error "Cannot compile mysql support without mysql!"
#endif

#include "dbmysql.h"
#include "dbmysqlres.h"

#include <mysql/mysql.h>

using namespace Exordium;
using AISutil::String;

// Constructor thingy ma bob 
CMySQLRes::CMySQLRes(MYSQL_RES ptr)
{
//  mysqlres = ptr;
  memcpy( &mysqlres, &ptr, sizeof(ptr) );
  mysql_data_seek(&mysqlres,0); // Seek back to 0..
  results = mysql_num_rows(&mysqlres)-1;
  std::cout << "We have " << results << " results " << std::endl;
  seekpos = 0;
  nextRow();

}

void CMySQLRes::nextRow()
{
   std::cout << "nextRow() we are at " << seekpos << " out of " << results << std::endl;
if(seekpos==results)
     {
	std::cout << "nextrow() trying to seek past results!" << std::endl;
	return;
     }
   else
     {
	if(&mysqlres!=NULL)
	  mysqlrow = mysql_fetch_row(&mysqlres); 
     }
   
if(mysqlrow==NULL)
     {
	std::cout << "nextRow() returned no data!! (or an error!)" << std::endl;
	return;
     }
   else
     {
	seekpos++;
	
     }
   
}


String CMySQLRes::getValue()
{
   if(&mysqlres!=NULL)
     {
	return mysqlrow[0];
     }
   
   else
     return "";
}
