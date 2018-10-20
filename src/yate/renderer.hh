#pragma once

#include "lexer.hh"
#include "token.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

class Frame;

/// Interprest a template stored in an input stream and generates a
/// rendered results which is copied in the output stream.
class Renderer {
 public:
  /// Constructs a renderer which will use the given maps as the
  /// initial set of symbols to replace the tags.
  ///
  /// @param printable_values These are strings that can directly be
  ///        copied into the output stream.
  /// @param iterable_values These are the symbols which store vector
  ///        which can be used inside loops.
  Renderer(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Renderer() {}

  /// Interprest a template stored in an input stream and generates a
  /// rendered results which is copied in the output stream.
  /// NOTE: This function is not reentrant, so do not call it in a
  ///       multithreaded environment without protection.
  ///
  /// @param input The stream from which the template will be read.
  /// @param output The stream where the rendered output will be
  ///        stored.
  void Render(std::istream &input, std::ostream &output);

 private:
  std::shared_ptr<Frame> top_;
  std::shared_ptr<Frame> root_;
  std::unique_ptr<Lexer> lexer_;

  /// This method actually performs the rendering after the lexer has
  /// been initialized, which allows for this overload to be
  /// recursively called.
  ///
  /// @param output The stream where the rendered output will be
  ///        stored.
  /// @return The position of the input stream at the moment this
  ///        method returns.
  StreamPos Render(std::ostream &output);

  /// Creates a new Frame and sets it as the new top Frame. It returns
  /// a tuple containing the identifiers to be used to iterate over
  /// as well as the symbol which will be used for the loop variable.
  ///
  /// @param frame_id The frame identifier for the new  top frame.
  /// @return A tuple where position 0 contains the identifier of the
  ///        array the loop will iterate over and position 1 has the
  ///        identifier where the loop value will be stored.
  std::tuple<Token, Token> SetLoopFrame(const std::string &frame_id);
  void RestoreParentFrame();

  /// Helper function to generate error strings when a token does not
  /// match an specific expected one.
  std::string CreateError(const Token &token, Token::Tag expected);

  /// Helper function to generate error strings when a token does not
  /// match an expected one, but there is no specific token which
  /// could appear at that point.
  std::string CreateError(const Token &token);
};

} // namespace yate
