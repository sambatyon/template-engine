#include "renderer.hh"

#include "frame.hh"
#include "lexer.hh"
#include "token.hh"

namespace yate {

Renderer::Renderer(
    std::unordered_map<std::string, std::string> printable_values,
    std::unordered_map<std::string, std::vector<std::string>> iterable_values)
    : root_(std::make_shared<Frame>(printable_values, iterable_values)),
      top_() {
  top_ = root_;
}

void Renderer::Render(std::istream &input, std::ostream &output) {
}

void Renderer::ParseFrame(Token frame_type) {
  // Create a new frame with id frame_type.value()
  // Set this frame in the children frames of the parent
  // replace top.
  auto new_frame = std::make_shared<Frame>(top_);

  auto current = frame_type;
  // TODO: validate this.
  auto array_id = lexer_->Scan(); // must be eIdentifier and exist
  auto item_id = lexer_->Scan(); // must be eIdentifier and exist
  current = lexer_->Scan(); // must be }}
  while (current.tag() != Token::Tag::eEOF) {
    current = lexer_->Scan();
    if (current.tag() == Token::Tag::eScriptBegin) {
      current = lexer_->Scan();
      if (current.tag() == Token::Tag::eLoopBegin) {
        ParseFrame(current);
      } else if (current.tag() == Token::Tag::eIdentifier) {
        top_->tokens().push_back(current);
        current = lexer_->Scan();
        if (current.tag() != Token::Tag::eScriptEnd) {
          throw std::runtime_error("Invalid Syntax");
        }
      } else if (current.tag() == Token::Tag::eLoopEnd) {
        current = lexer_->Scan();
        if (current.tag() != Token::Tag::eScriptEnd) {
          throw std::runtime_error("Invalid Syntax");
        }
        // Replace top frame with parent frame.
        return;
      }
    }
    top_->tokens().push_back(current);
  }
}

} // namespace yate
