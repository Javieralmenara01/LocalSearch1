/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * 
 * @author Javier Almenara Herrera
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include "./src/ProblemInstance.h"
#include "./src/RandomSolver.h"

/**
 * Función principal.
 */
int main(int argc, char* argv[]) {
  
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <archivo.json>" << std::endl;
    return 1;
  }

  // Abrir el archivo JSON
  std::string filename = argv[1];
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "No se pudo abrir el archivo JSON." << std::endl;
    return 1;
  }

  // Generar solución
  ProblemInstance problem;
  problem.loadFromJSON(filename);
 
  // Ejecutar el algoritmo de resolución n veces y quedarse con la mejor solución
  auto start = std::chrono::high_resolution_clock::now();
  int bestSoftConstraints = std::numeric_limits<int>::max();
  while (std::chrono::high_resolution_clock::now() - start < std::chrono::minutes(10)) {
    
    try {
      ProblemInstance problem_aux;
      problem_aux = problem;
      RandomSolver solver(problem_aux);
      Solution solution = solver.generateSolution();
      if (solution.total_soft_constraints < bestSoftConstraints) {
        bestSoftConstraints = solution.total_soft_constraints;
        solution.exportToJSON("../RandomSearch/sol_test10.json");
      }
    } catch (const std::exception& e) {
      continue;
    }
    
  }

  return 0;
}
