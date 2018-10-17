#include "lexer.hh"

#include <iostream>

namespace yate {

Lexer::Lexer(std::istream &istream)
    : istream_(istream), current_(), script_mode_(false) {
}

std::istream::char_type Lexer::ReadChar() {
  istream_.get(current_);
  if (istream_.eof() || istream_.fail() || istream_.bad()) {
    current_ = '\0';
  }
  return current_;
}

Token Lexer::Scan() {
  ReadChar();
  if (script_mode_) {
  } else {
    std::string value;
    do {
      if (current_ == '{') {
        ReadChar();
        if (current_ == '{') {
          script_mode_ = true;
          return Token(Token::Tag::eNoOp, value);
        } else {
          value += '{';
        }
      }
      value += current_;
      ReadChar();
    } while (!script_mode_ && current_ != '\0');
    return Token(Token::Tag::eNoOp, value);
  }
  return Token(Token::Tag::eNoOp, "");
}

} // namespace yate
