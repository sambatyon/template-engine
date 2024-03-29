#include "renderer.hh"

#include "frame.hh"
#include "lexer.hh"
#include "token.hh"

#include <string>
#include <unordered_map>
#include <vector>

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

StreamPos Renderer::Render(std::ostream &output, bool write_output) {
  auto current = lexer_->Scan();
  while (current.tag() != Token::Tag::eEOF) {
    switch (current.tag()) {
      case Token::Tag::eNoOp: {
        // Literate mode token, just copy everything to the output
        // stream.
        if (write_output) {
          output << current.value();
        }
      } break;

      case Token::Tag::eScriptBegin: {
        current = lexer_->Scan();

        switch (current.tag()) {
          case Token::Tag::eIdentifier: {
            std::string id = current.value();
            if (!top_->ContainsValue(id)) {
              throw std::runtime_error("Identifier '" + id + "' is undefined");
            }

            if (write_output) {
              output << top_->GetValue(id);
            }
            current = lexer_->Scan();

            if (current.tag() != Token::Tag::eScriptEnd) {
              throw std::runtime_error(
                  CreateError(current, Token::Tag::eScriptEnd));
            }
          } break;

          case Token::Tag::eLoopBegin: {
            Token array_name, item_name;
            std::tie(array_name, item_name) = SetLoopFrame(current.value());
            auto loop_begin = lexer_->CurrentStreamPos();
            StreamPos loop_end;

            auto &array = top_->GetIterable(array_name.value());

            if (write_output && !array.empty()) {
              for (const auto &item : array) {
                top_->PutValue(item_name.value(), item);
                loop_end = Render(output, write_output);
                lexer_->SetStreamPos(loop_begin);
              }
            } else {
                top_->PutValue(item_name.value(), "");
              loop_end = Render(output, false);
            }

            RestoreParentFrame();
            lexer_->SetStreamPos(loop_end);
          } break;

          case Token::Tag::eLoopEnd: {
            if (top_ == root_) {
              throw std::runtime_error("Invalid Syntax: Unmatched 'LOOP_END'");
            }

            current = lexer_->Scan();
            if (current.tag() != Token::Tag::eScriptEnd) {
              throw std::runtime_error(
                  CreateError(current, Token::Tag::eScriptEnd));
            }
            return lexer_->CurrentStreamPos();
          } break;

          default:
            throw std::runtime_error(CreateError(current));
            break;
        }
      } break;

      default:
        // UNREACHABLE
        throw std::runtime_error(CreateError(current));
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
    throw std::runtime_error("Array '" + array_id.value() + "' is undefined");
  }
  auto item_id = lexer_->Scan(); // must be eIdentifier does not need to exist
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
  if (top_->parent() == nullptr) {
    // UNREACHABLE
    throw std::runtime_error("root frame has no parent.");
  }
  top_ = top_->parent();
}

std::string Renderer::CreateError(const Token &token, Token::Tag expected) {
  return "Invalid Syntax: Expected '" + to_string(expected) + "' but got '" +
         to_string(token.tag()) + "' " +
         (token.value().empty() ? "" : ("('" + token.value() + "')")) +
         " at line " + std::to_string(token.line()) + " column " +
         std::to_string(token.column());
}

std::string Renderer::CreateError(const Token &token) {
  return "Invalid Syntax: Unexpected token '" + to_string(token.tag()) + "' " +
         (token.value().empty() ? "" : ("('" + token.value() + "')")) +
         " at line " + std::to_string(token.line()) + " column " +
         std::to_string(token.column());
}

} // namespace yate
