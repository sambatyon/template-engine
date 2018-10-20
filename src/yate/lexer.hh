#pragma once

#include <cstdint>
#include <iosfwd>
#include <istream>

#include "token.hh"

namespace yate {

class StreamPos;

/// This class is on charge of going through every character in the
/// stream and generate tokens out of it.
class Lexer {
 public:
  /// Creates a new lexer with the associated stream to it.
  ///
  /// @param istream The stream from which characters will be read.
  Lexer(std::istream &istream);
  ~Lexer() {}

  /// Consumes input from the associated stream until it matches any
  /// type of token and returns it. If the input does not match any
  /// token, it throws a `std::runtime_error`.
  Token Scan();

  /// Returns the current stream position, by doing `istream.tellg()`.
  /// It also keeps track of the number of lines read up to that
  /// position and the column within that line in which the stream is
  /// at this point.
  ///
  /// @return The current position of the stream.
  StreamPos CurrentStreamPos() const;

  /// Moves the underlying stream to the given position by calling
  /// `istream.seekg(0, pos)`. It updates the current line and column
  /// accordingly.
  ///
  /// @param pos The position where the stream should be set to.
  void SetStreamPos(StreamPos pos);

 private:
  /// Consumes one char of the stream and returns it. If any of
  /// `eofbit`, `failbit` or `badfit` it returns the '\0' bit.
  ///
  /// @return The next character in the stream.
  std::istream::char_type ReadChar();

  /// Consumes one character from the stream and compares it with
  /// the given one, returns true if the consumed character is the
  /// same as the given one, false otherwise.
  ///
  /// @param ch The expected value of the next character in the
  ///        stream.
  /// @return true if the next character from the stream is equal to
  ///         ch.
  bool ReadCompare(std::istream::char_type ch);

  /// Helper method to generate error messages including the line and
  /// column in the stream for easier debugging.
  ///
  /// @param message The core error message.
  /// @return A formated error including the given message as well as
  ///        the location in the input where the error was generated.
  std::string GenerateError(const std::string &message);

  /// Helper method called by `Scan()` when in literate mode. It
  /// consumes all input until it finds the token `{{` it which point
  /// it changes to script mode and returns.
  Token ScanLiterate();

  /// Helper method called by `Scan()` when in script mode. It
  /// tokenizes most of the input that actually needs specific
  /// processing.
  Token ScanScript();

  std::istream &istream_;
  std::istream::char_type current_;
  bool script_mode_;
  bool initialized_; // TODO: Find a more elegant solution to this.
  std::uint64_t id_generator_;
  bool must_return_script_begin_;
  std::uint32_t line_;
  std::uint32_t column_;
};

/// A position in the stream whose underlying implementation can be
/// used by `std::istream::seekg()` but also keeps track of the same
/// position but in human readable form, i.e. a line and a column.
class StreamPos {
 public:
  StreamPos();
  StreamPos(std::streampos pos, std::uint32_t line, std::uint32_t col);
  StreamPos(const StreamPos &other);
  StreamPos(StreamPos &&other);
  StreamPos &operator=(const StreamPos &other);
  ~StreamPos() {}

 private:
  friend class Lexer;
  std::streampos position_;
  std::uint32_t line_;
  std::uint32_t column_;
};

} // yate
