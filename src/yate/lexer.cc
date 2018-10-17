#include "lexer.hh"

#include <iostream>

namespace yate {

Lexer::Lexer(std::istream &istream)
    : istream_(istream), current_(), script_mode_(false), initialized_(false) {
}

std::istream::char_type Lexer::ReadChar() {
  istream_.get(current_);
  if (istream_.eof() || istream_.fail() || istream_.bad()) {
    current_ = '\0';
  }
  return current_;
}

std::istream::char_type Lexer::ReadCompare(std::istream::char_type ch) {
  ReadChar();
  if (current_ == ch) {
    return true;
  }
  return false;
}

Token Lexer::Scan() {
  if (initialized_ && current_ == '\0') {
    return Token(Token::Tag::eEOF, "");
  }
  initialized_ = true;
  if (script_mode_) {
    return ScanScript();
  } else {
    return ScanLiterate();
  }
}

Token Lexer::ScanScript() {
  // In script mode we discard all spaces
  while (current_ == ' ' || current_ == '\t') {
    ReadChar();
  }
  // Handles keywords begin which should start with '#'
  if (current_ == '#') {
    if (ReadCompare('l') && ReadCompare('o') && ReadCompare('o') &&
        ReadCompare('p')) {
      ReadChar();
      if (std::isalnum(current_)) {
        throw std::runtime_error("Invalid input");
      }
      return Token(Token::Tag::eLoopBegin, "#loop");
    } else {
      throw std::runtime_error("Invalid input");
    }
  }
  // Handles keywords ends, which should start with '/'
  if (current_ == '/') {
    if (ReadCompare('l') && ReadCompare('o') && ReadCompare('o') &&
        ReadCompare('p')) {
      ReadChar();
      if (std::isalnum(current_)) {
        throw std::runtime_error("Invalid input");
      }
      return Token(Token::Tag::eLoopEnd, "/loop");
    } else {
      throw std::runtime_error("Invalid input");
    }
  }
  // Handles Identifiers.
  if (std::isalpha(current_)) {
    std::string value;
    do {
      value += current_;
      ReadChar();
    } while (std::isalnum(current_));
    return Token(Token::Tag::eIdentifier, value);
  }
  // Handles end of script mode.
  if (current_ == '}' && ReadCompare('}')) {
    script_mode_ = false;
    if (current_ == '\0') {
      return Token(Token::Tag::eEOF, "");
    }
    return ScanLiterate();
  }
  throw std::runtime_error("Invalid input");
}

Token Lexer::ScanLiterate() {
  ReadChar();
  std::string value;
  // Consume all input until we come to a `{{` which indicates the
  // begin of script mode.
  while (!script_mode_ && current_ != '\0') {
    if (current_ == '{') {
      if (ReadCompare('{')) {
        script_mode_ = true;
        current_ = ' '; // This will cause script mode to ignore this value.
        if (!value.empty()) {
          return Token(Token::Tag::eNoOp, value);
        } else {
          return ScanScript();
        }
      } else if (current_ == '\\') {
        if (ReadCompare('{')) {
          value += '{';
        } else {
          value += "{\\";
        }
      } else {
        value += '{';
      }
    }
    value += current_;
    ReadChar();
  }
  // Investigate the case of the string containing only '\0'
  if (!value.empty()) {
    return Token(Token::Tag::eNoOp, value);
  } else {
    return Token(Token::Tag::eEOF, "EOF");
  }
}

} // namespace yate
