#include "render_tests.hh"

#include "unit.hh"

#include <yate/renderer.hh>

#include <sstream>
#include <iostream>

int RenderTests::RunTests() {
  int result = 0;
  result += TestFlatTemplate();
  result += TestSimpleTemplate();
  result += TestTemplateWithLoop();
  result += TestTemplateNestedLoop();
  result += TestTemplateVarialbleShadowing();
  return 0;
}

int RenderTests::TestFlatTemplate() {
  yate::Renderer renderer({}, {});
  std::stringstream input("Hello World");
  std::stringstream output;
  renderer.Render(input, output);
  TEST_EXPECT_EQ(output.str(), "Hello World");
  return 0;
}

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
      "Hi, I'm {{foo}} and my hobies are {{#loop hobbies hobby}}\n- "
      "{{hobby}}{{/loop}}");
  std::stringstream output;
  renderer.Render(input, output);
  TEST_EXPECT_EQ(
      output.str(),
      "Hi, I'm bar and my hobies are \n- baking\n- music\n- hiking");
  return 0;
}

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

int RenderTests::TestTemplateVarialbleShadowing() {
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
