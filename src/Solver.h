/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 *
 * @author Javier Almenara Herrera
 * @brief Implementación de clase que representa una solución al problema de asignación hospitalaria.
 * @file Solver.h
 */

//  Solver.h
#ifndef SOLVER_H
#define SOLVER_H

#include "ProblemInstance.h"
#include "Structs.h"
#include <vector>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>

/**
 * Clase que representa una solución al problema de asignación hospitalaria.
 */
class Solver {
 public:
  Solver(ProblemInstance problemInstance);
  
  // Recibe una solución codificada (con el paciente, día de admisión y habitación sugerida) y la procesa para intentar obtener una solución completa y factible.
  std::pair<int, int> solve(EncodedSolution& encodedSolution);
  void exportSolution(const std::string& filename);

  std::pair<int,int> localSearchNurses(EncodedSolution& enc);
  
 private:
  ProblemInstance problem;
  ProblemInstance originalProblem;
  std::mt19937 rng;
  Solution solution;

  // Inicialización de los estados dinámicos
  void initializeDynamicStates();
  void applyNurses(const EncodedSolution& enc);

  std::pair<int,int> computeNurseOnlyCosts();

  // Funciones de verificación de disponibilidad
  bool checkSurgeonAvailability(const Patient &patient, int day);
  bool checkOperatingTheaterAvailability(const Patient &patient, int day);
  bool checkRoomAvailability(const Patient &patient, int day, const std::string &room_id);
  
  // Asignación de recursos
  void assignSurgeon(const Patient &patient, int day);
  std::string assignOperatingTheater(const Patient &patient, int day);
  void assignRoom(const Patient &patient, int day, const std::string &room_id);

  // Operadores de reparación
  bool repairMandatoryPatient(const Patient &patient, EncodedPatientSolution &enc);
  bool repairOptionalPatient(const Patient &patient, EncodedPatientSolution &enc);

  // Cálculo de restricciones blandas
  int calculateSoftConstraints(); // Cálculo de la función objetivo (suma de penalizaciones)
  int calculateAgeGroupPenalty(); // Cálculo de S1
  int calculateMinimumSkillPenalty(); // Cálculo de S2
  int calculateContinuityOfCarePenalty(); // Cálculo de S3
  int calculateMaximumWorkloadPenalty(); // Cálculo de S4
  int calculateOpenOperatingTheatersPenalty(); // Cálculo de S5
  int calculateSurgeonTransferPenalty(); // Cálculo de S6
  int calculateAdmissionDelayPenalty(); // Cálculo de S7
  int calculateUnscheduledOptionalPatientsPenalty(); // Cálculo de S8
  // Calculo de restricciones duras
  int calculateHardConstraintViolations();
};

#endif // SOLVER_H
