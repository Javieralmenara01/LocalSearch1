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
#include "./src/MetaHeuristicSolver.h"
#include "./src/Solver.h"

/**
 * Función principal.
 */
int main(int argc, char* argv[]) {
  
  // Comprobar los argumentos
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

  // Cargar la instancia del problema
  ProblemInstance problem;
  problem.loadFromJSON(filename);
 
  /// Hora de inicio
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm local_time = *std::localtime(&now_time);
  std::cout << "Hora de inicio: " << std::put_time(&local_time, "%H:%M:%S") << std::endl;

  /// Ejecutar el solver
  MetaHeuristicSolver solver(problem);
  auto [bestSolution, bestFitness] = solver.geneticAlgorithm(100);

  // Hora de finalización
  now = std::chrono::system_clock::now();
  now_time = std::chrono::system_clock::to_time_t(now);
  local_time = *std::localtime(&now_time);
  std::cout << "Hora de finalización: " << std::put_time(&local_time, "%H:%M:%S") << std::endl;

  return 0;
}
