#include "render_tests.hh"

#include <yate/renderer.hh>

#include <sstream>
#include <iostream>

int RenderTests::RunTests() {
  int result = 0;
  result += TestFlatTemplate();
  result += TestSimpleTemplate();
  return 0;
}

int RenderTests::TestFlatTemplate() {
  yate::Renderer renderer({}, {});
  std::stringstream input("Hello World");
  std::stringstream output;
  renderer.Render(input, output);
  assert(output.str() == std::string("Hello World"));
  return 0;
}

int RenderTests::TestSimpleTemplate() {

  yate::Renderer renderer({{"foo", "bar"}, {"weather", "sunny"}}, {});
    std::stringstream input(
        "Hello {{foo}}, How are you doing in this {{weather}} day?");
    std::stringstream output;
    renderer.Render(input, output);
    assert(output.str() == "Hello bar, How are you doing in this sunny day?");

    input.str("{{foo}}{{weather}}");
    input.clear();
    output.str("");
    output.clear();
    renderer.Render(input, output);
    assert(output.str() == "barsunny");
    return 0;
}
