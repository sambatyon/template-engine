#include <iostream>

#include "lexer_tests.hh"
#include "render_tests.hh"

#include <yate/yate.hh>

int main(int argc, char **argv) {
  LexerTests lexer_tests;
  lexer_tests.RunTests();

  RenderTests render_tests;
  render_tests.RunTests();
  return 0;
}
