#include "lexer_tests.hh"

#include <yate/lexer.hh>

#include <iostream>
#include <sstream>
#include <string>


int LexerTests::RunTests() {
  int result = 0;
  result += TestLiterateOnly() == 0 ? 0 : 1;
  result += TestMultiTokenInput() == 0 ? 0 : 1;
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


int LexerTests::TestMultiTokenInput() {
  std::stringstream stream(
      "Input {\\{complex{\\this is {{with}} some {{#loop array item}} funny "
      "{{item}} in it {{/loop}}");
  yate::Lexer lexer(stream);

  auto token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == "Input {{complex{\\this is ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "with");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == " some ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eLoopBegin);
  assert(token.value() == "#loop");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "array");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "item");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == " funny ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  assert(token.value() == "item");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  assert(token.value() == " in it ");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eLoopEnd);
  assert(token.value() == "/loop");

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eEOF);

  return 0;
}
