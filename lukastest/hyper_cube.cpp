#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>


#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>

#include <deal.II/dofs/dof_renumbering.h>

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

//! make_grid
/*!
  make_grid function will use the unit square as domain and produce a globally refined grid from it
  the triangulation is refined #refinement times
*/
void make_grid(Triangulation<2> &triangulation, int refinement)
{
  GridGenerator::hyper_cube(triangulation);
  triangulation.refine_global(refinement);

  std::cout << "Number of active cells: " << triangulation.n_active_cells() << std::endl;
}

//! vtk_grid
/*!
  vtk_grid function will write graphical representation of mesh to an output file in vtk format
*/
void vtk_grid(Triangulation<2> &triangulation)
{
  std::ofstream out("grid.vtk");
  GridOut grid_out;
  grid_out.write_vtk(triangulation, out);

  std::cout << "Grid written to: grid.vtk " << std::endl;
}

void distribute_dofs(DoFHandler<2> &dof_handler, int dim, int order)
{
  const FE_Q<order> finite_element<2>;
  dof_handler.distribute_dofs(finite_element);
  //optional: print sparsity pattern ?!
}

