#ifndef PARSER_H
#define PARSER_H

#include "peg.h"

#include <string>
#include <experimental/string_view>

using std::experimental::string_view;

enum class ResultType {
  Success,
  Failure
};

struct Result {
  ResultType type;
  size_t length;

  static Result OK(size_t length) { return {ResultType::Success, length}; }
  static Result Fail() { return {ResultType::Failure, 0}; }

  std::string repr() const;
};

template <class ... Rs>
struct Parser {
  Grammar<Rs...> grammar;

  Result parse(string_view sv, Empty);
  Result parse(string_view sv, Terminal t);
};
template <class ... Rs> Parser(Grammar<Rs...>) -> Parser<Rs...>;

std::string Result::repr() const
{
  switch(type) {
    case(ResultType::Success):
      return "Success(" + std::to_string(length) + ")";
    case(ResultType::Failure):
      return "Failure";
  }
}

template <class ... Rs>
Result Parser<Rs...>::parse(string_view sv, Empty)
{
  return Result::OK(0);
}

template <class ... Rs>
Result Parser<Rs...>::parse(string_view sv, Terminal t)
{
  if(sv.length() < t.symbol.length()) { 
    return Result::Fail();
  }

  for(size_t i = 0; i < t.symbol.length(); ++i) {
    if(sv[i] != t.symbol[i]) {
      return Result::Fail();
    }
  }

  return Result::OK(t.symbol.length());
}

#endif
