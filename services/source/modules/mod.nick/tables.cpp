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


// The 'nicks' table fields
static const Exordium::DatabaseTable::fields_type nicksFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"nickname",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  32,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"password",
	  DatabaseTable::Field::Type::FixedLengthBinaryData,
	  20,
	  0,
	  0
     },
     {
	"email",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"registered",
	  DatabaseTable::Field::Type::Timestamp,
	  14,
	  0,
	  0
     },
     {
	"lastid",
	  DatabaseTable::Field::Type::Timestamp,
	  14,
	  0,
	  0
     },
     {
	"lasthost",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  255,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"boolean",
	  DatabaseTable::Field::Type::Boolean,
	  1,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"lang",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  20,
	  "en", // this should not be fixed, and should correlate with autoconf
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"icq",
	  DatabaseTable::Field::Type::UnsignedInteger64,
	  20,
	  "0",
	  0
     },
     {
	"msn",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  0,
	  0
     },
     {
	"url",
	  DatabaseTable::Field::Type::VariableLengthString16,
	  400,
	  0,
	  0
     },
     {
	"deopaway",
	  DatabaseTable::Field::Type::Boolean,
	  1,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"yahoo",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  64,
	  0,
	  0
     },
     {
	"aim",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  64,
	  0,
	  0
     },
     {
	"quitmsg",
	  DatabaseTable::Field::Type::VariableLengthString16,
	  512,
	  0,
	  0
     },
     {
	"modnick",
	  DatabaseTable::Field::Type::Boolean,
	  1,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'nicks' table itself
static const Exordium::DatabaseTable nicksTable = {
   // The name of the table
   "nicks",
   
   // The table's fields..
   nicksFields
};



// The 'nicksidentified' table fields
static const Exordium::DatabaseTable::fields_type nicksidentifiedFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"nick",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"idas",
	  DatabaseTable::Field::Type::UnsignedInteger32,
	  11,
	  "0",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'nicksidentified' table itself
static const Exordium::DatabaseTable nicksidentifiedTable = {
   // The name of the table
   "nicksidentified",
   
   // The table's fields..
   nicksidentifiedFields
};



// The 'nickspending' table fields
static const Exordium::DatabaseTable::fields_type nickspendingFields = {
     {
	"id",
	  DatabaseTable::Field::Type::UniqueSequence32,
	  11,
	  0,
	  DatabaseTable::Field::Flags::PrimaryKey |
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"nickname",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  32,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     {
	"auth",
	  DatabaseTable::Field::Type::VariableLengthString8,
	  250,
	  "",
	  DatabaseTable::Field::Flags::NotNull
     },
     { 0, DatabaseTable::Field::Type::Void, 0, 0, 0 } // <=- Terminator
};

// The 'nickspending' table itself
static const Exordium::DatabaseTable nickspendingTable = {
   // The name of the table
   "nickspending",
   
   // The table's fields..
   nickspendingFields
};



// A table of tables, hehe :)
const Exordium::DatabaseTable* const Exordium::NickModule::Tables::tables[] = {
   &nicksTable,
   &nicksidentifiedTable,
   &nickspendingTable,
   0
};
