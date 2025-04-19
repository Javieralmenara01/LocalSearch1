/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 *
 * @author Javier Almenara Herrera
 * @brief Implementación de clase que representa una solución al problema de asignación hospitalaria.
 * @file MetaHeuristicSolver.cc
 */

#include "MetaHeuristicSolver.h"
#include "Solver.h"
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <unordered_map>

/**
 * Constructor de la clase MetaHeuristicSolver.
 * 
 * @param problemInstance Instancia del problema a resolver.
 */
MetaHeuristicSolver::MetaHeuristicSolver(ProblemInstance &problemInstance)
    : problem(problemInstance), rng(std::random_device{}()) {}

/**
 * Genera una población inicial de soluciones codificadas.
 * 
 * @param populationSize Tamaño de la población a generar.
 * @return Población inicial de soluciones codificadas.
 */
std::vector<EncodedSolution> MetaHeuristicSolver::generateInitialPopulation(int populationSize) {
  std::vector<EncodedSolution> population;

  // Generar una lista de salas disponibles.
  std::vector<std::string> availableRooms;
  for (const auto &room : problem.rooms) {
    availableRooms.push_back(room.id);
  }

  // Índice para acceder como día al turno del problema
  std::unordered_map<std::string, int> shiftIndex;
  for (int i = 0; i < problem.shift_types.size(); ++i) {
    shiftIndex[problem.shift_types[i]] = i;
  }

  // Generar un vector de vectores para cada día y turno, con las enfermeras disponibles.
  std::vector<std::vector<std::string>> availableNurses(problem.days * problem.shift_types.size());
  for (const auto &nurse : problem.nurses) {
    for (const auto &work_day : nurse.working_shifts) {
      int day = work_day.day;
      int shift = shiftIndex[work_day.shift];
      availableNurses[day * problem.shift_types.size() + shift].push_back(nurse.id);
    }
  }

  for (int i = 0; i < populationSize; ++i) {

    // Paso 1. Generar una solución aleatoria de los pacientes para cada individuo de la población.
    std::vector<EncodedPatientSolution> encodedSolution;
    /// Alterar el orden de los pacientes para generar una solución aleatoria guardandolo en un vector.
    std::shuffle(problem.patients.begin(), problem.patients.end(), rng);
    for (const auto &patient : problem.patients) {
      // Seleccionar un día aleatorio para el paciente entre sus días disponibles.
      /// A) Si es un paciente obligatorio, seleccionar un día aleatorio entre surgery_release_day y surgery_due_day.
      /// B) Si no es un paciente obligatorio, seleccionar un día aleatorio entre surgery_release_day y el último día disponible.
      int day = patient.mandatory
                    ? std::uniform_int_distribution<int>(patient.surgery_release_day, patient.surgery_due_day)(rng)
                    : std::uniform_int_distribution<int>(patient.surgery_release_day, problem.days - 1)(rng);
      
      // Seleccionar una sala aleatoria para el paciente fuera de las lista de incompatible_rooms.
      std::vector<std::string> availableRooms;
      for (const auto &room : problem.rooms) {
        if (std::find(patient.incompatible_room_ids.begin(), patient.incompatible_room_ids.end(), room.id) == patient.incompatible_room_ids.end()) {
          availableRooms.push_back(room.id);
        }
      }
      std::string room = availableRooms[std::uniform_int_distribution<int>(0, availableRooms.size() - 1)(rng)];

      encodedSolution.push_back({patient.id, day, room});
    }

    // Paso 2. Generar una solución aleatoria para las enfermeras.
    std::vector<std::vector<std::string>> nurseAssignments;
    std::vector<std::vector<std::string>> availableRoomsPerDay(problem.days * problem.shift_types.size(), availableRooms);
    std::vector<std::vector<std::string>> availableNursesAux = availableNurses;
    
    for (const auto &nurse : problem.nurses) {
      for (const auto &work_day : nurse.working_shifts) {
        int day = work_day.day;
        int shift = shiftIndex[work_day.shift];
        int shiftSlot = day * problem.shift_types.size() + shift;
        // Quitar a la enfermera actual de la lista de enfermeras disponibles para este turno.
        auto& nursesAvailable = availableNursesAux[shiftSlot];
        auto it = std::find(nursesAvailable.begin(), nursesAvailable.end(), nurse.id);
        if (it != nursesAvailable.end()) {
          nursesAvailable.erase(it);
        }
        int numNursesRemaining = nursesAvailable.size();

        // Check if no rooms are available for this shift to avoid segmentation fault.
        if (availableRoomsPerDay[shiftSlot].empty()) {
          nurseAssignments.push_back({});
          continue;
        }

        // Determinar cuántas salas asignar: si es la última enfermera disponible para el turno, se le asignan todas las salas restantes; de lo contrario, un número aleatorio entre 1 y el total de salas disponibles.
        int maxRooms = availableRoomsPerDay[shiftSlot].size();
        int numRoomsToAssign = (numNursesRemaining == 0) ? maxRooms 
                                : std::uniform_int_distribution<int>(1, maxRooms)(rng);
        std::vector<std::string> roomsForNurse;
        for (int j = 0; j < numRoomsToAssign; ++j) {
          std::uniform_int_distribution<int> roomDist(0, availableRoomsPerDay[shiftSlot].size() - 1);
          int roomIndex = roomDist(rng);
          roomsForNurse.push_back(availableRoomsPerDay[shiftSlot][roomIndex]);
          availableRoomsPerDay[shiftSlot].erase(availableRoomsPerDay[shiftSlot].begin() + roomIndex);
        }
        nurseAssignments.push_back(roomsForNurse);
      }
    }
    population.push_back({encodedSolution, nurseAssignments});
  }
  return population;
}


