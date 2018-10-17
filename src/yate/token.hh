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

} // namespace yate
