#pragma once

#include <iosfwd>
#include <string>

namespace yate {

std::string hello(const std::string& world);

class Environment {
 public:
  Environment();
  ~Environment() {}

  std::ostream &Render(std::istream &input, std::ostream &output);
};

} // namespace yate
