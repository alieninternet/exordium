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

#ifndef SQL_H
#define SQL_H

#include <iostream>
#include "exordium/conf.h"
#include "exordium/log.h"
#include "exordium/mysql.h" // was "mysql.h" -- did I change this in error?
#include <kineircd/str.h>

namespace Exordium {
   class Sql {
    private: 
      int sock;
      Log& logger;
      
    public:
      Sql(Log& l, const Config& c);

      // These should probably be templated :( In fact, MySql:: should be
      // simply merged with this class altogether :(
      MysqlRes query(const char* const);
      MysqlRes query(const LibAIS::String&);

      // Escape a string so it is safe to be used in an SQL query
      static LibAIS::String makeSafe(const LibAIS::String& line);
   };
};

#endif

