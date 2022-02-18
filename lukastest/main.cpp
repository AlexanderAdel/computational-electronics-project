#include "poisson.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

int main(){

  std::vector<int> dimensions = {2,4};

  Poisson poisson_problem(dimensions, 4 , 1);
  poisson_problem.prepare();
  poisson_problem.run();
}
