
#pragma once

#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>


#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/dofs/dof_renumbering.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>


#include <deal.II/numerics/data_out.h>
#include <deal.II/base/point.h>

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace dealii;




template <int dim>
class Poisson
{
public:
  Poisson(std::vector<int> _dimensions, int _refinement, int _shape_function);
  void run();
private:
  void make_grid();
  void setup_system();
  void assemble_system();
  void solve();
  void output_results() const;

  std::vector<int> dimensions;
  int refinement;


  Triangulation<dim> triangulation;
  FE_Q<dim>          fe;
  Point<dim> point;
  DoFHandler<dim>    dof_handler;
  SparsityPattern      sparsity_pattern;
  SparseMatrix<double> system_matrix;
  Vector<double> solution;
  Vector<double> system_rhs;
};


template <int dim>
class RightHandSide : public Function<dim>
{
public:
  virtual double value(const Point<dim> & p,
                       const unsigned int component = 0) const override;
};


template <int dim>
class BoundaryValues : public Function<dim>
{
public:
  virtual double value(const Point<dim> & p,
                       const unsigned int component = 0) const override;
};


template <int dim>
double RightHandSide<dim>::value(const Point<dim> &p,
                                 const unsigned int /*component*/) const
{
  double return_value = 0.0;
  for (unsigned int i = 0; i < dim; ++i)
    return_value += 4.0 * std::pow(p(i), 4.0);
  return return_value;
}


template <int dim>
double BoundaryValues<dim>::value(const Point<dim> &p,
                                  const unsigned int /*component*/) const
{
  return p.square();
}


template <int dim>
Poisson<dim>::Poisson(std::vector<int> _dimensions, 
                      int _refinement, 
                      int _shape_function) 
  : refinement(_refinement), fe(_shape_function), dof_handler(triangulation)
{
  for(int i = 0; i < dim; i++){
    point[i] = _dimensions[i];
  }
}


template <int dim>
void Poisson<dim>::make_grid()
{
  Point<dim> origin;
  GridGenerator::hyper_rectangle(triangulation, origin, point, false);
  triangulation.refine_global(refinement);
  std::cout << "   Number of active cells: " << triangulation.n_active_cells()
            << std::endl
            << "   Total number of cells: " << triangulation.n_cells()
            << std::endl;
}


template <int dim>
void Poisson<dim>::setup_system()
{
  dof_handler.distribute_dofs(fe);
  std::cout << "   Number of degrees of freedom: " << dof_handler.n_dofs()
            << std::endl;
  DynamicSparsityPattern dsp(dof_handler.n_dofs());
  DoFTools::make_sparsity_pattern(dof_handler, dsp);
  sparsity_pattern.copy_from(dsp);
  system_matrix.reinit(sparsity_pattern);
  solution.reinit(dof_handler.n_dofs());
  system_rhs.reinit(dof_handler.n_dofs());
}


template <int dim>
void Poisson<dim>::assemble_system()
{
  QGauss<dim> quadrature_formula(fe.degree + 1);
  RightHandSide<dim> right_hand_side;
  FEValues<dim> fe_values(fe,
                          quadrature_formula,
                          update_values | update_gradients |
                            update_quadrature_points | update_JxW_values);
  const unsigned int dofs_per_cell = fe.n_dofs_per_cell();
  FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
  Vector<double>     cell_rhs(dofs_per_cell);
  std::vector<types::global_dof_index> local_dof_indices(dofs_per_cell);
  for (const auto &cell : dof_handler.active_cell_iterators())
    {
      fe_values.reinit(cell);
      cell_matrix = 0;
      cell_rhs    = 0;
      for (const unsigned int q_index : fe_values.quadrature_point_indices())
        for (const unsigned int i : fe_values.dof_indices())
          {
            for (const unsigned int j : fe_values.dof_indices())
              cell_matrix(i, j) +=
                (fe_values.shape_grad(i, q_index) * // grad phi_i(x_q)
                 fe_values.shape_grad(j, q_index) * // grad phi_j(x_q)
                 fe_values.JxW(q_index));           // dx
            const auto &x_q = fe_values.quadrature_point(q_index);
            cell_rhs(i) += (fe_values.shape_value(i, q_index) * // phi_i(x_q)
                            right_hand_side.value(x_q) *        // f(x_q)
                            fe_values.JxW(q_index));            // dx
          }
      cell->get_dof_indices(local_dof_indices);
      for (const unsigned int i : fe_values.dof_indices())
        {
          for (const unsigned int j : fe_values.dof_indices())
            system_matrix.add(local_dof_indices[i],
                              local_dof_indices[j],
                              cell_matrix(i, j));
          system_rhs(local_dof_indices[i]) += cell_rhs(i);
        }
    }
  std::map<types::global_dof_index, double> boundary_values;
  VectorTools::interpolate_boundary_values(dof_handler,
                                           0,
                                           BoundaryValues<dim>(),
                                           boundary_values);
  MatrixTools::apply_boundary_values(boundary_values,
                                     system_matrix,
                                     solution,
                                     system_rhs);
}


template <int dim>
void Poisson<dim>::solve()
{
  SolverControl            solver_control(1000, 1e-12);
  SolverCG<Vector<double>> solver(solver_control);
  solver.solve(system_matrix, solution, system_rhs, PreconditionIdentity());
  std::cout << "   " << solver_control.last_step()
            << " CG iterations needed to obtain convergence." << std::endl;
}


template <int dim>
void Poisson<dim>::output_results() const
{
  DataOut<dim> data_out;
  data_out.attach_dof_handler(dof_handler);
  data_out.add_data_vector(solution, "solution");
  data_out.build_patches();
  std::ofstream output(dim == 2 ? "solution-2d.vtk" : "solution-3d.vtk");
  data_out.write_vtk(output);
}


template <int dim>
void Poisson<dim>::run()
{
  std::cout << "Solving problem in " << dim << " space dimensions."
            << std::endl;
  make_grid();
  setup_system();
  assemble_system();
  solve();
  output_results();
}
