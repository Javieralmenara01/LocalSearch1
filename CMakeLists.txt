cmake_minimum_required(VERSION 3.15)
project(Metaheuristic)

# Establece el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Asegurar que Release use -O3
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Agrega la ruta al toolchain de vcpkg
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Busca la librería nlohmann_json instalada vía vcpkg
find_package(nlohmann_json CONFIG REQUIRED)

# Busca la librería de threads de CMake
find_package(Threads REQUIRED)

# Añade el ejecutable con las fuentes correspondientes
add_executable(main 
    main.cc 
    src/Functions.cc 
    src/ProblemInstance.cc 
    src/MetaHeuristicSolver.cc 
    src/Solver.cc
)

# Enlaza la librería nlohmann_json y la biblioteca de hilos (pthread) al ejecutable
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json Threads::Threads)
