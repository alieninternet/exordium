/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef SQL_H
#define SQL_H

#include <iostream>
#include "exordium/conf.h"
#include "mysql.h"
#include <kineircd/str.h>

using Kine::String;

namespace Exordium {

class Sql
{
private: 
	static int sock;
public:
	static void init(const Config &);
	static MysqlRes query(String const &);
	static String makeSafe(String const &);
};


};
#endif