/**
 * Algoritmo genético para resolver el problema de asignación hospitalaria.
 * 
 * @param populationSize Tamaño de la población.
 * @return Tupla con la mejor solución encontrada y su fitness.
 */
std::tuple<std::vector<EncodedPatientSolution>, int> MetaHeuristicSolver::geneticAlgorithm(int populationSize) {
  // Parámetros del algoritmo
  const int tournamentSize = 3;                         // Tamaño del torneo para la selección
  const double crossoverRate = 0.3;                       // Parámetro de cruce
  const auto maxDuration = std::chrono::minutes(60);      // Tiempo máximo de ejecución
  const int eliteCount = 10;                              // Número de soluciones elite a retener

  // Tiempo de inicio
  auto startTime = std::chrono::steady_clock::now();

  // Generar población inicial
  std::vector<EncodedSolution> population = generateInitialPopulation(populationSize);

  // Variables para la mejor solución global
  EncodedSolution globalBestSolution = population[0];
  std::pair<int,int> globalBestFitness = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };

  // Mutex para proteger el acceso a la solución global
  std::mutex bestMutex;

  // Evaluar fitness de la población inicial usando hasta 4 hilos concurrentes
  std::vector<std::pair<int,int>> fitnesses(population.size());
  for (size_t i = 0; i < population.size(); i += 4) {
    std::vector<std::thread> threads;
    size_t end = std::min(population.size(), i + static_cast<size_t>(4));
    for (size_t j = i; j < end; ++j) {
      threads.emplace_back([&, j]() {
        Solver solver(problem);
        auto [hardViolations, softPenalty] = solver.localSearch(population[j]);
        fitnesses[j] = { hardViolations, softPenalty };
        // Actualizar la mejor solución global de forma segura
        std::lock_guard<std::mutex> lock(bestMutex);
        if (hardViolations < globalBestFitness.first ||
            (hardViolations == globalBestFitness.first && softPenalty < globalBestFitness.second)) {
          globalBestFitness = { hardViolations, softPenalty };
          globalBestSolution = population[j];
          // Se elimina la exportación inmediata para evitar inconsistencias:
          // solver.exportSolution("bestSolution.json");
        }
      });
    }
    for (auto &t : threads) {
      t.join();
    }
  }

  std::uniform_real_distribution<double> probDist(0.0, 1.0);
  int generation = 0;

  // Lambda para selección por torneo
  auto tournamentSelection = [&](const std::vector<EncodedSolution>& pop,
      const std::vector<std::pair<int,int>>& fit) -> int {
    int bestIndex = -1;
    std::pair<int,int> bestFitness = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
    std::uniform_int_distribution<int> dist(0, pop.size() - 1);
    for (int i = 0; i < tournamentSize; ++i) {
      int idx = dist(rng);
      if (fit[idx].first < bestFitness.first ||
      (fit[idx].first == bestFitness.first && fit[idx].second < bestFitness.second)) {
        bestFitness = fit[idx];
        bestIndex = idx;
      }
    }
    return bestIndex;
  };


  // Bucle principal del algoritmo genético (máximo 10 minutos)
  while (std::chrono::steady_clock::now() - startTime < maxDuration) {
    // Elitismo: conservar los 'eliteCount' mejores individuos
    std::vector<int> indices(population.size());
    for (size_t i = 0; i < population.size(); ++i) {
      indices[i] = i;
    }
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
      if (fitnesses[a].first != fitnesses[b].first)
        return fitnesses[a].first < fitnesses[b].first;
      else
        return fitnesses[a].second < fitnesses[b].second;
    });

    std::vector<EncodedSolution> newPopulation;
    for (int i = 0; i < eliteCount && i < static_cast<int>(indices.size()); ++i) {
      newPopulation.push_back(population[indices[i]]);
    }

    // Generar nuevos individuos hasta completar la población
    while (newPopulation.size() < populationSize) {
      int idx1 = tournamentSelection(population, fitnesses);
      int idx2 = tournamentSelection(population, fitnesses);
      while (idx2 == idx1 && population.size() > 1) {
        idx2 = tournamentSelection(population, fitnesses);
      }
      const EncodedSolution &parent1 = population[idx1];
      const EncodedSolution &parent2 = population[idx2];

      double r = probDist(rng);
      EncodedSolution offspring;
      if (r < crossoverRate) {
        // Aplicar cruce a la parte de pacientes y enfermeras
        std::vector<EncodedPatientSolution> offspringPatients =
          pmxCrossoverPatients(parent1.encoded_patients, parent2.encoded_patients);
        std::vector<std::vector<std::string>> offspringNurses =
          pmxCrossoverNurses(parent1.encoded_nurses, parent2.encoded_nurses);
        offspring = { offspringPatients, offspringNurses };
      } else {
        // Sin cruce: clonar el primer padre
        offspring = parent1;
      }

      // Aplicar mutación a cada parte
      mutatePatients(offspring.encoded_patients);
      mutateNurses(offspring.encoded_nurses);

      newPopulation.push_back(offspring);
    }

    // Actualizar población y re-evaluar fitness usando hasta 4 hilos concurrentes
    population = newPopulation;
    fitnesses.clear();
    fitnesses.resize(population.size());
    for (size_t i = 0; i < population.size(); i += 4) {
      std::vector<std::thread> threads;
      size_t end = std::min(population.size(), i + static_cast<size_t>(4));
      for (size_t j = i; j < end; ++j) {
        threads.emplace_back([&, j]() {
          Solver solver(problem);
          auto [hardViolations, softPenalty] = solver.localSearch(population[j]);
          fitnesses[j] = { hardViolations, softPenalty };
          // Actualizar la mejor solución global de forma segura
          std::lock_guard<std::mutex> lock(bestMutex);
          if (hardViolations < globalBestFitness.first ||
              (hardViolations == globalBestFitness.first && softPenalty < globalBestFitness.second)) {
            globalBestFitness = { hardViolations, softPenalty };
            globalBestSolution = population[j];
            // Se elimina la exportación inmediata:
            // solver.exportSolution("bestSolution.json");          
          }
        });
      }
      for (auto &t : threads) {
        t.join();
      }
    }

    // Imprimir información de la generación (se usa el mejor de la generación, es decir, indices[0])
    std::cout << "Generación: " << generation
              << " - Mejor global : " << globalBestFitness.first
              << ", " << globalBestFitness.second
              << std::endl;
    generation++;
  }

  std::cout << "Solución final: Hard violations = " << globalBestFitness.first
            << ", Soft penalty = " << globalBestFitness.second << std::endl;
  
  // Exportar la solución final de forma segura
  {
    std::lock_guard<std::mutex> lock(bestMutex);
    // Se crea un solver final para exportar la solución global obtenida.
    // Se asume que Solver tiene un método setSolution que establece la solución interna.
    Solver finalSolver(problem);
    finalSolver.solve(globalBestSolution);  
    finalSolver.exportSolution("bestSolution.json");
  }

  return { globalBestSolution.encoded_patients, globalBestFitness.second };
}

