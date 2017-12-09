#include "parser.h"
#include "peg.h"

#include <iostream>

int main()
{
  Parser p{Grammar{"Expr",
    Rule{"Expr", NonTerminal{"Sum"}},
    Rule{"Sum", Sequence{
      NonTerminal{"Product"},
      ZeroOrMore{Sequence{
        Choice{Terminal{"+"}, Terminal{"-"}},
        NonTerminal{"Product"}
      }}
    }},
    Rule{"Product", Sequence{
      NonTerminal{"Value"},
      ZeroOrMore{Sequence{
        Choice{Terminal{"*"}, Terminal{"/"}},
        NonTerminal{"Value"}
      }}
    }},
    Rule{"Value", Choice{
      OneOrMore{NonTerminal{"Digit"}},
      Sequence{
        Terminal{"("}, Sequence{
        NonTerminal{"Expr"},
        Terminal{")"}
      }}
    }},
    Rule{"Digit", Choice{
      Terminal{"0"}, Choice{
      Terminal{"1"}, Choice{
      Terminal{"2"}, Choice{
      Terminal{"3"}, Choice{
      Terminal{"4"}, Choice{
      Terminal{"5"}, Choice{
      Terminal{"6"}, Choice{
      Terminal{"7"}, Choice{
      Terminal{"8"},
      Terminal{"9"}
    }}}}}}}}}}
  }};

  std::cout << p.grammar.repr();

  std::cout << p.parse("(1+2*3+((88)/44))-025").repr() << '\n';
}
