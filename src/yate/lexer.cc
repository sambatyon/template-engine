#include "lexer.hh"

#include <iostream>

namespace yate {

Lexer::Lexer(std::istream &istream)
    : istream_(istream),
      current_(),
      script_mode_(false),
      initialized_(false),
      id_generator_(0),
      must_return_script_begin_(false),
      line_(1),
      column_(0) {}

std::istream::char_type Lexer::ReadChar() {
  istream_.get(current_);
  if (istream_.eof() || istream_.fail() || istream_.bad()) {
    current_ = '\0';
  }
  ++column_;
  if (current_ == '\n') {
    ++line_;
    column_ = 0;
  }
  return current_;
}

bool Lexer::ReadCompare(std::istream::char_type ch) {
  ReadChar();
  if (current_ == ch) {
    return true;
  }
  return false;
}

Token Lexer::Scan() {
  // Small workaround which prevents issuing EOF as the first token.
  if (initialized_ && current_ == '\0') {
    return Token(Token::Tag::eEOF, "", line_, column_);
  }
  initialized_ = true;
  if (script_mode_) {
    return ScanScript();
  } else {
    return ScanLiterate();
  }
}

Token Lexer::ScanScript() {
  // In literate mode when the lexer encounters the `{{` it returns
  // `NOOP` since `{{` where consumed it sets a flag so the next token
  // emitted is SCRIPT_BEGIN. Alternatively those two characters could
  // be put ack in the stream, but then this method would need to
  // include code to parse that input too.
  if (must_return_script_begin_) {
    must_return_script_begin_ = false;
    return Token(Token::Tag::eScriptBegin, "{{", line_, column_-2);
  }

  // In script mode we discard all spaces
  while (std::isspace(current_)) {
    ReadChar();
  }
  auto line = line_;
  auto column = column_;
  // Handles keywords begin which should start with '#'
  if (current_ == '#') {
    if (ReadCompare('l') && ReadCompare('o') && ReadCompare('o') &&
        ReadCompare('p')) {
      ReadChar();
      if (std::isalnum(current_)) {
        throw std::runtime_error(GenerateError("Invalid keyword found"));
      }
      return Token(
          Token::Tag::eLoopBegin,
          "#loop" + std::to_string(id_generator_++),
          line,
          column);
    } else {
      throw std::runtime_error(GenerateError("Invalid keyword found."));
    }
  }
  // Handles keywords ends, which should start with '/'
  if (current_ == '/') {
    if (ReadCompare('l') && ReadCompare('o') && ReadCompare('o') &&
        ReadCompare('p')) {
      ReadChar();
      if (std::isalnum(current_)) {
        throw std::runtime_error(GenerateError("Invalid keyword found."));
      }
      return Token(Token::Tag::eLoopEnd, "/loop", line, column);
    } else {
      throw std::runtime_error(GenerateError("Invalid keyword found."));
    }
  }
  // Handles Identifiers.
  if (std::isalpha(current_)) {
    std::string value;
    do {
      value += current_;
      ReadChar();
    } while (std::isalnum(current_));
    return Token(Token::Tag::eIdentifier, value, line, column);
  }
  // Handles end of script mode.
  if (current_ == '}' && ReadCompare('}')) {
    script_mode_ = false;
    return Token(Token::Tag::eScriptEnd, "}}", line, column);
  }
  if (current_ == '\0') {
    throw std::runtime_error(GenerateError("EOF found inside script mode."));
  }
  std::string message = "Cannot recognize character '";
  message += current_;
  message += "'.";
  throw std::runtime_error(GenerateError(message));
}

Token Lexer::ScanLiterate() {
  ReadChar();
  auto line = line_;
  auto column = column_;
  std::string value;
  // Consume all input until we come to a `{{` which indicates the
  // begin of script mode.
  while (!script_mode_ && current_ != '\0') {
    if (current_ == '{') {
      if (ReadCompare('{')) {
        script_mode_ = true;
        current_ = ' '; // This will cause script mode to ignore this value.
        if (!value.empty()) {
          must_return_script_begin_ = true;
          return Token(Token::Tag::eNoOp, value, line, column);
        } else {
          must_return_script_begin_ = false;
          return Token(Token::Tag::eScriptBegin, "{{", line, column);
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
    return Token(Token::Tag::eNoOp, value, line, column);
  } else {
    return Token(Token::Tag::eEOF, "EOF", line, column);
  }
}

StreamPos Lexer::CurrentStreamPos() const {
  return StreamPos(istream_.tellg(), line_, column_);
}

void Lexer::SetStreamPos(StreamPos pos) {
  istream_.seekg(pos.position_, std::ios_base::beg);
  line_ = pos.line_;
  column_ = pos.column_;
}

std::string Lexer::GenerateError(const std::string &message)
{
  return "Error found in line " + std::to_string(line_) + " column " +
      std::to_string(column_) + ": " + message;
}

StreamPos::StreamPos() : StreamPos(0, 0, 0) {}

StreamPos::StreamPos(std::streampos pos, std::uint32_t line, std::uint32_t col)
    : position_(pos), line_(line), column_(col) {}

StreamPos::StreamPos(const StreamPos &other)
    : StreamPos(other.position_, other.line_, other.column_) {}

StreamPos::StreamPos(StreamPos &&other)
    : StreamPos(other.position_, other.line_, other.column_) {}

StreamPos &StreamPos::operator=(const StreamPos &other) {
  if (this == &other) {
    return *this;
  }
  position_ = other.position_;
  line_ = other.line_;
  column_ = other.column_;
  return *this;
}

} // namespace yate