/**
 * Realiza el operador de cruce PMX asegurando que se apliquen restricciones
 * separadas a pacientes obligatorios y opcionales, evitando duplicados.
 */
std::vector<EncodedPatientSolution> MetaHeuristicSolver::pmxCrossoverPatients(const std::vector<EncodedPatientSolution>& parent1, const std::vector<EncodedPatientSolution>& parent2) {
  int size = parent1.size();
  std::vector<EncodedPatientSolution> offspring(size);

  std::uniform_int_distribution<int> dist(0, size - 1);
  int point1 = dist(rng);
  int point2 = dist(rng);
  if (point1 > point2) std::swap(point1, point2);

  std::unordered_map<std::string, EncodedPatientSolution> mappingP1toP2;
  std::unordered_map<std::string, EncodedPatientSolution> mappingP2toP1;

  // Copiar la sección intercambiada y establecer mapeo
  for (int i = point1; i <= point2; i++) {
    offspring[i] = parent2[i];
    mappingP1toP2[parent1[i].patient_id] = parent2[i];
    mappingP2toP1[parent2[i].patient_id] = parent1[i];
  }

  // Copiar los valores restantes del primer padre con corrección
  for (int i = 0; i < size; i++) {
    if (i >= point1 && i <= point2) continue;

    EncodedPatientSolution newSolution = parent1[i];
    while (mappingP2toP1.find(newSolution.patient_id) != mappingP2toP1.end()) {
      newSolution = mappingP2toP1[newSolution.patient_id];
    }
    
    offspring[i] = newSolution;
  }

  return offspring;
}

