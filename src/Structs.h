/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 * 
 * @author Javier Almenara Herrera
 * @brief Estructuras de datos para el problema de asignación hospitalaria IHTP.
 * @file Structs.h 
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <string>
#include <sstream>
#define JSON_USE_ORDERED_MAP
#include <nlohmann/json.hpp> // Biblioteca JSON

/**
 * Estructura que representa un ocupante.
 */
struct Occupant {
  std::string id;
  std::string gender;
  std::string age_group;
  int length_of_stay;
  std::vector<int> workload_produced;
  std::vector<int> skill_level_required;
  std::string room_id;
};

/**
 * Estructura que representa un paciente.
 */
struct Patient {
  std::string id;
  bool mandatory;
  std::string gender;
  std::string age_group;
  int length_of_stay;
  int surgery_release_day;
  int surgery_due_day;
  int surgery_duration;
  std::string surgeon_id;
  std::vector<std::string> incompatible_room_ids;
  std::vector<int> workload_produced;
  std::vector<int> skill_level_required;
};

/**
 * Estructura que representa un cirujano.
 */
struct Surgeon {
  std::string id;
  std::vector<int> max_surgery_time;
};

/**
 * Estructura que representa un quirófano.
 */
struct OperatingTheater {
  std::string id;
  std::vector<int> availability;
};

/**
 * Estructura que representa una habitación.
 */
struct Room {
  std::string id;
  int capacity;
};

/**
 * Estructura que representa una enfermera.
 */
struct Nurse {
  std::string id;
  int skill_level;
  struct WorkingShift {
    int day;
    std::string shift;
    int max_load;
  };
  std::vector<WorkingShift> working_shifts; // Turnos de trabajo
};

/**
 * Estructura que representa los pesos para las restricciones blandas.
 */
struct Weights {
  int room_mixed_age;          // Penalización por mezcla de edades en una habitación
  int room_nurse_skill;        // Penalización por nivel de habilidad de la enfermera inadecuado
  int continuity_of_care;      // Penalización por falta de continuidad en el cuidado
  int nurse_eccessive_workload; // Penalización por exceso de trabajo de la enfermera
  int open_operating_theater;  // Penalización por quirófano abierto
  int surgeon_transfer;        // Penalización por tiempo extra del cirujano
  int patient_delay;           // Penalización por retraso del paciente
  int unscheduled_optional;    // Penalización por cirugía no programada
};

/**
 * Estructura que una asignación de paciente.
 */
struct PatientAssignment {
  std::string id;                                // Identificador del paciente
  std::optional<int> admission_day;              // Día de admisión asignado; si no está asignado, será std::nullopt
  std::optional<std::string> room;               // Habitación asignada (opcional)
  std::optional<std::string> operating_theater;  // Quirófano asignado (opcional)
};

/**
 * Estructura que representa una asignación de enfermera.
 */
struct NurseAssignment {
  struct ShiftAssignment {
    int day;                   // Día del turno
    std::string shift;         // Turno 
    std::vector<std::string> rooms; // Habitaciones asignadas
  };
  std::string id;                      // Identificador de la enfermera
  std::vector<ShiftAssignment> assignments; // Asignaciones por turno
};

/**
 * Estructura que representa el estado de una habitación.
 */
struct RoomState {
  Room room_info; // Información estática de la habitación
  std::map<int, std::vector<Patient>> patients_per_day; // Día, pacientes asignados
  std::map<int, std::vector<Occupant>> occupants_per_day; // Ocupantes actuales
  std::map<int, int> capacity_per_day; // Capacidad restante por día 
};

/**
 * Estructura que representa el estado de un quirófano.
 */
struct OperatingTheaterState {
  OperatingTheater theater_info; // Información estática del quirófano
  std::map<int, std::vector<Patient>> patients_per_day; // Día, pacientes asignados
  std::map<int, int> availability_per_day; // Disponibilidad restante por día
};

/**
 * Estructura que representa una solución al problema de asignación hospitalaria.
 */
struct Solution {
  // Asignaciones
  std::vector<PatientAssignment> patients;
  std::vector<NurseAssignment> nurses;
  // Restricciones blandas
  std::vector<int> soft_constraints; // Valores de las restricciones blandas
  int total_soft_constraints;        // Suma de las restricciones blandas
  
  // Estructuras de control interno
  std::vector<RoomState> roomStates; // Estado dinámico de las habitaciones
  std::vector<OperatingTheaterState> theaterStates; // Estado dinámico de los quirófanos
  
  /**
   * Exporta la solución a un archivo JSON.
   * 
   * @param filename Nombre del archivo a guardar.
   */
  void exportToJSON(const std::string& filename) const {
    nlohmann::json jsonSolution;
    // Exportar pacientes
    for (const auto& patient : patients) {
      nlohmann::json patientJson;
      patientJson["id"] = patient.id;
      if (patient.admission_day.has_value()) {
        
        patientJson["admission_day"] = patient.admission_day.value();
        patientJson["room"] = patient.room.value();
        patientJson["operating_theater"] = patient.operating_theater.value();
      } else {
        patientJson["admission_day"] = "none";
        // No se agregan los campos "room" y "operating_theater"
      }
      jsonSolution["patients"].push_back(patientJson);
    }
    // Exportar enfermeras
    for (const auto& nurse : nurses) {
      nlohmann::json nurseJson = {{"id", nurse.id}, {"assignments", {}}};
      for (const auto& assignment : nurse.assignments) {
        nurseJson["assignments"].push_back({
            {"day", assignment.day},
            {"shift", assignment.shift},
            {"rooms", assignment.rooms},
        });
      }
      jsonSolution["nurses"].push_back(nurseJson);
    }
    // Exportar restricciones blandas en el formato requerido
    std::ostringstream softConstraintsStream;
    softConstraintsStream << "Cost: " << total_soft_constraints
                          << ", Unscheduled: " << soft_constraints[7]
                          << ", Delay: " << soft_constraints[6]
                          << ", OpenOT: " << soft_constraints[4]
                          << ", AgeMix: " << soft_constraints[0]
                          << ", Skill: " << soft_constraints[1]
                          << ", Excess: " << soft_constraints[3]
                          << ", Continuity: " << soft_constraints[2]
                          << ", SurgeonTransfer: " << soft_constraints[5];
    jsonSolution["costs"] = {softConstraintsStream.str()}; 
    // Guardar en el archivo
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
      throw std::runtime_error("No se pudo abrir el archivo para guardar la solución.");
    }
    outFile << jsonSolution.dump(2); // Formato bonito con indentación de 4 espacios
    outFile.close();
  }
};

/**
 * Estructura que representa una solución codificada de un paciente.
 */
struct EncodedPatientSolution {
  std::string patient_id;
  int admission_day;
  std::string room_id; // En primera aproximación se podría ignorar y asignar después.
};

/**
 * Estrucutra que representa la codificación de una solución.
 */
struct EncodedSolution {
  std::vector<EncodedPatientSolution> encoded_patients;
  std::vector<std::vector<std::string>> encoded_nurses;
};  

#endif // STRUCTS_H