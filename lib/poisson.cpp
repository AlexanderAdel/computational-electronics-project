#include "poisson.hpp"
using namespace dealii;


Radial_Poisson::Radial_Poisson(std::vector<double> _dimensions, 
                      int _refinement, 
                      int _shape_function, int _bc) 
  : refinement(_refinement), bc(_bc), fe(_shape_function), dof_handler(triangulation)
{
  dimensions = _dimensions;
}

void Radial_Poisson::make_grid()
{
  const Point<2> center(1, 0);
  const double   inner_radius = dimensions[0], outer_radius = dimensions[1];
  GridGenerator::hyper_shell(
    triangulation, center, inner_radius, outer_radius);
  for (unsigned int step = 0; step < 3; ++step)
    {
      for (auto &cell : triangulation.active_cell_iterators())
        for (const auto v : cell->vertex_indices())
          {
            const double distance_from_center =
              center.distance(cell->vertex(v));
            if (std::fabs(distance_from_center - inner_radius) <=
                1e-6 * inner_radius)
              {
                cell->set_refine_flag();
                break;
              }
          }
      triangulation.execute_coarsening_and_refinement();
    }
}


void Radial_Poisson::run()
{
  std::cout << "Solving radial problem in 2 space dimensions."
            << std::endl;
  make_grid();
  setup_system();
  assemble_system();
  solve();
  output_results();
}



