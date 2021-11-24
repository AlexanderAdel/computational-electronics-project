#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace dealii;
void first_grid(int refinement)
{
  Triangulation<2> triangulation;
  GridGenerator::hyper_cube(triangulation);
  triangulation.refine_global(refinement);
  std::ofstream out("grid-1.vtk");
  GridOut       grid_out;
  grid_out.write_vtk(triangulation, out);
  std::cout << "Grid written to grid-1.vtk" << std::endl;
}