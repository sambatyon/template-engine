#pragma once

#include "lexer.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

class Frame;
class Token;

class Renderer {
 public:
  Renderer(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Renderer() {}

  void Render(std::istream &input, std::ostream &output);
  void ParseFrame(Token /*frame_type*/);

 private:
  std::shared_ptr<Frame> top_;
  std::shared_ptr<Frame> root_;
  std::unique_ptr<Lexer> lexer_;
};

} // namespace yate