/**
 * Aplica el operador PMX sobre un conjunto de pacientes.
 * 
 * @param parent1 Primer padre.
 * @param parent2 Segundo padre.
 * @return Vector con la descendencia generada.
 */
std::vector<EncodedPatientSolution> MetaHeuristicSolver::applyPMXPatients(const std::vector<EncodedPatientSolution>& parent1, const std::vector<EncodedPatientSolution>& parent2) {
  
  int size = parent1.size();
  if (size == 0) return {}; // Si la subpoblación está vacía, retornamos vacío

  std::vector<EncodedPatientSolution> offspring(size);

  // Selección aleatoria de dos puntos de cruce
  std::uniform_int_distribution<int> dist(0, size - 1);
  int point1 = dist(rng);
  int point2 = dist(rng);

  if (point1 > point2) std::swap(point1, point2);

  // Mapas para corrección de mapeo
  std::unordered_map<std::string, std::string> mappingP1toP2;
  std::unordered_map<std::string, std::string> mappingP2toP1;

  // Copiar la sección intercambiada y establecer el mapeo
  for (int i = point1; i <= point2; i++) {
    offspring[i] = parent2[i];  // Copia desde el segundo padre
    mappingP1toP2[parent1[i].patient_id] = parent2[i].patient_id;
    mappingP2toP1[parent2[i].patient_id] = parent1[i].patient_id;
  }

  // Copiar los valores restantes del primer padre con corrección
  for (int i = 0; i < size; i++) {
    if (i >= point1 && i <= point2) continue; // Saltar la sección intercambiada

    std::string patient_id = parent1[i].patient_id;
    while (mappingP2toP1.find(patient_id) != mappingP2toP1.end()) {
      patient_id = mappingP2toP1[patient_id]; // Aplicar corrección de mapeo
    }

    offspring[i] = parent1[i];
    offspring[i].patient_id = patient_id; // Corregir ID si es necesario
  }

  return offspring;
}

/**
 * Operador de mutación: selecciona un paciente aleatorio y modifica su día de admisión en ±1 día dentro del rango permitido.
 * Se ejecuta en un bucle hasta encontrar una mutación válida.
 * 
 * @param individual Individuo a mutar.
 */
