#pragma once

#include <string>

namespace yate {

class Token {
 public:
  enum class Tag {
    eNoOp = 0,
    eBeginTemplateMode = 1,
    eEndTemplateMode = 2,
    eFor = 3,
    eIdentifier = 4
  };

  explicit Token(Tag tag, std::string value);

  Token(const Token &other) = default;
  Token(Token &&other) = default;
  Token &operator=(const Token&) = default;

  Tag tag() const { return tag_; }
  std::string value() const { return value_; }

 private:
  const Tag tag_;
  const std::string value_;
};

inline
Token::Token(Token::Tag tag, std::string value)
  : tag_(tag), value_(value) {}

} // namespace yate
