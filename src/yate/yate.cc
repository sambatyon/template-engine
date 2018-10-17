#include <yate/yate.hh>

namespace yate {

std::string hello(const std::string& world) {
  return "hello " + world + '!';
}

} // namespace yate
