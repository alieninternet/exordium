#ifndef __CHAN_H_
#define __CHAN_H_

#include "kineircd/str.h"

# define CHAN_FUNC(x)           x(Kine::String &origin, Kine::StringTokens &tokens, Kine::String &chan)

using Kine::String;

#include "exordium/service.h"

namespace Exordium {


class Chan : public Service
{
private:
  struct functionTableStruct
  {
    char const *command;
    void CHAN_FUNC ((*function));
  };
  static struct functionTableStruct const functionTable[];

public:
  Chan(void)
	: Service()
	{
		std::cout << "Chan :)" << std::endl;
	};
  ~Chan(void)
	{
		std::cout << "Dead Chan :(" << std::endl;
	};
  void parseLine (String const &, String const &);
  void parseLine (String const &, String const &, String const &);
private:
   static void CHAN_FUNC (parseHELP);
   static void CHAN_FUNC (parseACCESS);
   static void CHAN_FUNC (parseKICK);
   static void CHAN_FUNC (parseOP);
   static void CHAN_FUNC (parseVOICE);
   static void CHAN_FUNC (parseREGISTER);
   static void CHAN_FUNC (parseBAN);
   static void CHAN_FUNC (parseDEOP);
   static void CHAN_FUNC (parseTOPIC);
   static void CHAN_FUNC (parseDEVOICE);
   static void CHAN_FUNC (parseADDUSER);
   static void CHAN_FUNC (parseINFO);
};




};

#endif
