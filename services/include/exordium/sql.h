/*
    SQL.H     - Database related header file.
    Version   - 0.1
    Date      - 8th May 2002
    Revisions -
  
    Copyright 2002 James Ian Wilkins <james@ircdome.org>

    This file is part of IRCDome's Services.

    IRCDome Services is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    IRCDome Services is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 
*/

#ifndef SQL_H
#define SQL_H

#include <iostream>
#include "mysql.h"
#include "kineircd/str.h"

using Kine::String;

namespace Exordium {

class Sql
{
private: 
	static int sock;
public:
	static void init(const String &, const String &, const String &, const String &);
	static MysqlRes query(String const &);
	static String makeSafe(String const &);
};


};
#endif

