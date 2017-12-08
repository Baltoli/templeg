#include "parser.h"
#include "peg.h"

#include <iostream>

int main()
{
  Grammar g{"start",
    Rule{"start", Not{NonTerminal{"symbol"}}},
    Rule{"symbol", Sequence{NonTerminal{"value"}, Terminal{"!"}}},
    Rule{"value",
      Choice{Terminal{"woo"}, Terminal{"the"}}
    }
  };

  std::cout << g.repr();

  Parser p{g};
  std::cout << p.parse("th!", NonTerminal{"start"}).repr() << '\n';
}
