#include "lexer_tests.hh"

#include <yate/lexer.hh>

#include <sstream>
#include <iostream>


int LexerTests::RunTests() {
  int result = 0;
  result += TestLiterateOnly() == 0 ? 0 : 1;
  result += TestMultiTokenInput() == 0 ? 0 : 1;
  return result;
}

int LexerTests::TestLiterateOnly() {
  std::stringstream stream("This is a {\\{string{\\o");
  yate::Lexer lexer(stream);
  auto token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  std::cout << token.value() << '\n';

  return 0;
}


int LexerTests::TestMultiTokenInput() {
  std::stringstream stream(
      "Input {\\{complex{\\this is {{with}} some {{#loop array item}} funny "
      "{{item}} in it {{/loop}}");
  yate::Lexer lexer(stream);

  auto token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eLoopBegin);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eIdentifier);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eLoopEnd);
  std::cout << token.value() << '\n';

  token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eEOF);
  std::cout  << token.value() << '\n';

  return 0;
}
