#ifndef __BOT_H_
#define __BOT_H_

#include "kineircd/str.h"

# define BOT_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {


class Bot : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void BOT_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];

public:
  Bot(void)
	: Service()
	{
		std::cout << "Bot" << std::endl;
	};
  ~Bot(void)
	{
		std::cout << "Dead Bot" << std::endl;
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   static void BOT_FUNC (parseHELP);
   static void BOT_FUNC (parseQUOTE);
};




};

#endif
