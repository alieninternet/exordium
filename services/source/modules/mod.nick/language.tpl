[+ AutoGen5 template cpp h +]
/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2003 Exordium Development Team
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
 *
[+(dne " * ")+]
 */
[+ CASE (suffix) +][+ == cpp +]

#include "language.h"

static Kine::Language::tagMap_type Exordium::NickModule::Lang::tagMap = {[+ FOR langtag +]
   { "[+
   (string-upcase
      (sprintf "%s%s"
         (get "tagPrefix")
	 (get "name")))
 +]" },[+ ENDFOR +]
   { 0 }
};
[+ == h +]
#ifndef _SOURCE_MODULES_NICK_LANGUAGE_H_
# define _SOURCE_MODULES_NICK_LANGUAGE_H_ 1

# include <kineircd/languages.h>

namespace Exordium {
   namespace NickModule {
      struct Lang { // <=- probably should be namespace too
         // Language tag look-up table (for our language map)
         enum {[+ FOR langtag "," +]
	    [+name+] = [+(for-index)+][+ ENDFOR +]	 
	 };
	 
	 // The language map
	 static Kine::Language::tagMap_type tagMap;
      }; // struct Language
   }; // namespce NickModule
}; // namespace Exordium

#endif // _SOURCE_MODULES_NICK_LANGUAGE_H_
[+ ESAC +]
