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
  lexer_ = std::make_unique<Lexer>(input);
  auto current = lexer_->Scan();
  while (current.tag() != Token::Tag::eEOF) {
    switch (current.tag()) {
      case Token::Tag::eNoOp: {
        output << current.value();
      } break;
      case Token::Tag::eScriptBegin: {
        current = lexer_->Scan();
        switch (current.tag()) {
          case Token::Tag::eIdentifier: {
            std::string id = current.value();
            if (!top_->ContainsValue(id)) {
              throw std::runtime_error("Use of undeclared id '" + id + "'");
            }
            output << top_->GetValue(id);
            current = lexer_->Scan();
            if (current.tag() != Token::Tag::eScriptEnd) {
              throw std::runtime_error(
                  "Syntax Error: Unexpected token " + current.value());
            }
          } break;
          case Token::Tag::eLoopBegin: {
            std::string frame_id = current.value();
            ParseFrame(current);
            // print(frame_id);
          } break;
          default:
            throw std::runtime_error(
                "Syntax Error: Unexpected token " + current.value());
            break;
        }
      } break;
      default:
        throw std::runtime_error(
            "Syntax Error: Unexpected token " + current.value());
        break;
    }
    current = lexer_->Scan();
  }
  // TODO: clear child frames.
}

void Renderer::ParseFrame(Token frame_type) {
  auto current = frame_type;
  auto array_id = lexer_->Scan(); // must be eIdentifier and exist
  if (array_id.tag() != Token::Tag::eIdentifier) {
    throw std::runtime_error("Invalid Syntax");
  }
  if (!top_->ContainsIterable(array_id.value())) {
    throw std::runtime_error("Use of invalid array name " + array_id.value());
  }
  auto item_id = lexer_->Scan(); // must be eIdentifier
  if (array_id.tag() != Token::Tag::eIdentifier) {
    throw std::runtime_error("Invalid Syntax");
  }
  current = lexer_->Scan(); // must be `}}`
  if (array_id.tag() != Token::Tag::eScriptEnd) {
    throw std::runtime_error("Invalid Syntax");
  }

  auto new_frame = std::make_shared<Frame>(
      top_, frame_type.value(), array_id.value(), item_id.value());

  top_->AddNestedFrame(new_frame);
  top_ = new_frame;

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
        auto parent = top_->parent().lock();
        if (parent == nullptr) {
          throw std::runtime_error("Invalid Syntax, leaving parent frame");
        }
        top_ = parent;
        return;
      }
    }
    top_->tokens().push_back(current);
  }
}

} // namespace yate
