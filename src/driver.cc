// #include <iostream>

#include "illini_book.hpp"

int main() {
  IlliniBook book = IlliniBook("ppl.csv", "relationships.csv");
  std::vector<int> steps = book.GetSteps(1, 3);
  // for (unsigned int i = 0; i < steps.size(); ++i) {
  //   std::cout << steps.at(i) << std::endl;
  // }
  return 0;
}
