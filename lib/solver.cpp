/* File: solver.hpp */

#include "solver.hpp"

void Solver::generateRadialGrid(double inner_radius, double outer_radius)
{
  dealii::Triangulation<2> triangulation;
  const dealii::Point<2> center(1, 0);
  dealii::GridGenerator::hyper_shell(triangulation, center, inner_radius, outer_radius, 10);

  for (unsigned int step = 0; step < 5; ++step)
  {
    for (auto cell : triangulation.active_cell_iterators())
    {
      for (unsigned int v = 0; v < dealii::GeometryInfo<2>::vertices_per_cell; ++v)
      {
        const double distance_from_center = center.distance(cell->vertex(v));
        if (std::fabs(distance_from_center - inner_radius) < 1e-10)
        {
          cell->set_refine_flag();
          break;
        }
      }
    }
    triangulation.execute_coarsening_and_refinement();
  }
  
  std::ofstream out("radialGrid.vtk");
  dealii::GridOut grid_out;
  grid_out.write_vtk(triangulation, out);
  //std::cout << "Grid written to grid-2.vtk" << std::endl;
  triangulation.reset_manifold(0);
}
