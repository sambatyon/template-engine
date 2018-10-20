#include <yate/yate.hh>

#include "renderer.hh"

namespace yate {

void Render(
    const std::unordered_map<std::string, std::string> &values,
    const std::unordered_map<std::string, std::vector<std::string>> &arrays,
    std::istream &input,
    std::ostream &output) {
  Renderer renderer(values, arrays);
  renderer.Render(input, output);
}

} // namespace yate
