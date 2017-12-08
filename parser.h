#ifndef PARSER_H
#define PARSER_H

#include "peg.h"
#include "tuple.h"

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
  Result parse(string_view sv, NonTerminal nt);

  template <class L, class R>
  Result parse(string_view sv, Choice<L, R> c);

  template <class L, class R>
  Result parse(string_view sv, Sequence<L, R> s);

  template <class E>
  Result parse(string_view sv, ZeroOrMore<E> z);

  template <class E>
  Result parse(string_view sv, OneOrMore<E> o);

  template <class E>
  Result parse(string_view sv, Optional<E> o);

  template <class E>
  Result parse(string_view sv, And<E> a);

  template <class E>
  Result parse(string_view sv, Not<E> n);
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

template <class... Rs>
Result Parser<Rs...>::parse(string_view sv, NonTerminal nt)
{
  auto res = Result::Fail();

  for_each_in_tuple(grammar.rules, [&](auto&& r) {
    if(r.lhs == nt.symbol) {
      res = parse(sv, r.expr);
    }
  });

  return res;
}

template <class ... Rs>
template <class L, class R>
Result Parser<Rs...>::parse(string_view sv, Choice<L, R> c)
{
  auto left_result = parse(sv, c.left);
  if(left_result.type == ResultType::Success) {
    return left_result;
  } else {
    return parse(sv, c.right);
  }
}

template <class ... Rs>
template <class L, class R>
Result Parser<Rs...>::parse(string_view sv, Sequence<L, R> s)
{
  auto left_result = parse(sv, s.left);
  if(left_result.type == ResultType::Failure) {
    return Result::Fail();
  }

  auto right_result = parse(sv.substr(left_result.length), s.right);
  if(right_result.type == ResultType::Failure) {
    return Result::Fail();
  }

  return Result::OK(left_result.length + right_result.length);
}

template <class ... Rs>
template <class E>
Result Parser<Rs...>::parse(string_view sv, ZeroOrMore<E> z)
{
  auto res = Result::OK(0);
  auto rep = parse(sv, z.expr);

  while(rep.type != ResultType::Failure) {
    res.length += rep.length;
    rep = parse(sv.substr(res.length), z.expr);
  }

  return res;
}

template <class ... Rs>
template <class E>
Result Parser<Rs...>::parse(string_view sv, OneOrMore<E> o)
{
  auto res = Result::OK(0);
  auto rep = parse(sv, o.expr);

  if(rep.type == ResultType::Failure) {
    return Result::Fail();
  }

  while(rep.type != ResultType::Failure) {
    res.length += rep.length;
    rep = parse(sv.substr(res.length), o.expr);
  }

  return res;
}

template <class ... Rs>
template <class E>
Result Parser<Rs...>::parse(string_view sv, Optional<E> o)
{
  auto res = parse(sv, o.expr);

  if(res.type == ResultType::Failure) {
    return Result::OK(0);
  }

  return res;
}

template <class ... Rs>
template <class E>
Result Parser<Rs...>::parse(string_view sv, And<E> a)
{
  auto res = parse(sv, a.expr);

  if(res.type == ResultType::Success) {
    return Result::OK(0);
  } else {
    return Result::Fail();
  }
}

template <class ... Rs>
template <class E>
Result Parser<Rs...>::parse(string_view sv, Not<E> a)
{
  auto res = parse(sv, a.expr);

  if(res.type == ResultType::Failure) {
    return Result::OK(0);
  } else {
    return Result::Fail();
  }
}

#endif
