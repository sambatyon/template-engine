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
