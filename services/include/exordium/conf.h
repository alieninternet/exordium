/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef _CONF_H_
#define _CONF_H_

#include <kineircd/configdata.h>

using Kine::String;

namespace Exordium {

   class Config : public Kine::ConfigData {
    public:
      // The definition table used by Kine's configuration parser
      static const Kine::ConfigParser::defTable_type definitionTable;

    private:
      // Variables (top class from the definition table above)
      String defLogfile;			// Log filename
      String defUplink;				// Uplink Host
      
      // 'SQL' class and variables
      static const Kine::ConfigParser::defTable_type defClassSql;
      String defSqlDatabase;			// MySql server database name
      String defSqlHostname;			// MySql server hostname
      String defSqlPassword;			// MySql server password
      unsigned short defSqlPort;		// MySql server port
      String defSqlUsername;			// MySql server username
      
    public:
      // Constructor (sets up defaults mainly)
      Config(void);
      
      // Top variables
      const String &getLogfile(void) const
	{ return defLogfile; };
      const String &getUplink(void) const
	{ return defUplink; };      
      // 'SQL' class
      const String &getSqlDatabase(void) const
	{ return defSqlDatabase; };
      const String &getSqlHostname(void) const
	{ return defSqlHostname; };
      const String &getSqlPassword(void) const
	{ return defSqlPassword; };
      const unsigned short getSqlPort(void) const
	{ return defSqlPort; };
      const String &getSqlUsername(void) const
	{ return defSqlUsername; };
   };
}; // namespace Exordium

#endif

