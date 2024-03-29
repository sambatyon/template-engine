#include "token.hh"

namespace yate {

Token::Token() : Token(Token::Tag::eEOF, "", 0, 0) {}

Token::Token(
    Token::Tag tag,
    std::string value,
    std::uint32_t line,
    std::uint16_t col)
    : tag_(tag), value_(std::move(value)), line_(line), column_(col) {}

Token::Token(const Token &other)
    : tag_(other.tag_),
      value_(other.value_),
      line_(other.line_),
      column_(other.column_) {}

Token::Token(Token &&other)
    : tag_(other.tag_),
      value_(std::move(other.value_)),
      line_(other.line_),
      column_(other.column_) {}

Token &Token::operator=(const Token&other) {
  if (this == &other) {
    return *this;
  }
  tag_ = other.tag_;
  value_ = other.value_;
  line_ = other.line_;
  column_ = other.column_;
  return *this;
}

std::string to_string(Token::Tag tag) {
  switch (tag) {
    case Token::Tag::eEOF:
      return "EOF";
    case Token::Tag::eNoOp:
      return "LITERAL";
    case Token::Tag::eLoopBegin:
      return "LOOP_BEGIN";
    case Token::Tag::eLoopEnd:
      return "LOOP_END";
    case Token::Tag::eIdentifier:
      return "IDENTIFIER";
    case Token::Tag::eScriptBegin:
      return "SCRIPT_BEGIN";
    case Token::Tag::eScriptEnd:
      return "SCRIPT_END";
  }
  return "";
}

std::ostream &operator<<(std::ostream &stream, Token::Tag tag) {
  return stream << to_string(tag);
}

} // namespace yate
