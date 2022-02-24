/**
 * \file poisson.hpp
 *
 * Poisson solver header file
 */

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

/**
 *  Class for denoting non-homogenuous Dirichlet boundary values.
 *  Function of dim-dimensional space variable. 
 */
template <int dim>
class BoundaryValues : public Function<dim>
{
public:
  virtual double value(const Point<dim> & p,
                       const unsigned int component = 0) const override;
};

/**
	 * Function to apply boundary values. For simplicity, we choose x^2+y^2 in 2D and x^2+y^2+z^2 in 3D which is equal to the sqare of the vector from
   * the origin to the point, where we evaluate the function. 
	 */
template <int dim>
double BoundaryValues<dim>::value(const Point<dim> &p,
                                  const unsigned int /*component*/) const
{
  return p.square();
}

/**
 *  Class for calculating the poisson problem on a radial domain. 
 */
class Radial_Poisson
{
public:
  Radial_Poisson(std::vector<double> _dimensions, int _refinement, int _shape_function, int _bc);
  void run(int _bc);
  void run();
private:
  void make_grid();
  void setup_system();
  void assemble_system();
  void solve();
  void output_results() const;

  std::vector<double> dimensions;
  int refinement;                       //!< Refinement of triangulation
  int bc;                               //!< Constant boundary condition
  

  Triangulation<2> triangulation;       //!< Collection of cells that jointly cover the domain
  FE_Q<2>          fe;                  //!< Implementation of scalar Lagrange finite element  that yields the finite element space.
  DoFHandler<2>    dof_handler;         //!< Global numbering of degrees of freedom
  SparsityPattern      sparsity_pattern;  //!< Class stores sparsity pattern in the CSR format
  SparseMatrix<double> system_matrix;   //!< Sparse matrix to store entry values in the locations denoted by SparsityPattern
  Vector<double> solution;              //!< Vector containing the solution 
  Vector<double> system_rhs;            //!< Vector containing the right hand side of the system
};




/**
 *  Class for calculating the poisson problem on a hyper rectangular domain in 2D and 3D.
 */
template <int dim>
class Poisson
{
public:
  Poisson(std::vector<int> _dimensions, int _refinement, int _shape_function, int _bc, bool _homogeneous);
  void run(int _bc);
  void run();
private:
  void make_grid();
  void setup_system();
  void assemble_system();
  void solve();
  void output_results() const;

  int refinement;                       //!< Refinement of triangulation
  int bc;                               //!< Constant boundary condition
  bool homogeneous;                     //!< If false, non-homogeneous BC are applied


  Triangulation<dim> triangulation;     //!< Collection of cells that jointly cover the domain
  FE_Q<dim>          fe;                //!< Implementation of scalar Lagrange finite element  that yields the finite element space.
  Point<dim> point;                     //!< Diagonally opposite corner point of hyper rectangle (p1 is origin)
  DoFHandler<dim>    dof_handler;       //!< Global numbering of degrees of freedom
  SparsityPattern      sparsity_pattern;  //!< Class stores sparsity pattern in the CSR format
  SparseMatrix<double> system_matrix;   //!< Sparse matrix to store entry values in the locations denoted by SparsityPattern
  Vector<double> solution;              //!< Vector containing the solution 
  Vector<double> system_rhs;            //!< Vector containing the right hand side of the system
};

/**
	 * Constructor for Poisson class
	 *
	 * \param _dimensions Dimensions of hyper rectangle defined by two points: origin and point with dimensions coordinates.
   * \param _refinement Refine all cells _refinement times. In each iteration, loops over all cells and refines each cell uniformly into  2^{dim}  children. 
   * The end result is the number of cells increased by a factor  2^{dim x _refinement} 
   * \param _shape_function Degree of continuous, piecewise polynomials for finite element space of Lagrangian finite elements.
   * \param _bc Constant Dirichlet boundary values 
	 * \return Constructed poisson class object
	 */
template <int dim>
Poisson<dim>::Poisson(std::vector<int> _dimensions, 
                      int _refinement, 
                      int _shape_function, int _bc, bool _homogeneous) 
  : refinement(_refinement), bc(_bc), homogeneous(_homogeneous), fe(_shape_function), dof_handler(triangulation)
{
  for(int i = 0; i < dim; i++){
    point[i] = _dimensions[i];
  }
  make_grid();
}

/**
	 * Function to create a hyper reactangular grid from the given coordinates and the origin. The triangulation is refined refinement times to yield a triangulation
   * with 2^{dim x refinement} cells. 
 	 *
	 * 
	 */
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

/**
	 * Enumerate all degrees of freedom and set up matrix and vector objects to hold the system data. The number of degrees of freedom depends on the 
   * polynomial degree of the finite elements. 
 	 * 
	 */
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

/**
	 * Compute the entries of the matrix and right hand side that form the linear system from which the solutio is computed. 
 	 * 
	 */
template <int dim>
void Poisson<dim>::assemble_system()
{
    
    QGauss<dim> quadrature_formula(fe.degree + 1);
    FEValues<dim> fe_values(fe, quadrature_formula, update_values | update_gradients | update_JxW_values);
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

    if(homogeneous)
      VectorTools::interpolate_boundary_values(dof_handler,0,Functions::ConstantFunction<dim>(bc),boundary_values);

    else  
      VectorTools::interpolate_boundary_values(dof_handler,0,BoundaryValues<dim>(),boundary_values);

    MatrixTools::apply_boundary_values(boundary_values,system_matrix,solution,system_rhs);

}

/**
	 * Solve the discretized equation. The Conjugate Gradients algorithm is used as a solver. The stopping criteria is either 1000 iterations or a residual below
   * 1e-12. The identity matrix is used as a preconditioner for the solver. 
 	 * 
	 */
template <int dim>
void Poisson<dim>::solve()
{
  SolverControl            solver_control(1000, 1e-12);
  SolverCG<Vector<double>> solver(solver_control);
  solver.solve(system_matrix, solution, system_rhs, PreconditionIdentity());
  std::cout << "   " << solver_control.last_step()
            << " CG iterations needed to obtain convergence." << std::endl;
}

/**
	 * Finally, the results are written to a file. The format is VTK. 
 	 * 
	 */
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

/**
	 * The run function is the main function of the class, that will trigger all other functions. Since there is only one API-like access point to the class,
   * the system is ot error prone. 
   * 
   * \param _bc Boundary condition for changed parameters, the grid can be reused. 
 	 * 
	 */
template <int dim>
void Poisson<dim>::run(int _bc)
{
  bc = _bc;
  std::cout << "Solving problem in " << dim << " space dimensions."
            << std::endl;
  setup_system();
  assemble_system();
  solve();
  output_results();
}

/**
	 * The run function is the main function of the class, that will trigger all other functions. Since there is only one API-like access point to the class,
   * the system is ot error prone. 
 	 * 
	 */
template <int dim>
void Poisson<dim>::run()
{
  std::cout << "Solving problem in " << dim << " space dimensions."
            << std::endl;
  setup_system();
  assemble_system();
  solve();
  output_results();
}
