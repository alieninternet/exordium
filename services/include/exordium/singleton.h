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

#ifndef _INCLUDE_EXORDIUM_SINGLETON_H_
# define _INCLUDE_EXORDIUM_SINGLETON_H_ 1


namespace Exordium
{


  template<class T>
  class Singleton
  {
    public:
           static T *instance(void)
           {
             if(instancePtr!=NULL)
               return instancePtr;
             else
             {
               static T newinst;
               instancePtr=&newinst;
               return instancePtr;
             }
           }
             
    private:
           static T *instancePtr;
  };

  template<class T>
  T* Singleton<T>::instancePtr=NULL;

};

#endif // _INCLUDE_EXORDIUM_SINGLETON_H_
