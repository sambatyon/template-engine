#pragma once

#include "lexer.hh"
#include "token.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

class Frame;

class Renderer {
 public:
  Renderer(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Renderer() {}

  void Render(std::istream &input, std::ostream &output);

 private:
  std::shared_ptr<Frame> top_;
  std::shared_ptr<Frame> root_;
  std::unique_ptr<Lexer> lexer_;

  StreamPos Render(std::ostream &output);
  std::tuple<Token, Token> SetLoopFrame(const std::string &frame_id);
  void RestoreParentFrame();

  std::string CreateError(const Token &token, Token::Tag expected);
  std::string CreateError(const Token &token);
};

} // namespace yate
