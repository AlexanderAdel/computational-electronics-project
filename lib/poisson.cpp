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


void Radial_Poisson::setup_system()
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


void Radial_Poisson::assemble_system()
{
    QGauss<2> quadrature_formula(fe.degree + 1);
    FEValues<2> fe_values(fe, quadrature_formula, update_values | update_gradients | update_JxW_values);
    const unsigned int dofs_per_cell = fe.n_dofs_per_cell();

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
    VectorTools::interpolate_boundary_values(dof_handler,0,Functions::ConstantFunction<2>(bc),boundary_values);

    MatrixTools::apply_boundary_values(boundary_values,system_matrix,solution,system_rhs);

}

void Radial_Poisson::solve()
{
  SolverControl            solver_control(1000, 1e-12);
  SolverCG<Vector<double>> solver(solver_control);
  solver.solve(system_matrix, solution, system_rhs, PreconditionIdentity());
  std::cout << "   " << solver_control.last_step()
            << " CG iterations needed to obtain convergence." << std::endl;
}

void Radial_Poisson::output_results() const
{
  DataOut<2> data_out;
  data_out.attach_dof_handler(dof_handler);
  data_out.add_data_vector(solution, "solution");
  data_out.build_patches();
  std::ofstream output("solution-2d.vtk");
  data_out.write_vtk(output);
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