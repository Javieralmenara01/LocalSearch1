cmake_minimum_required(VERSION 3.15)  
project(Metaheuristic)

# Establece el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Asegurar que Release use -O3
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Agrega la ruta a vcpkg
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Añade el ejecutable
add_executable(main main.cc src/Functions.cc src/ProblemInstance.cc src/MetaHeuristicSolver.cc src/Solver.cc)

# Incluye el directorio de encabezados de vcpkg
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-osx/include")
