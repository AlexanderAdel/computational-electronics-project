#include "poisson.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

int main(){
  Poisson poisson_problem;
  poisson_problem.prepare();
  poisson_problem.run();
}
