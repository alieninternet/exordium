#ifndef __NOTE_H_
#define __NOTE_H_

#include "kineircd/str.h"
# define NOTE_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {

class Note : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void NOTE_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];

public:
  Note(void)
   : Service()
   {
	std::cout << "Hi im an instance. :(" << std::endl;
   };
  ~Note(void) 
   {
	std::cout << "YOU BASTARD, YOU KILLED NOTE" << std::endl;
   };   
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
 static void NOTE_FUNC (parseSEND);
 static void NOTE_FUNC (parseLIST);
 static void NOTE_FUNC (parseREAD);
 static void NOTE_FUNC (parseDEL);
};
















};
#endif
