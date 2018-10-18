#include "lexer_tests.hh"

#include <yate/lexer.hh>
#include <yate/utils.hh>

#include <iostream>
#include <sstream>
#include <string>


int LexerTests::RunTests() {
  int result = 0;
  result += TestLiterateOnly() == 0 ? 0 : 1;
  result += TestMultiTokenInput() == 0 ? 0 : 1;
  result += TestInputValidation() == 0 ? 0 : 1;
  return result;
}

// Very simple test used to check the behavior of the lexer when given
// only literate mode characters. It also checks that the string
// `{\\{` is properly handled (so that there is a way to write `{{`.
int LexerTests::TestLiterateOnly() {
  std::stringstream stream("This is a {\\{string{\\o");
  yate::Lexer lexer(stream);
  auto token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == "This is a {{string{\\o");

  return 0;
}


// Tests the functionality of the whole lexer making use of all the
// recognized tokens.
int LexerTests::TestMultiTokenInput() {
  std::stringstream stream(
      "Input {\\{complex{\\this is {{with}} some {{#loop array item}} funny "
      "{{item}} in it {{/loop}}{{with}}");
  yate::Lexer lexer(stream);

  auto token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == "Input {{complex{\\this is ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptBegin);
  assert(token.value() == "{{");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "with");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptEnd);
  assert(token.value() == "}}");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == " some ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptBegin);
  assert(token.value() == "{{");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eLoopBegin);
  assert(token.value() == "#loop0");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "array");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "item");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptEnd);
  assert(token.value() == "}}");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == " funny ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptBegin);
  assert(token.value() == "{{");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "item");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptEnd);
  assert(token.value() == "}}");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == " in it ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptBegin);
  assert(token.value() == "{{");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eLoopEnd);
  assert(token.value() == "/loop");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptEnd);
  assert(token.value() == "}}");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptBegin);
  assert(token.value() == "{{");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "with");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eScriptEnd);
  assert(token.value() == "}}");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eEOF);

  return 0;
}

int LexerTests::TestInputValidation() {
  {
    // Check that EOF inside script mode is an error.
    std::stringstream stream("{{");
    yate::Lexer lexer(stream);
    bool error_thrown = false;
    try {
      lexer.Scan();
      lexer.Scan();
    } catch (const std::runtime_error &e) {
      error_thrown = true;
      assert(e.what() == std::string("EOF found inside script mode."));
    }
    assert(error_thrown);
  }
  {
    // Check that `#loop` is not accepted with suffixes.
    std::stringstream stream("{{#looper}}");
    yate::Lexer lexer(stream);
    bool error_thrown = false;
    try {
      lexer.Scan();
      lexer.Scan();
    } catch (const std::runtime_error &e) {
      error_thrown = true;
      assert(e.what() == std::string("Invalid keyword found."));
    }
    assert(error_thrown);
  }
  {
    // Check that we fail with unknown keywords
    std::stringstream stream("{{#while}}");
    yate::Lexer lexer(stream);
    bool error_thrown = false;
    try {
      lexer.Scan();
      lexer.Scan();
    } catch (const std::runtime_error &e) {
      error_thrown = true;
      assert(e.what() == std::string("Invalid keyword found."));
    }
    assert(error_thrown);
  }
  {
    // Check that `/loop` is not accepted with suffixes.
    std::stringstream stream("{{/looper}}");
    yate::Lexer lexer(stream);
    bool error_thrown = false;
    try {
      lexer.Scan();
      lexer.Scan();
    } catch (const std::runtime_error &e) {
      error_thrown = true;
      assert(e.what() == std::string("Invalid keyword found."));
    }
    assert(error_thrown);
  }
  {
    // Check that we fail with unknown keywords
    std::stringstream stream("{{/while}}");
    yate::Lexer lexer(stream);
    bool error_thrown = false;
    try {
      lexer.Scan();
      lexer.Scan();
    } catch (const std::runtime_error &e) {
      error_thrown = true;
      assert(e.what() == std::string("Invalid keyword found."));
    }
    assert(error_thrown);
  }

  {
    // Check that we fail with invalid strings.
    std::stringstream stream("{{#loop 12}}");
    yate::Lexer lexer(stream);
    bool error_thrown = false;
    try {
      lexer.Scan();
      lexer.Scan();
      lexer.Scan();
    } catch (const std::runtime_error &e) {
      error_thrown = true;
      assert(begins_with(std::string(e.what()),
                         std::string("Cannot recognize character '")));
    }
    assert(error_thrown);
  }
  return 0;
}
