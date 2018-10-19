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
    throw std::runtime_error(CreateError(array_id, Token::Tag::eIdentifier));
  }
  if (!top_->ContainsIterable(array_id.value())) {
    throw std::runtime_error("Array " + array_id.value() + " is not defined");
  }
  auto item_id = lexer_->Scan(); // must be eIdentifier
  if (item_id.tag() != Token::Tag::eIdentifier) {
    throw std::runtime_error(CreateError(item_id, Token::Tag::eIdentifier));
  }
  current = lexer_->Scan(); // must be `}}`
  if (current.tag() != Token::Tag::eScriptEnd) {
    throw std::runtime_error(CreateError(current, Token::Tag::eScriptEnd));
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
          throw std::runtime_error(
              CreateError(current, Token::Tag::eScriptEnd));
        }
      } else if (current.tag() == Token::Tag::eLoopEnd) {
        current = lexer_->Scan();
        if (current.tag() != Token::Tag::eScriptEnd) {
          throw std::runtime_error(
              CreateError(current, Token::Tag::eScriptEnd));
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

std::string Renderer::CreateError(const Token &token, Token::Tag expected) {
  return "Invalid Syntax: Expected " + to_string(expected) + "'' but got '" +
         to_string(token.tag()) +
         (token.value().empty() ? "" : ("(" + token.value() + ")")) +
         " at line " + std::to_string(token.line()) + " column " +
         std::to_string(token.column());
}

} // namespace yate
