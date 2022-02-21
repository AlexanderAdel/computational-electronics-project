# File: CMakeLists.txt

# Terminal command:
# cmake -DVTK_DIR="/home/alexander/VTK/VTK-9.1.0/Build/lib/cmake/vtk-9.1" 
#       -Ddeal.II_DIR="/home/alexander/DEAL_II/dealii-9.3.2/build/lib/cmake/deal.II" ..

cmake_minimum_required(VERSION 3.0)

# 0. Project Properties

project(ComputationalElectronicsProject
        VERSION 0.1.0.0 
        LANGUAGES CXX 
        DESCRIPTION "Poisson Equation Solver with GUI")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 1. Doxygen 

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Build Doxygen documentation" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
	if(DOXYGEN_FOUND)
		set(DOXYGEN_IN ${CMAKE_CURRENT_LIST_DIR}/docs/Doxyfile.in)
		set(DOXYGEN_OUT ${PROJECT_BINARY_DIR}/Doxyfile)

		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
		message("-- Doxygen build enabled")

		add_custom_target(docs ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			COMMENT "Generating API docs with Doxygen"
			VERBATIM)
	else()
		message(FATAL_ERROR 
			"Doxygen generation requested but package was not found")
	endif()
endif()

# 2. Deal II Library with Solver Library

find_package(deal.II 9.3.2 REQUIRED)
DEAL_II_INITIALIZE_CACHED_VARIABLES()

# 3. Poisson Solver Library

add_library(PoissonLib STATIC lib/poisson.cpp)
DEAL_II_SETUP_TARGET(PoissonLib)

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

target_link_libraries(${PROJECT_NAME} PoissonLib
                                      Qt5::Widgets 
                                      Qt5::OpenGL
                                      ${VTK_LIBRARIES})

vtk_module_autoinit(TARGETS ${PROJECT_NAME} MODULES ${VTK_LIBRARIES})
