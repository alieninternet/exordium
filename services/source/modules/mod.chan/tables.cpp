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


// The 'chanaccess' table fields
static const Exordium::DatabaseTable::fields_type chanaccessFields = {
     {
	"chanid",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"nickid",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"access",
	  DatabaseTable::Field::Type::UnsignedInteger16,
	  3,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'chanaccess' table itself
static const Exordium::DatabaseTable chanaccessTable = {
   // The name of the table
   "chanaccess",
   
   // The table's fields..
   chanaccessFields
};



// The 'chanbans' table fields
static const Exordium::DatabaseTable::fields_type chanbansFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"chan",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"mask",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  200,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"setby",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  32,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"seton",
	  DatabaseTable::Field::Type::Timestamp,
	  14,
	  0,
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"seton",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  20,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"reason",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'chanbans' table itself
static const Exordium::DatabaseTable chanbansTable = {
   // The name of the table
   "chanbans",
   
   // The table's fields..
   chanbansFields
};



// The 'chanlogs' table fields
static const Exordium::DatabaseTable::fields_type chanlogsFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"name",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  200,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"llog",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  20,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'chanlogs' table itself
static const Exordium::DatabaseTable chanlogsTable = {
   // The name of the table
   "chanlogs",
   
   // The table's fields..
   chanlogsFields
};



// The 'chanopts' table fields
static const Exordium::DatabaseTable::fields_type chanoptsFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"name",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"clog",
	  DatabaseTable::Field::Type::Boolean,
	  1,
	  0,
	  0
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'chanopts' table itself
static const Exordium::DatabaseTable chanoptsTable = {
   // The name of the table
   "chanopts",
   
   // The table's fields..
   chanoptsFields
};



// The 'chans' table fields
static const Exordium::DatabaseTable::fields_type chansFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"name",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"owner",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  32,
	  0,
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"topic",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  0,
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"modes",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  200,
	  0,
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"cdesc",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  0,
	  0
     },
     {
	"url",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  200,
	  0,
	  0
     },
     {
	"clog",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  20,
	  0,
	  0
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'chans' table itself
static const Exordium::DatabaseTable chansTable = {
   // The name of the table
   "chans",
   
   // The table's fields..
   chansFields
};



// The 'chanstatus' table fields
static const Exordium::DatabaseTable::fields_type chanstatusFields = {
     {
	"chanid",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"nickid",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"status",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'chanstatus' table itself
static const Exordium::DatabaseTable chanstatusTable = {
   // The name of the table
   "chanstatus",
   
   // The table's fields..
   chanstatusFields
};



// A table of tables, hehe :)
const Exordium::DatabaseTable* const Exordium::ChanModule::Tables::tables[] = {
   &chanaccessTable,
   &chanbansTable,
   &chanlogsTable,
   &chanoptsTable,
   &chansTable,
   &chanstatusTable,
   0
};
