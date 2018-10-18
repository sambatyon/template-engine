#include <yate/yate.hh>

#include <iostream>

namespace yate {

std::string hello(const std::string& world) {
  return "hello " + world + '!';
}

Environment::Environment() {}

std::ostream &Environment::Render(std::istream &input, std::ostream &output) {
  return output;
}

} // namespace yate
