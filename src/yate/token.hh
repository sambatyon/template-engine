#pragma once

#include <ostream>
#include <string>

namespace yate {

/// A token represents a semantic symbol which needs to be processed
/// by the parser. Each token has an associated `Tag` which indicates
/// which kind of symbol it is, as well as a `value`. It also contains
/// the line number and column where this Token was initially parsed.
class Token {
 public:
  enum class Tag {
    eEOF = 0,   /// Special token emitted when the input stream has been
                /// consumed.
    eNoOp = 1,  /// Token representing a contiguous amount of text which won't
                /// be interpret
    eLoopBegin = 4,    /// The keyword `#loop`.
    eLoopEnd = 5,      /// The keyword `/loop`.
    eIdentifier = 6,   /// Identifier follows the regex [a-zA-Z_][a-zA-Z0-9_]*
    eScriptBegin = 7,  /// The string `{{`, used to switch between literal and
                       /// script modes.
    eScriptEnd = 8     /// The string `}}`, used to switch between literal and
                       /// script modes.
  };

  /// Initializes a token with the given type (`tag`), `value` and its
  /// position in the stream.
  ///
  /// @param tag The type of the token.
  /// @param value The value of the string that caused this token to be issued.
  /// @param line The line number in the input stream.
  /// @param col The column in `line` where this token began.
  Token(Tag tag, std::string value, std::uint32_t line, std::uint16_t col);
  ~Token() {}

  // Token is copyable and movable.
  Token(const Token &other);
  Token(Token &&other);
  Token &operator=(const Token&);

  // Getters for Token attributes.
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

/// Helper function to get the string representation of a
/// `Token::Tag`. Follows the conventions of the STL in the `string`
/// header.
///
/// @param tag An element of the `Token::Tag` enum.
/// @return the string representation of the given `tag`.
std::string to_string(Token::Tag tag);

/// Stream operator for `Token::Tag`. It is equivalent to:
/// `stream << to_string(tag)`.
std::ostream &operator<<(std::ostream &stream, Token::Tag tag);

} // namespace yate
