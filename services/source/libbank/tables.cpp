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

#include "tables.h"

using namespace Exordium;


// The 'bank' table fields
static const Exordium::DatabaseTable::fields_type bankFields = {
     {
	"nickid",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"balance",
	  DatabaseTable::Field::Type::UnsignedInteger64,
	  21,
	  "0",
	  0
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'bank' table itself
const Exordium::DatabaseTable Exordium::BankTables::bankTable = {
   // The name of the table
   "bank",
   
   // The table's fields..
   bankFields
};