void MetaHeuristicSolver::mutatePatients(std::vector<EncodedPatientSolution>& individual) {
  
  if (individual.empty()) return; // No mutar si el individuo está vacío

  std::uniform_int_distribution<int> dist(0, individual.size() - 1);
  std::uniform_int_distribution<int> dayDist(-1, 1); // Cambio de -1 a +1 días

  bool mutated = false;
  while (!mutated) {
    int patientIndex = dist(rng);
    EncodedPatientSolution& selectedPatient = individual[patientIndex];

    // Buscar paciente real en la instancia del problema
    auto patientIt = std::find_if(problem.patients.begin(), problem.patients.end(),
                                  [&](const Patient& p) { return p.id == selectedPatient.patient_id; });

    if (patientIt != problem.patients.end()) {
      const Patient& realPatient = *patientIt;

      // Intentar modificar el día hasta que se encuentre un cambio válido
      for (int attempt = 0; attempt < 3; ++attempt) { // Máximo 3 intentos de mutación
        int delta = dayDist(rng);
        int newAdmissionDay = selectedPatient.admission_day + delta;

        // Si el paciente es obligatorio, el día debe estar dentro del rango de admisión obligatorio
        if (realPatient.mandatory) {
          if (newAdmissionDay >= realPatient.surgery_release_day && newAdmissionDay <= realPatient.surgery_due_day) {
            selectedPatient.admission_day = newAdmissionDay;
            mutated = true;
            break;
          }
        } else {
          if (newAdmissionDay >= realPatient.surgery_release_day && newAdmissionDay < problem.days) {
            selectedPatient.admission_day = newAdmissionDay;
            mutated = true;
            break;
          }
        }
        
      }
    }
  }
}

/**
 * Operador de cruce por bloques para las asignaciones de enfermeras.
 * Se seleccionan dos puntos de corte y se copia el bloque del segundo padre,
 * mientras que fuera del bloque se conserva la asignación del primer padre.
 *
 * @param parent1 Primer padre (vector de vectores de asignaciones).
 * @param parent2 Segundo padre.
 * @return Descendencia resultante (vector de vectores de asignaciones).
 */
std::vector<std::vector<std::string>> MetaHeuristicSolver::pmxCrossoverNurses(const std::vector<std::vector<std::string>>& parent1, const std::vector<std::vector<std::string>>& parent2) {
  int size = parent1.size();
  std::vector<std::vector<std::string>> offspring(size);
  
  std::uniform_int_distribution<int> dist(0, size - 1);
  int point1 = dist(rng);
  int point2 = dist(rng);
  if (point1 > point2) std::swap(point1, point2);
  
  // Se copia el bloque intermedio del segundo padre
  for (int i = 0; i < size; i++) {
    if (i >= point1 && i <= point2) {
      offspring[i] = parent2[i];
    } else {
      offspring[i] = parent1[i];
    }
  }
  return offspring;
}

/**
 * Operador de mutación para las asignaciones de enfermeras, que realiza
 * o bien una eliminación o una inserción en un bloque aleatorio (turno).
 *
 * En el caso de eliminación, se elimina una sala asignada (si existe).
 * En el caso de inserción, se selecciona una sala aleatoria de problem.rooms y
 * se inserta en una posición aleatoria del bloque.
 *
 * @param nurseAssignments Vector de asignaciones (por turno) de enfermeras.
 */
