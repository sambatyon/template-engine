#include "lexer_tests.hh"

#include <yate/lexer.hh>

#include <sstream>
#include <iostream>


int LexerTests::RunTests() {
  int result = 0;
  result = TestLiterateOnly() == 0 ? 0 : 1;
  return result;
}

int LexerTests::TestLiterateOnly() {
  std::stringstream stream("This is a {{string");
  yate::Lexer lexer(stream);
  auto token = lexer.Scan();
  assert(token.tag() == yate::Token::Tag::eNoOp);
  std::cout << token.value() << '\n';

  return 0;
}
