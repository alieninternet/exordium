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

#ifndef _INCLUDE_EXORDIUM_SERVER_H_
# define _INCLUDE_EXORDIUM_SERVER_H_ 1

# include <aisutil/string.h>
# include <map>
# include <exordium/services.h>
# include <kineircd/name.h>

namespace Exordium
{
   class Server
     {
      private:
	AISutil::String name;
	int hops;
	AISutil::String desc;	
	Services& services;
      public:
      /* Inline functions here */
	const AISutil::String& getName(void) const
	  { return name; };
	
	const int getHops(void) const
	  { return hops; };
	
	const AISutil::String& getDesc(void) const
	  { return desc; };

      /* Our constructor definition */
	Server(AISutil::String const &sname, int const &shops, AISutil::String const &sdesc, Services &s)
	  : name(sname),
	hops(shops),
	desc(sdesc),
	services(s)
	  {};

	// Destructor
	~Server(void)
	  {};
	
	//functions.
	bool isAuthorised(void);
	
	void addOnlineServer(void);
	void delOnlineServer(void);

	// is equal to operator
	bool operator==(const Server &s) const
	  { return (hops == s.hops); };
     };
}; // namespace Exordium

#endif // _INCLUDE_EXORDIUM_SERVER_H_
