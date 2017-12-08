#ifndef PEG_H
#define PEG_H

#include <array>
#include <initializer_list>
#include <sstream>
#include <string>
#include <tuple>

struct Empty {
  std::string repr() const { return "ε"; }
};

struct Terminal {
  const std::string symbol;

  std::string repr() const { return "\"" + symbol + "\""; }
};

struct NonTerminal {
  const std::string symbol;

  std::string repr() const { return symbol; }
};

template <class L, class R>
struct Sequence {
  const L left;
  const R right;

  std::string repr() const { return left.repr() + " " + right.repr(); }
};
template <class L, class R> Sequence(L l, R r) -> Sequence<L, R>;

template <class L, class R>
struct Choice {
  const L left;
  const R right;

  std::string repr() const { return left.repr() + " | " + right.repr(); }
};
template <class L, class R> Choice(L l, R r) -> Choice<L, R>;

template <class E>
struct ZeroOrMore {
  const E expr;

  std::string repr() const { return expr.repr() + "*"; }
};
template <class E> ZeroOrMore(E e) -> ZeroOrMore<E>;

template <class E>
struct OneOrMore {
  const E expr;

  std::string repr() const { return expr.repr() + "+"; }
};
template <class E> OneOrMore(E e) -> OneOrMore<E>;

template <class E>
struct Optional {
  const E expr;

  std::string repr() const { return expr.repr() + "?"; }
};
template <class E> Optional(E e) -> Optional<E>;

template <class E>
struct And {
  const E expr;

  std::string repr() const { return "&" + expr.repr(); }
};
template <class E> And(E e) -> And<E>;

template <class E>
struct Not {
  const E expr;

  std::string repr() const { return "!" + expr.repr(); }
};
template <class E> Not(E e) -> Not<E>;

template <class E>
struct Rule {
  const std::string lhs;
  const E expr;

  std::string repr() const { return lhs + " -> " + expr.repr(); }
};
template <class E> Rule(std::string s, E e) -> Rule<E>;

template <class ... Rs>
struct Grammar {
  Grammar(std::string s, Rs... rules_) :
    start(s), rules(rules_...) {}

  const std::string start;
  const std::tuple<Rs...> rules;

  std::string repr() const;
};
template <class ... Rs> Grammar(std::string s, Rs... rules) -> Grammar<Rs...>;

template<class F, class...Ts, std::size_t...Is>
void for_each_in_tuple(const std::tuple<Ts...> & tuple, F func, std::index_sequence<Is...>){
    using expander = int[];
    (void)expander { 0, ((void)func(std::get<Is>(tuple)), 0)... };
}

template<class F, class...Ts>
void for_each_in_tuple(const std::tuple<Ts...> & tuple, F func){
    for_each_in_tuple(tuple, func, std::make_index_sequence<sizeof...(Ts)>());
}

template <class ... Rs>
std::string Grammar<Rs...>::repr() const
{
  std::stringstream ss;
  ss << "Start: " << start << '\n';
  ss << "Rules:\n";

  for_each_in_tuple(rules, [&](auto&& v) {
    ss << "  " << v.repr() << '\n';
  });

  return ss.str();
}

#endif
