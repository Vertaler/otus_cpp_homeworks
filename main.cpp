#include <cassert>
#include <iostream>
#include <list>
#include <vector>

#include "printIp.h"

int main(int argc, const char *const argv[]) {
  IpPrinter(std::cout) << char(-1) << short(0) << int(2130706433)
                       << long(8875824491850138409)
                       << std::string("192.168.1.1") << std::list{1, 2, 3, 4}
                       << std::vector{2, 0, 2, 1};

  return 0;
}