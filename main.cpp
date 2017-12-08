#include "parser.h"
#include "peg.h"

#include <iostream>

int main()
{
  Grammar g{"wefi",
    Rule{"wefi", Empty{}},
    Rule{"wie", Terminal{"i"}},
    Rule{"weri", Choice{Empty{}, Empty{}}},
    Rule{"fijoe", Sequence{Terminal{"iof"}, NonTerminal{"weio"}}}
  };

  std::cout << g.repr();

  Parser p{g};
  p.parse("test string", Terminal{"tes"});
}
