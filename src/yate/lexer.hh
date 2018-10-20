#pragma once

#include <cstdint>
#include <iosfwd>
#include <istream>

#include "token.hh"

namespace yate {

class StreamPos;

class Lexer {
 public:
  Lexer(std::istream &istream);
  ~Lexer() {}

  Token Scan();

  StreamPos CurrentStreamPos() const;
  void SetStreamPos(StreamPos pos);

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

class StreamPos {
 public:
  StreamPos();
  StreamPos(std::streampos pos, std::uint32_t line, std::uint32_t col);
  StreamPos(const StreamPos &other);
  StreamPos(StreamPos &&other);
  StreamPos &operator=(const StreamPos &other);
  ~StreamPos() {}

 private:
  friend class Lexer;
  std::streampos position_;
  std::uint32_t line_;
  std::uint32_t column_;
};

} // yate
