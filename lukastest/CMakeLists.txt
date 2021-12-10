cmake_minimum_required(VERSION 3.10)

project (poisson)

FIND_PACKAGE(deal.II 9.1.1 REQUIRED
    HINTS $(DEAL_II_DIR) ../ ../ ../ $ENV{DEAL_II_DIR}
    /usr/local/lib/
)
DEAL_II_INITIALIZE_CACHED_VARIABLES()

# 3. poisson Library

add_library(Poissonlib STATIC poisson.cpp)
DEAL_II_SETUP_TARGET(Poissonlib)



ADD_EXECUTABLE(poisson main.cpp)
DEAL_II_SETUP_TARGET(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Poissonlib)

DEAL_II_SETUP_TARGET(poisson)

set(CMAKE_CXX_STANDARD 17)
