#include "lexer_tests.hh"
#include "unit.hh"

#include <yate/lexer.hh>
#include <yate/utils.hh>

#include <iostream>
#include <regex>
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
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eNoOp);
  TEST_EXPECT_EQ(token.value(), "This is a {{string{\\o");
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
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eNoOp);
  TEST_EXPECT_EQ(token.value(), "Input {{complex{\\this is ");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptBegin);
  TEST_EXPECT_EQ(token.value(), "{{");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eIdentifier);
  TEST_EXPECT_EQ(token.value(), "with");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptEnd);
  TEST_EXPECT_EQ(token.value(), "}}");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eNoOp);
  TEST_EXPECT_EQ(token.value(), " some ");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptBegin);
  TEST_EXPECT_EQ(token.value(), "{{");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eLoopBegin);
  TEST_EXPECT_EQ(token.value(), "#loop0");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eIdentifier);
  TEST_EXPECT_EQ(token.value(), "array");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eIdentifier);
  TEST_EXPECT_EQ(token.value(), "item");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptEnd);
  TEST_EXPECT_EQ(token.value(), "}}");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eNoOp);
  TEST_EXPECT_EQ(token.value(), " funny ");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptBegin);
  TEST_EXPECT_EQ(token.value(), "{{");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eIdentifier);
  TEST_EXPECT_EQ(token.value(), "item");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptEnd);
  TEST_EXPECT_EQ(token.value(), "}}");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eNoOp);
  TEST_EXPECT_EQ(token.value(), " in it ");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptBegin);
  TEST_EXPECT_EQ(token.value(), "{{");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eLoopEnd);
  TEST_EXPECT_EQ(token.value(), "/loop");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptEnd);
  TEST_EXPECT_EQ(token.value(), "}}");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptBegin);
  TEST_EXPECT_EQ(token.value(), "{{");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eIdentifier);
  TEST_EXPECT_EQ(token.value(), "with");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eScriptEnd);
  TEST_EXPECT_EQ(token.value(), "}}");

  token = lexer.Scan();
  TEST_EXPECT_EQ(token.tag(), yate::Token::Tag::eEOF);

  return 0;
}

int LexerTests::TestInputValidation() {
  {
    // Check that EOF inside script mode is an error.
    std::stringstream stream("{{");
    yate::Lexer lexer(stream);
    lexer.Scan();
    TEST_EXPECT_EXCEPTION(
        lexer.Scan(),
        std::runtime_error,
        "Error found in line 1 column 3: EOF found inside script mode.");
  }
  {
    // Check that `#loop` is not accepted with suffixes.
    std::stringstream stream("{{#looper}}");
    yate::Lexer lexer(stream);
    lexer.Scan();
    TEST_EXPECT_EXCEPTION(
        lexer.Scan(),
        std::runtime_error,
        "Error found in line 1 column 8: Invalid keyword found");
  }
  {
    // Check that we fail with unknown keywords
    std::stringstream stream("{{#while}}");
    yate::Lexer lexer(stream);
    lexer.Scan();
    TEST_EXPECT_EXCEPTION(
        lexer.Scan(),
        std::runtime_error,
        "Error found in line 1 column 4: Invalid keyword found.");
  }
  {
    // Check that `/loop` is not accepted with suffixes.
    std::stringstream stream("{{/looper}}");
    yate::Lexer lexer(stream);
    lexer.Scan();
    TEST_EXPECT_EXCEPTION(
        lexer.Scan(),
        std::runtime_error,
        "Error found in line 1 column 8: Invalid keyword found.");
  }
  {
    // Check that we fail with unknown keywords
    std::stringstream stream("{{/while}}");
    yate::Lexer lexer(stream);
    lexer.Scan();
    TEST_EXPECT_EXCEPTION(
        lexer.Scan(),
        std::runtime_error,
        "Error found in line 1 column 4: Invalid keyword found.");
  }

  {
    // Check that we fail with invalid strings.
    std::stringstream stream("{{#loop 12}}");
    yate::Lexer lexer(stream);
    lexer.Scan();
    lexer.Scan();
    TEST_EXPECT_EXCEPTION_REGEX(
        lexer.Scan(),
        std::runtime_error,
        "Error found in line 1 column 9: Cannot recognize character '.'");
  }
  return 0;
}
