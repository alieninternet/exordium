/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 Exordium Development Team
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
 * For contact details for the Exordium Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _INCLUDE_EXORDIUM_MODULE_H_
# define _INCLUDE_EXORDIUM_MODULE_H_ 1

# include <aisutil/config.h>
# include <kineircd/clientname.h>

// Definitions for the module init functions..
#define EXORDIUM_MODULE_INIT_FUNCTION_NO_EXTERN(x) \
   Exordium::Module* x(void)

#define EXORDIUM_MODULE_INIT_FUNCTION \
   extern "C" EXORDIUM_MODULE_INIT_FUNCTION_NO_EXTERN(exordium_module_init)

namespace Exordium {
   class Service;
   class Services;
   
   class Module {
    public:
      struct Info {
	 // Version information about the module
	 const char* const shortName;
	 const char* const fullName;
	 const unsigned short versionMajor;
	 const unsigned short versionMinor;
      };
      
      class ConfigData : public AISutil::ConfigData {
       private:
	 // Configuration information (this is a default list for this base)
	 static const AISutil::ConfigParser::defTable_type defaultDefinitions;
	 
       protected:
	 AISutil::String defDescription;		// Our description
	 AISutil::String defDistribution;		// Our scope mask
	 AISutil::String defHostname;			// Our hostname
	 Kine::ClientName defName;			// Our name
	 AISutil::String defIdent;			// Out ident
	 
       public:
	 // Constructor
	 ConfigData(const AISutil::String& d, const AISutil::String& h,
		    const AISutil::String& n, const AISutil::String& u)
	   : defDescription(d),
	     defDistribution("*"), // <=- network-wide distribution scope
	     defHostname(h),
	     defName(n),
	     defIdent(u)
	   {};

	 // Destructor
	 virtual ~ConfigData(void)
	   {};

	 // Grab the configuration definition table
	 virtual const AISutil::ConfigParser::defTable_type&
	   getDefinitions(void) const
	   { return defaultDefinitions; };
	 
	 // Return variables..
	 const AISutil::String& getDescription(void) const
	   { return defDescription; };
	 const AISutil::String& getDistribution(void) const
	   { return defDistribution; };
	 const AISutil::String& getHostname(void) const
	   { return defHostname; };
	 const Kine::Name& getName(void) const
	   { return defName; };
	 const AISutil::String& getIdent(void) const
           { return defIdent; };
      };
      
    protected:
      // Constructor
      Module(void)
	{};
      
    public:
      // Destructor
      virtual ~Module()
	{};

      // Start the module (return false if the module was unable to start)
      virtual bool start(Exordium::Services& s) = 0;
      
      // Stop the module (called just before a module is unloaded)
      virtual void stop(const AISutil::String* const reason = 0) {};

      // Grab the information structure of a module
      virtual const Info& getModuleInfo(void) const = 0;
      
      // Return an appropriate instance of a configuration data class
      virtual const ConfigData& getConfigData(void) const = 0;
      virtual ConfigData& getConfigData(void) = 0;
      
      // Return the service this module has wrapped up inside it (0 = not init)
      virtual Service* const getService(void) const = 0;
   };
};

# include <exordium/service.h>
# include <exordium/services.h>

#endif // _INCLUDE_EXORDIUM_MODULE_H_

