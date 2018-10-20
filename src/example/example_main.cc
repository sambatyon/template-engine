#include <yate/yate.hh>

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

int main(int argc, char **argv) {
  std::unordered_map<std::string, std::string> values({
      {"header", "hello"},
      {"footer", "That's it"}
    });
  std::unordered_map<std::string, std::vector<std::string>> arrays({
      {"somearray", {"apple", "banana", "citrus"}}
    });
  std::stringstream input(
      "First Line.\n"
      "{{header}}\n"
      "{{#loop somearray item}}This is a {{item}}.\n"
      "{{/loop}}"
      "{{footer}}\n"
      "Last Line.");
  yate::Render(values, arrays, input, std::cout);
}
