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
#include "exordium/log.h"
#include "exordium/mysql.h" // was "mysql.h" -- did I change this in error?
#include <kineircd/str.h>

using Kine::String;

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
      MysqlRes query(const String&);
      
      String makeSafe(const String&);
   };
};

#endif

