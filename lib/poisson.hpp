/* File: poisson.hpp */

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

class Poisson
{
  public:
    Poisson(std::vector<int> _dimensions, int _refinement, int _shape_function);
    void prepare();
    void run();

  private:
    void make_grid();
    void vtk_grid();
    void setup_system();
    void assemble_system();
    void solve();
    void output_results() const;

    std::vector<int> dimensions;
    int refinement;
    int shape_functions;

    Triangulation<2> triangulation;
    Point<2> point;
    FE_Q<2> finite_element;
    DoFHandler<2> dof_handler;

    SparsityPattern sparsity_pattern;
    SparseMatrix<double> system_matrix;

    Vector<double> solution;
    Vector<double> system_rhs;
};
