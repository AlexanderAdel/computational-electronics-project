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
#include <vector> 

using namespace dealii;

class Poisson
{
  public:
    Poisson();
    void run();

  private():
    void make_grid();
    void vtk_grid();
    void setup_system();
    void assemble_system();
    void solve();
    void output_results();

    Triangulation<2> triangulation;
    FE_Q<2> fe;
    DoFHandler<2> dof_handler;

    SparsityPattern sparsity_pattern;
    SparseMatrix<double> system_matrix;

    Vector<double> solution;
    Vector<double> system_rhs;
};

Poisson::Poisson(): finite_element(1), dof_handler(triangulation)
{}

//! make_grid
/*!
  make_grid function will use the unit square as domain and produce a globally refined grid from it
  the triangulation is refined #refinement times
*/
void Poisson::make_grid()
{
  GridGenerator::hyper_cube(triangulation);
  triangulation.refine_global(refinement);

  std::cout << "Number of active cells: " << triangulation.n_active_cells() << std::endl;
}

//! vtk_grid
/*!
  vtk_grid function will write graphical representation of mesh to an output file in vtk format
*/
void Poisson::vtk_grid()
{
  std::ofstream out("grid.vtk");
  GridOut grid_out;
  grid_out.write_vtk(triangulation, out);

  std::cout << "Grid written to: grid.vtk " << std::endl;
}

void Poisson::setup_system()
{
  dof_handler.distribute_dofs(finite_element);

  std::cout << "Number of degrees of freedom: " << dof_handler.n_dofs() << std::endl;
  
  DynamicSparsityPattern dsp(dof_handler.n_dofs());
  DoFTools::make_sparsity_pattern(dof_handler, dsp);
  sparsity_pattern.copy_from(dsp);

  system_matrix.reinit(sparsity_pattern);
  system_rhs.reinit(dof_handler.n_dofs());
}

void Poisson::assemble_system()
{
    QGauss<2> quadrature_formula(fe.degree + 1);
    FEValues<2> fe_values(finite_element, quadrature_formula, update_values | update_gradients | update_JxW_values);
    const unsigned int dofs_per_cell = finite_element.n_dofs_per_cell();

    FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
    Vector<double> cell_rhs(dofs_per_cell);

    std::vector<types::global_dof_index> local_dof_indices(dofs_per_cell);

    for (const auto &cell : dof_handler.active_cell_iterators())
    {
        fe_values.reinit(cell);
        cell_matrix = 0;
        cell_rhs    = 0;
        for (const unsigned int q_index : fe_values.quadrature_point_indices())
        {
            for (const unsigned int i : fe_values.dof_indices())
                for (const unsigned int j : fe_values.dof_indices())
                cell_matrix(i, j) +=
                    (fe_values.shape_grad(i, q_index) * // grad phi_i(x_q)
                    fe_values.shape_grad(j, q_index) * // grad phi_j(x_q)
                    fe_values.JxW(q_index));           // dx
            for (const unsigned int i : fe_values.dof_indices())
                cell_rhs(i) += (fe_values.shape_value(i, q_index) * // phi_i(x_q)
                                1. *                                // f(x_q)
                                fe_values.JxW(q_index));            // dx
        }
        cell->get_dof_indices(local_dof_indices);

        for (const unsigned int i : fe_values.dof_indices())
            for (const unsigned int j : fe_values.dof_indices())
                system_matrix.add(local_dof_indices[i], local_dof_indices[j],cell_matrix(i, j));

        for (const unsigned int i : fe_values.dof_indices())
            system_rhs(local_dof_indices[i]) += cell_rhs(i);
    }

    std::map<types::global_dof_index, double> boundary_values;
    VectorTools::interpolate_boundary_values(dof_handler,0,Functions::ZeroFunction<2>(),boundary_values);

    MatrixTools::apply_boundary_values(boundary_values,system_matrix,solution,system_rhs);

}

void Poisson::solve()
{
    SolverControl solver_control(1000, 1e-12);
    SolverCG<Vector<double>> solver(solver_control);
}

