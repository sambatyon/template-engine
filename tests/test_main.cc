#include <iostream>

#include <yate/yate.hh>

int main(int argc, char **argv) {
  std::cout << yate::hello("world") << '\n';
  return 0;
}
