# File: CMakeLists.txt

# Terminal command:
# cmake -DVTK_DIR="/home/alexander/VTK/VTK-9.1.0/Build/lib/cmake/vtk-9.1" 
#       -Ddeal.II_DIR="/home/alexander/DEAL_II/dealii-9.3.2/build/lib/cmake/deal.II" ..

cmake_minimum_required(VERSION 3.0)

# 1. Project Properties

project(ComputationalElectronicsProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 2. Deal II Library with Solver Library

find_package(deal.II 9.3.2 REQUIRED)
DEAL_II_INITIALIZE_CACHED_VARIABLES()

# 3. Solver Library

add_library(Solver STATIC lib/solver.cpp)
DEAL_II_SETUP_TARGET(Solver)

add_library(Poisson STATIC lib/poisson.cpp)
DEAL_II_SETUP_TARGET(Poisson)

# 4. Qt Library 

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)

# 5. VTK Library

find_package(VTK 9.0 REQUIRED COMPONENTS GUISupportQt 
                                         IOLegacy
                                         CommonColor
                                         CommonCore
                                         CommonDataModel
                                         CommonComputationalGeometry
                                         FiltersCore
                                         FiltersGeneral
                                         FiltersSources
                                         FiltersGeometry
                                         IOLegacy
                                         InteractionStyle
                                         RenderingAnnotation
                                         RenderingContextOpenGL2
                                         RenderingCore
                                         RenderingFreeType
                                         RenderingGL2PSOpenGL2
                                         RenderingOpenGL2)

# 6. Project Executable

add_executable(${PROJECT_NAME} src/VisualizationGUI.cpp 
                               src/VisualizationWidget.hpp
                               src/VisualizationWindow.hpp)
DEAL_II_SETUP_TARGET(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Solver
                                      Poisson
                                      Qt5::Widgets 
                                      Qt5::OpenGL
                                      ${VTK_LIBRARIES})

vtk_module_autoinit(TARGETS ${PROJECT_NAME} MODULES ${VTK_LIBRARIES})
