/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 *
 * @author Javier Almenara Herrera
 * @brief Declaración de clase que representa una solución al problema de asignación hospitalaria.
 * @file MetaHeuristicSolver.h
 */

// MetaHeuristicSolver.h
#ifndef METAHEURISTICSOLVER_H
#define METAHEURISTICSOLVER_H

#include "ProblemInstance.h"
#include "Structs.h"
#include <vector>
#include <random>
#include <thread>
#include <mutex>

class MetaHeuristicSolver {
 public:
  MetaHeuristicSolver(ProblemInstance &problemInstance);
  std::tuple<std::vector<EncodedPatientSolution>, int> geneticAlgorithm(int populationSize);
  
 private:
  ProblemInstance& problem;
  std::mt19937 rng;
  Solution solution;
  
  // Operadores de cruce
  std::vector<EncodedPatientSolution> pmxCrossoverPatients(const std::vector<EncodedPatientSolution>& parent1, const std::vector<EncodedPatientSolution>& parent2);
  std::vector<EncodedPatientSolution> applyPMXPatients(const std::vector<EncodedPatientSolution>& parent1, const std::vector<EncodedPatientSolution>& parent2);
  std::vector<std::vector<std::string>> pmxCrossoverNurses(const std::vector<std::vector<std::string>>& parent1, const std::vector<std::vector<std::string>>& parent2);
  
  /// Operador de mutación
  void mutatePatients(std::vector<EncodedPatientSolution>& individual);
  void mutateNurses(std::vector<std::vector<std::string>>& individual);

  // Generación de población inicial
  std::vector<EncodedSolution> generateInitialPopulation(int populationSize);
};

#endif // METAHEURISTICSOLVER_H
