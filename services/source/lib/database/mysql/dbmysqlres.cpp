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
using AIS::Util::String;

// Constructor thingy ma bob 
CMySQLRes::CMySQLRes(MYSQL_RES ptr)
{
  memcpy( &mysqlres, &ptr, sizeof(ptr) );
  mysql_data_seek(&mysqlres,0); // Seek back to 0..
  results = mysql_num_rows(&mysqlres);
  unsigned int num_fields,i,colnum=0,rownum=0;
  num_fields = mysql_num_fields(&mysqlres);
  while ((mysqlrow = mysql_fetch_row(&mysqlres)))
     {
	unsigned long *lengths;
	lengths = mysql_fetch_lengths(&mysqlres);
	numfields=(int)lengths;
	for(i=0; i < num_fields; i++)
	  {
	     rows.push_back(std::vector <std::string>());
	     rows[rownum].push_back(mysqlrow[i]);
	  }
	rownum++;
     }
   numrows=rownum;
   seekpos = 0;
}

String CMySQLRes::getValue(const int &row,const int &col)
{
   std::cout << "Am trying to retrieve rows[" << row << "][" << col << "]" << std::endl;
   if(row>numrows)
     {
	std::cout << "getValue(): Error! Trying to seek past rows - Only " << numrows << " available" << std::endl;
	return "";
     }
   if(col>numfields)
     {
	std::cout << "getValue(): Error! Trying to seek past cols - Only " << numfields << " available" << std::endl;
	return "";
     }
   return rows[row][col];
}
