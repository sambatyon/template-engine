#include "token.hh"

namespace yate {

Token::Token(Token::Tag tag, std::string value)
  : tag_(tag), value_(value) {}

Token::Token(const Token &other) : tag_(other.tag_), value_(other.value_) {}

Token::Token(Token &&other)
  : tag_(std::move(other.tag_)), value_(std::move(other.value_)) {}

Token &Token::operator=(const Token&other) {
  if (this == &other) {
    return *this;
  }
  tag_ = other.tag_;
  value_ = other.value_;
  return *this;
}

} // namespace yate
