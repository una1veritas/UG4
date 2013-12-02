
#include <iostream>
#include <stdlib.h>

#include "layout.hpp"

int main(int argc, char **argv) {
  using namespace gridlayout;

  int g = 0;

  if(argc < 2) {
    std::cerr << "USAGE : layout [gridsize/int] < [input] > [output]" << std::endl;
    return 0;
  }
  g = atoi(argv[1]);

  PointSetProxy<int> pset(std::cin);
  GridLayout<int>(&pset, g);
  pset.print(std::cout);

  return 0;
}
