/* File: poisson.cpp */

#include "poisson.hpp"
using namespace dealii;


Poisson::Poisson(std::vector<int> _dimensions, int _refinement, int _shape_function): refinement(_refinement), shape_functions(_shape_function), finite_element(_shape_function), dof_handler(triangulation)
{
  point[0] = _dimensions[0];
  point[1] = _dimensions[1];
}

//! make_grid
/*!
  make_grid function will use the unit square as domain and produce a globally refined grid from it
  the triangulation is refined #refinement times
*/
void Poisson::make_grid()
{
  Point<2> origin;
  //GridGenerator::hyper_cube(triangulation, -1 , 1);
  GridGenerator::hyper_rectangle(triangulation, origin, point, false);
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
  solution.reinit(dof_handler.n_dofs());
  system_rhs.reinit(dof_handler.n_dofs());
}

void Poisson::assemble_system()
{
    QGauss<2> quadrature_formula(finite_element.degree + 1);
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

    /* Change boundary value here, with either ZeroFunction<2>() or ConstantFunction<2>(value)*/
    VectorTools::interpolate_boundary_values(dof_handler,0,Functions::ConstantFunction<2>(1),boundary_values);

    MatrixTools::apply_boundary_values(boundary_values,system_matrix,solution,system_rhs);

}

void Poisson::solve()
{
    SolverControl solver_control(1000, 1e-12);
    SolverCG<Vector<double>> solver(solver_control);
    solver.solve(system_matrix, solution, system_rhs, PreconditionIdentity());
}

void Poisson::output_results() const
{
  DataOut<2> data_out;
  data_out.attach_dof_handler(dof_handler);
  data_out.add_data_vector(solution, "Solution");
  data_out.build_patches();

  std::ofstream output("solution.vtk");
  data_out.write_vtk(output);
}

void Poisson::prepare()
{
  make_grid();
  vtk_grid();
}

void Poisson::run()
{
  setup_system();
  assemble_system();
  solve();
  output_results();
}