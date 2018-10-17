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
    eIdentifier = 6
  };

  explicit Token(Tag tag, std::string value);
  ~Token() {}

  Token(const Token &other);
  Token(Token &&other);
  Token &operator=(const Token&);

  Tag tag() const { return tag_; }
  std::string value() const { return value_; }

 private:
  Tag tag_;
  std::string value_;
};

inline
Token::Token(Token::Tag tag, std::string value)
  : tag_(tag), value_(value) {}

inline
Token::Token(const Token &other) : tag_(other.tag_), value_(other.value_) {}

inline
Token::Token(Token &&other)
  : tag_(std::move(other.tag_)), value_(std::move(other.value_)) {}

inline
Token &Token::operator=(const Token&other) {
  if (this == &other) {
    return *this;
  }
  tag_ = other.tag_;
  value_ = other.value_;
  return *this;
}

} // namespace yate
