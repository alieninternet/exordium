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
      // Variables
      String defLogFile;			// Log filename
      String defMySqlDb;			// MySql server database
      String defMySqlHost;			// MySql server hostname
      String defMySqlPass;			// MySql server password
      unsigned short defMySqlPort;		// MySql server port
      String defMySqlUser;			// MySql server username
      String defUplinkHost;			// Server to connect to
      
    public:
      // Constructor (sets up defaults mainly)
      Config(void);
      
      // Variable grabbing thingies
      const String &getMySqlDb(void) const 
	{ return defMySqlDb; };
      const String &getMySqlHost(void) const
	{ return defMySqlHost; };
      const String &getMySqlPass(void) const
	{ return defMySqlPass; };
      const unsigned short getMySqlPort(void) const
	{ return defMySqlPort; };
      const String &getMySqlUser(void) const
	{ return defMySqlUser; };
   };
}; // namespace Exordium

#endif

