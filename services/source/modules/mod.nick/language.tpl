[+ AutoGen5 template cpp h +][+
   ;;; Do we use the tag in the current scope?
   (define (useThisTag?)
      (or
         ;; If 'component' doesn't exist, it is a tag that applies to all mods
         (not
            (exist? "component"))
	    
	 ;; Check if 'component' is the one for us
         (string-ci=?
            (get "component")
            "mod_nick")))
   
+]
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

Kine::Languages::tagMap_type Exordium::NickModule::Language::tagMap = {[+ FOR langtag +][+ IF
   (useThisTag?) +]
   { "[+
   (string-upcase
      (sprintf "%s%s"
         (get "tagPrefix")
	 (get "name")))
 +]" },[+ ENDIF +][+ ENDFOR +]
   { 0 }
};
[+ == h +]
#ifndef _SOURCE_MODULES_NICK_LANGUAGE_H_
# define _SOURCE_MODULES_NICK_LANGUAGE_H_ 1

# include <kineircd/languages.h>

namespace Exordium {
   namespace NickModule {
      struct Language { // <=- probably should be namespace too
         // Language tag look-up table (for our language map)
         enum {[+ FOR langtag +][+ IF (useThisTag?) +]
	    [+name+] = [+(for-index)+][+ IF 
   (not
      (last-for?))
 +],[+ ENDIF +][+ ENDIF +][+ ENDFOR +]
	 };
	 
	 // The language map
	 static Kine::Languages::tagMap_type tagMap;
      }; // struct Language
   }; // namespace NickModule
}; // namespace Exordium


// Macro (*shock horror*) to make the code neater (hopefully)
# define GETLANG(n,...) \
   Kine::langs().get(origin.getLanguage(), \
                     Language::tagMap[Language::n].tagID, \
                     ##__VA_ARGS__)


#endif // _SOURCE_MODULES_NICK_LANGUAGE_H_
[+ ESAC +]