void MetaHeuristicSolver::mutateNurses(std::vector<std::vector<std::string>>& nurseAssignments) {
  if (nurseAssignments.empty()) return;
  
  // Total de bloques (cada bloque es el turno de una enfermera)
  int totalBlocks = nurseAssignments.size();
  std::uniform_int_distribution<int> blockDist(0, totalBlocks - 1);
  int blockAIndex = blockDist(rng);

  // Decodificar blockA: determinar a qué enfermera y turno corresponde
  int currentIndex = 0;
  int nurseA = -1, shiftA = -1;
  for (int n = 0; n < problem.nurses.size(); n++) {
    int numShifts = problem.nurses[n].working_shifts.size();
    for (int s = 0; s < numShifts; s++) {
      if (currentIndex == blockAIndex) {
        nurseA = n;
        shiftA = s;
        break;
      }
      currentIndex++;
    }
    if (nurseA != -1) break;
  }
  if (nurseA == -1) return; // No debería ocurrir

  // Obtener los detalles del turno seleccionado (día y tipo de turno)
  int dayA = problem.nurses[nurseA].working_shifts[shiftA].day;
  std::string shiftTypeA = problem.nurses[nurseA].working_shifts[shiftA].shift;

  // Buscar bloques candidatos: aquellos (distintos de blockA) que correspondan al mismo día y turno
  std::vector<int> candidateBlocks;
  currentIndex = 0;
  for (int n = 0; n < problem.nurses.size(); n++) {
    int numShifts = problem.nurses[n].working_shifts.size();
    for (int s = 0; s < numShifts; s++) {
      if (currentIndex != blockAIndex) {
        int day = problem.nurses[n].working_shifts[s].day;
        std::string shiftType = problem.nurses[n].working_shifts[s].shift;
        if (day == dayA && shiftType == shiftTypeA) {
          candidateBlocks.push_back(currentIndex);
        }
      }
      currentIndex++;
    }
  }

  if (candidateBlocks.empty()) return; // No hay turno similar en otra enfermera

  // Seleccionar aleatoriamente uno de los bloques candidatos (blockB)
  std::uniform_int_distribution<int> candDist(0, candidateBlocks.size() - 1);
  int blockBIndex = candidateBlocks[candDist(rng)];

  // Referencias a los bloques a mutar
  std::vector<std::string>& blockA = nurseAssignments[blockAIndex];
  std::vector<std::string>& blockB = nurseAssignments[blockBIndex];

  // Solo se realiza la mutación si ambos bloques tienen al menos una habitación asignada
  if (blockA.empty() || blockB.empty()) return;

  // Seleccionar aleatoriamente una habitación de cada bloque
  std::uniform_int_distribution<int> roomDistA(0, blockA.size() - 1);
  std::uniform_int_distribution<int> roomDistB(0, blockB.size() - 1);
  int indexA = roomDistA(rng);
  int indexB = roomDistB(rng);
  std::string roomA = blockA[indexA];
  std::string roomB = blockB[indexB];

  // Comprobar que al intercambiar no se duplique: 
  // En blockA, roomB no debe estar ya (aparte de la posición que se va a sustituir)
  // En blockB, roomA no debe estar ya (aparte de la posición que se va a sustituir)
  bool duplicateInA = false;
  for (int i = 0; i < blockA.size(); i++) {
    if (i != indexA && blockA[i] == roomB) {
      duplicateInA = true;
      break;
    }
  }
  bool duplicateInB = false;
  for (int j = 0; j < blockB.size(); j++) {
    if (j != indexB && blockB[j] == roomA) {
      duplicateInB = true;
      break;
    }
  }
  if (duplicateInA || duplicateInB) {
    // Si el intercambio produciría duplicados, abortamos la mutación.
    return;
  }

  // Realizar el intercambio
  std::swap(blockA[indexA], blockB[indexB]);
}

/**
 * Selecciona dos padres de la población mediante torneo.
 * 
 * @param population Población de soluciones.
 * @param fitnesses Fitness de cada solución.
 * @return Par de padres seleccionados.
 */
// std::pair<std::vector<EncodedPatientSolution>, std::vector<EncodedPatientSolution>> MetaHeuristicSolver::selectParents(const std::vector<std::vector<EncodedPatientSolution>>& population, const std::vector<std::pair<int, int>>& fitnesses) {

//   std::uniform_int_distribution<int> dist(0, population.size() - 1);

//   auto tournamentSelect = [&]() -> int {
//     const int tournamentSize = 3; // Tamaño del torneo (ajustable)
//     int bestIndex = -1;
//     std::pair<int, int> bestFitness = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};

//     for (int i = 0; i < tournamentSize; ++i) {
//       int idx = dist(rng);

//       // Verificar si el individuo actual es mejor que el mejor encontrado
//       if (fitnesses[idx].first < bestFitness.first || 
//          (fitnesses[idx].first == bestFitness.first && fitnesses[idx].second < bestFitness.second)) {
//         bestFitness = fitnesses[idx];
//         bestIndex = idx;
//       }
//     }

//     return (bestIndex != -1) ? bestIndex : dist(rng);  // Si no encontró, elige al azar
//   };

//   int parent1 = tournamentSelect();
//   int parent2;
//   int attempts = 0;

//   do {
//     parent2 = tournamentSelect();
//     attempts++;
//     // Esto se podría comentar es complicado que de forma aleatoria se elija el mismo padre varias veces seguidas
//     if (attempts > 100) { // Máximo intentos para evitar bucles infinitos
//       break;
//     }
//   } while (parent2 == parent1);

//   return {population[parent1], population[parent2]};
// }
