/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef __IRCDOME_H_
#define __IRCDOME_H_

#include "kineircd/str.h"

# define IRCDOME_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;
using namespace Exordium;

class IRCDome
{
private:
  struct functionTableStruct
  {
    char const *command;
    void IRCDOME_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];
  static void NICK_FUNC (parseMODULE);
public: 
  static void parseLine( String &, String const &);
};


















#endif
