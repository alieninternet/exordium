/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __NOTE_H_
#define __NOTE_H_

#include <kineircd/str.h>
# define NOTE_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"
#include "exordium/services.h"

namespace Exordium {

class Note : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
     void NOTE_FUNC ((Note::* const function));
  };
  static struct functionTableStruct const functionTable[];
   Exordium::Services& services;
   
public:
   Note(Exordium::Services& s)
   : Service(), services(s)
   {
	std::cout << "Hi im an instance. :(" << std::endl;
   };
  ~Note(void) 
   {
	std::cout << "YOU BASTARD, YOU KILLED NOTE" << std::endl;
   };   
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
  void NOTE_FUNC (parseSEND);
  void NOTE_FUNC (parseLIST);
  void NOTE_FUNC (parseREAD);
  void NOTE_FUNC (parseDEL);
};
















};
#endif
