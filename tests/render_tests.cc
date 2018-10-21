#include "render_tests.hh"

#include "unit.hh"

#include <yate/renderer.hh>

#include <sstream>

int RenderTests::RunTests() {
  int result = 0;
  result += TestFlatTemplate();
  result += TestSimpleTemplate();
  result += TestTemplateWithLoop();
  result += TestTemplateNestedLoop();
  result += TestTemplateVariableShadowing();
  result += TestRenderErrors();
  result += TestLoopWithEmptyArray();
  return result;
}

// Initial render test, just checks simple strings are handled
// properly.
int RenderTests::TestFlatTemplate() {
  yate::Renderer renderer({}, {});
  std::stringstream input("Hello World");
  std::stringstream output;
  renderer.Render(input, output);
  TEST_EXPECT_EQ(output.str(), "Hello World");
  return 0;
}

// Checks that strings are copied correctly into the rendered result.
int RenderTests::TestSimpleTemplate() {
  yate::Renderer renderer({{"foo", "bar"}, {"weather", "sunny"}}, {});
  std::stringstream input(
      "Hello {{foo}}, How are you doing in this {{weather}} day?");
  std::stringstream output;
  renderer.Render(input, output);
  TEST_EXPECT_EQ(
      output.str(), "Hello bar, How are you doing in this sunny day?");

  input.str("{{foo}}{{weather}}");
  input.clear();
  output.str("");
  output.clear();
  renderer.Render(input, output);
  TEST_EXPECT_EQ(output.str(), "barsunny");
  return 0;
}

// Tests simple use for loops in the template engine.
int RenderTests::TestTemplateWithLoop() {
  yate::Renderer renderer(
      {
        {"foo", "bar"}
      },
      {
        {"hobbies",
          {
            "baking",
            "music",
            "hiking"
           }
        }
      });
  std::stringstream input(
      "Hi, I'm {{foo}} and my hobbies are {{#loop hobbies hobby}}\n- "
      "{{hobby}}{{/loop}}");
  std::stringstream output;
  renderer.Render(input, output);
  TEST_EXPECT_EQ(
      output.str(),
      "Hi, I'm bar and my hobbies are \n- baking\n- music\n- hiking");
  return 0;
}

// Tests behavior when a loop uses an empty array.
int RenderTests::TestLoopWithEmptyArray() {
  yate::Renderer renderer(
      {
        {"symbol", "value"}
      },
      {
        {"array", {}}
      });

  std::stringstream input(
      "{{symbol}}{{#loop array item}}{{symbol}} {{item}}{{/loop}}{{symbol}}");
  std::stringstream output;
  renderer.Render(input, output);

  TEST_EXPECT_EQ(output.str(), "valuevalue");
  return 0;
}

// Tests proper handling of nested loops.
int RenderTests::TestTemplateNestedLoop() {
  yate::Renderer renderer(
      {
        {"test", "something"},
        {"foo", "bar"}
      },
      {
        {"level0",
          {
            "first",
            "second",
            "third"
           }
        },
        {"level1",
          {
            "erste",
            "zweite",
            "dritte"
           }
        }
      });

  std::stringstream input(
      "{{#loop level0 item0}}"
      "{{test}}"
      "{{#loop level1 item1}}"
      "\n{{item0}}.{{item1}} {{foo}}"
      "{{/loop}}"
      "{{/loop}}");
  std::stringstream output;
  renderer.Render(input, output);

  TEST_EXPECT_EQ(
      output.str(),
      "something\n"
      "first.erste bar\n"
      "first.zweite bar\n"
      "first.dritte barsomething\n"
      "second.erste bar\n"
      "second.zweite bar\n"
      "second.dritte barsomething\n"
      "third.erste bar\n"
      "third.zweite bar\n"
      "third.dritte bar");

  return 0;
}

// Tests that the engine has proper handling of symbol shadowing
// inside loops.
int RenderTests::TestTemplateVariableShadowing() {
  yate::Renderer renderer(
      {
        {"foo", "bar"}
      },
      {
        {"array",
          {
            "first",
            "second"
          }
        }
      });
  std::stringstream input("{{foo}}{{#loop array foo}}{{foo}}{{/loop}}{{foo}}");
  std::stringstream output;
  renderer.Render(input, output);

  TEST_EXPECT_EQ(output.str(), "barfirstsecondbar");

  return 0;
}

// Tests that input validation in templates is handled correctly.
int RenderTests::TestRenderErrors() {
  yate::Renderer renderer(
      {
        {"foo", "bar"}
      },
      {
        {"array",
          {
            "first",
            "second"
          }
        }
      });

  // Malformed script.
  {
    std::stringstream input("{{}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Invalid Syntax: Unexpected token 'SCRIPT_END' ('}}') "
        "at line 1 column 3");
  }

  // Get an error while using an undefined variable.
  {
    std::stringstream input("{{name}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Identifier 'name' is undefined");
  }

  // Get an error while using an undefined array.
  {
    std::stringstream input("{{#loop name");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Array 'name' is undefined");
  }

  // Loop finished without setting a variable name for items.
  {
    std::stringstream input("{{#loop array}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Invalid Syntax: Expected 'IDENTIFIER' but got 'SCRIPT_END' ('}}') "
        "at line 1 column 14");
  }

  // Loop contains too many items.
  {
    std::stringstream input("{{#loop array item extra}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Invalid Syntax: Expected 'SCRIPT_END' but got 'IDENTIFIER' ('extra') "
        "at line 1 column 20");
  }

  // Unmatched loop end
  {
    std::stringstream input("{{/loop}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Invalid Syntax: Unmatched 'LOOP_END'");
  }

  // More complex unmatched loop end
  {
    std::stringstream input("{{#loop array item}}{{/loop}}{{/loop}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Invalid Syntax: Unmatched 'LOOP_END'");
  }

  // Malformed loop end
  {
    std::stringstream input("{{#loop array item}}{{/loop #loop}}");
    std::stringstream output;
    TEST_EXPECT_EXCEPTION(
        renderer.Render(input, output),
        std::runtime_error,
        "Invalid Syntax: Expected 'SCRIPT_END' but got 'LOOP_BEGIN' ('#loop1') "
        "at line 1 column 29");
  }

  return 0;
}
