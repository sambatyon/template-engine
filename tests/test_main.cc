#include <iostream>

#include "lexer_tests.hh"
#include "render_tests.hh"

#include <yate/yate.hh>

int main(int argc, char **argv) {
  int return_code = 0;
  LexerTests lexer_tests;
  return_code += lexer_tests.RunTests();

  RenderTests render_tests;
  return_code += render_tests.RunTests();

  return return_code;
}
