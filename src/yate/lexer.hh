#pragma once

#include <iosfwd>
#include <istream>

#include "token.hh"

namespace yate {

class Lexer {
 public:
  Lexer(std::istream &istream);
  ~Lexer() {}

  Token Scan();

 private:
  std::istream::char_type ReadChar();

  std::istream &istream_;
  std::istream::char_type current_;
  bool script_mode_;
};

} // yate
