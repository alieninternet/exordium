/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __MODULE_H
#define __MODULE_H

#include "service.h"
#include <kineircd/str.h>

namespace Exordium {


class Module {
 public:
   const char *modName;

   Service *modService;

   Module(const char *n, Service *s)
     : modName(n), modService(s)
     {};
   ~Module()
	{
		cout << "Module terminating " << *modName << endl;
	};

};

};

#endif

