cmake_minimum_required(VERSION 3.10)

FIND_PACKAGE(deal.II 9.3.2 REQUIRED
    HINTS $(DEAL_II_DIR) ../ ../ ../ $ENV{DEAL_II_DIR}
    /usr/local/lib/
)
DEAL_II_INITIALIZE_CACHED_VARIABLES()

project (poisson)

ADD_EXECUTABLE(poisson main.cpp)
DEAL_II_SETUP_TARGET(poisson)

set(CMAKE_CXX_STANDARD 17)