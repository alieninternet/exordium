[+ AutoGen5 template lang +][+ FOR language +][+
   (out-push-new
      (sprintf "%s%s.exordium.lang" 
         (base-name)
	 (get "langCode")))
+]## $Id$
## [+langName+] language file for exordium
##
## Copyright © 2003 Exordium Development Team
##
## This file is a part of Exordium.
## 
## Exordium is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## Exordium is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with Exordium; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## For contact details for the Exordium Development Team please see the
## enclosed README file.
##
##
[+(dne "## ")+]
##
##


.LangCode = [+langCode+]
.LangName = [+langName+][+ IF (exist? "langNote") +]
.LangNote = [+langNote+][+ ENDIF +][+ IF (exist? "langMaintainer") +]
.Maintainer[+ ENDIF +]
.Revision = [+langRevision+]

[+ FOR langtag +][+name+] = [+
   (get 
      (sprintf "data_%s"
         (get "langCode")))
+]
[+ ENDFOR +][+ (out-pop) +][+ ENDFOR +]
