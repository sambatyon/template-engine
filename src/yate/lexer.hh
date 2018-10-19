#pragma once

#include <cstdint>
#include <iosfwd>
#include <istream>

#include "token.hh"

namespace yate {

class Lexer {
 public:
  Lexer(std::istream &istream);
  ~Lexer() {}

  Token Scan();

  std::streampos CurrentStreamPos() const;
  void SetStreamPos(std::streampos pos);

 private:
  std::istream::char_type ReadChar();
  std::istream::char_type ReadCompare(std::istream::char_type ch);

  std::string GenerateError(const std::string &message);

  Token ScanLiterate();
  Token ScanScript();

  std::istream &istream_;
  std::istream::char_type current_;
  bool script_mode_;
  bool initialized_; // TODO: Find a more elegant solution to this.
  std::uint64_t id_generator_;
  bool must_return_script_begin_;
  std::uint32_t line_;
  std::uint32_t column_;
};

} // yate
