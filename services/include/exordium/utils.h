/* utils.h
 * Utilities
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _INCLUDE_KINEIRCD_UTILS_H_
# define _INCLUDE_KINEIRCD_UTILS_H_

# include <cstdlib>
# include <netinet/in.h>

# include "kineircd/str.h"

// Macros
# define octStr(x)	baseXStr(x, 8)		// Convert decimal to octal
# define hexStr(x)	baseXStr(x, 16)		// Convert decimal to hex


// Utility class
class Utils {
 public:
   // Type of a "base"
   typedef unsigned char base_type;
   
 private:
   Utils(void) {};				// Constructor (cannot be run)
   
 public:
   // utils.cpp
   static String baseXStr(unsigned long, 
			  const base_type);	// Convert to another base
   static bool toBool(const String &,
		      const bool);		// Convert a string to boolean
   static StringMask 
     fixToIdentityMask(const String &);		// Fix a mask into an id mask

   // sha1.cpp
   class SHA1 {
    public:
      // A digest. SHA1 returns 160 bits, this makes it easier to manage.
      union digest_type {
	 unsigned char c[20];
	 unsigned long l[5];
	 
	 // Boolean equals operator
	 bool operator==(const digest_type &d) const {
	    return ((l[0] == d.l[0]) &&
		    (l[1] == d.l[1]) &&
		    (l[2] == d.l[2]) &&
		    (l[3] == d.l[3]) &&
		    (l[4] == d.l[4]) &&
		    (l[5] == d.l[5]));
	 };
      };

      // An empty digest
      static const digest_type nullDigest;
      
      static digest_type generate(const 
				  String &);	// SHA1 digest from a string
      
      // Convert an SHA1 digest output to particular base
      static String digestToStr(const digest_type &, const base_type,
				const String::size_type);
   };
};


#endif

