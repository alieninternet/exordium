/* $Id$
 * 
 * Exordium Network Services
 * Copyright (C) 2002,2003 IRCDome Development Team
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
 * For contact details for the IRCDome Development Team please see the
 * enclosed README file.
 *
 */

#ifndef _SOURCE_VERSION_H_
# define _SOURCE_VERSION_H_ 1

// Short name of the exordium module, this must be one word
# define EXORDIUM_NAME_SHORT	"exordium"

// Long name of the module..
# define EXORDIUM_NAME_LONG	"Exordium IRC Network Services"

// Breif copyright information (one line)
# define EXORDIUM_COPYRIGHT	"Copyright (c) 2002,2003 Exordium Development Team"

// Version numbers (major.minor.patch). Patchlevel is used if it is > 0
# define EXORDIUM_VER_MAJOR	0
# define EXORDIUM_VER_MINOR	0
# define EXORDIUM_VER_PATCH	0

// Extra version information. Set to 0 rather than a string if none is needed
# define EXORDIUM_VER_EXTRA	".prealpha"
//# define EXORDIUM_VER_EXTRA	0

/* Long version information. This is a zero-terminated array of lines to be
 * appended to the list for /INFO and similar commands. Lines MUST be 60
 * visible characters long or shorter. A visible character is one that is not
 * used for formatting. Do not use control characters other than those used
 * in formatting IRC test. The character set here should be UTF-8, or otherwise
 * kept to 7-bit ASCII. A ruler is provided to help you keep within the 60
 * character limit..
 */
# define EXORDIUM_VER_INFO \
   { \
      /* Ruler: \
       *          1         2         3         4         5         6 \
       * 123456789012345678901234567890123456789012345678901234567890 */ \
      "Exordium Network Services, Copyright (C) 2002,2003 IRCDome ", \
      "Development Team", \
      "", \
      "Exordium comes with ABSOLUTELY NO WARRANTY; for details see", \
      "The enclosed LICENSE file.  This is free software", \
      "And you are welcome to redistribute it under certain", \
      "conditions; please see the enclosed LICENSE file", \
      0 /* <=- The terminator */ \
   }

#endif // _SOURCE_VERSION_H_
