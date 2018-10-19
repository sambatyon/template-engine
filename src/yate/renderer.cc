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
  Render(output);
}

std::streampos Renderer::Render(std::ostream &output) {
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
              throw std::runtime_error("Identifier '" + id + "' is undefined");
            }
            output << top_->GetValue(id);
            current = lexer_->Scan();
            if (current.tag() != Token::Tag::eScriptEnd) {
              throw std::runtime_error(
                  CreateError(current, Token::Tag::eScriptEnd));
            }
          } break;
          case Token::Tag::eLoopBegin: {
            auto loop_variables = SetLoopFrame(current.value());
            auto loop_begin = lexer_->CurrentStreamPos();
            std::streampos loop_end;
            for (const auto &item :
                 top_->GetIterable(std::get<0>(loop_variables).value())) {
              top_->PutValue(std::get<1>(loop_variables).value(), item);
              loop_end = Render(output);
              lexer_->SetStreamPos(loop_begin);
            }
            RestoreParentFrame();
            lexer_->SetStreamPos(loop_end);
          } break;
          case Token::Tag::eLoopEnd: {
            current = lexer_->Scan();
            if (current.tag() != Token::Tag::eScriptEnd) {
              throw std::runtime_error(
                  CreateError(current, Token::Tag::eScriptEnd));
            }
            return lexer_->CurrentStreamPos();
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
  return lexer_->CurrentStreamPos();
}

std::tuple<Token, Token> Renderer::SetLoopFrame(const std::string &frame_id) {
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
  auto current = lexer_->Scan(); // must be `}}`
  if (current.tag() != Token::Tag::eScriptEnd) {
    throw std::runtime_error(CreateError(current, Token::Tag::eScriptEnd));
  }

  auto new_frame = std::make_shared<Frame>(top_, frame_id);
  top_ = new_frame;

  return {array_id, item_id};
}

void Renderer::RestoreParentFrame() {
  auto parent = top_->parent().lock();
  if (parent == nullptr) {
    throw std::runtime_error("Invalid Syntax, leaving parent frame");
  }
  top_ = parent;
}

std::string Renderer::CreateError(const Token &token, Token::Tag expected) {
  return "Invalid Syntax: Expected " + to_string(expected) + "'' but got '" +
         to_string(token.tag()) +
         (token.value().empty() ? "" : ("(" + token.value() + ")")) +
         " at line " + std::to_string(token.line()) + " column " +
         std::to_string(token.column());
}

} // namespace yate
