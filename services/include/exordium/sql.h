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
#include "exordium/services.h"
#include "exordium/log.h"
#include "exordium/mysql.h" // was "mysql.h" -- did I change this in error?
#include <kineircd/str.h>

using Kine::String;

namespace Exordium {
   class Sql {
    private: 
      int sock;
      Services& services;
      Log& logger;
      
    public:
      Sql(Services& s, Log& l, const Config& c);
      
      MysqlRes query(const String&);
      String makeSafe(const String&);
   };
};

#endif

