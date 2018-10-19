#pragma once

#include <string>

namespace yate {

class Token {
 public:
  enum class Tag {
    eEOF = 0,
    eNoOp = 1,
    eLoopBegin = 4,
    eLoopEnd = 5,
    eIdentifier = 6,
    eScriptBegin = 7,
    eScriptEnd = 8
  };

  explicit Token(
      Tag tag,
      std::string value,
      std::uint32_t line,
      std::uint16_t col);
  ~Token() {}

  Token(const Token &other);
  Token(Token &&other);
  Token &operator=(const Token&);

  Tag tag() const { return tag_; }
  std::string value() const { return value_; }
  std::uint32_t line() const { return line_; }
  std::uint32_t column() const { return column_; }

 private:
  Tag tag_;
  std::string value_;
  std::uint32_t line_;
  std::uint32_t column_;
};

std::string to_string(Token::Tag tag);

} // namespace yate
