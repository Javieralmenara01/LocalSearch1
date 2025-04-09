/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 * 
 * @author Javier Almenara Herrera
 * @brief Implementación de clase que representa una instancia completa del problema IHTP.
 * @file ProblemInstance.cc
 */

#include <nlohmann/json.hpp>
#include "ProblemInstance.h"

using json = nlohmann::json;

/**
 * Operador de asignación.
 * 
 * @param other Instancia del problema a copiar.
 */
void ProblemInstance::operator=(const ProblemInstance& other) {
  days = other.days;
  skill_levels = other.skill_levels;
  shift_types = other.shift_types;
  age_groups = other.age_groups;
  occupants = other.occupants;
  patients = other.patients;
  surgeons = other.surgeons;
  theaters = other.theaters;
  rooms = other.rooms;
  nurses = other.nurses;
  weights = other.weights;
}

/**
 * Cargar datos de la instancia desde un archivo JSON.
 * 
 * @param filename Nombre del archivo JSON.
 */
void ProblemInstance::loadFromJSON(const std::string& filename) {
  // Abrir el archivo JSON
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("No se pudo abrir el archivo JSON.");
  }
  
  // Parsear el JSON
  json data;
  file >> data;

  // Cargar datos
  occupants = loadOccupants(data);
  patients = loadPatients(data);
  surgeons = loadSurgeons(data);
  theaters = loadOperatingTheaters(data);
  rooms = loadRooms(data);
  nurses = loadNurses(data);
  weights = loadWeights(data);

  // Cargar datos inherentes al problema
  days = data["days"];
  skill_levels = data["skill_levels"];
  shift_types = data["shift_types"].get<std::vector<std::string>>();
  age_groups = data["age_groups"].get<std::vector<std::string>>();
}

/**
 * Mostrar un resumen de los datos cargados.
 */
void ProblemInstance::printSummary() const {
  std::cout << "Resumen de la instancia del problema:" << std::endl;
  std::cout << "  Días: " << days << std::endl;
  std::cout << "  Niveles de habilidad: " << skill_levels << std::endl;
  std::cout << "  Tipos de turno: ";
  for (const std::string& shift : shift_types) {
    std::cout << shift << " ";
  }
  std::cout << std::endl;
  std::cout << "  Grupos de edad: ";
  for (const std::string& age : age_groups) {
    std::cout << age << " ";
  }
  std::cout << std::endl;
  std::cout << "  Ocupantes: " << occupants.size() << std::endl;
  std::cout << "  Pacientes: " << patients.size() << std::endl;
  std::cout << "  Cirujanos: " << surgeons.size() << std::endl;
  std::cout << "  Quirófanos: " << theaters.size() << std::endl;
  std::cout << "  Habitaciones: " << rooms.size() << std::endl;
  std::cout << "  Enfermeras: " << nurses.size() << std::endl; 
  std::cout << "  Pesos para restricciones blandas: " << std::endl 
            << "    Mezcla de edades en habitación: " << weights.room_mixed_age << std::endl
            << "    Nivel de habilidad de enfermera: " << weights.room_nurse_skill << std::endl
            << "    Continuidad de cuidado: " << weights.continuity_of_care << std::endl
            << "    Exceso de trabajo de enfermera: " << weights.nurse_eccessive_workload << std::endl
            << "    Quirófano abierto: " << weights.open_operating_theater << std::endl
            << "    Tiempo extra del cirujano: " << weights.surgeon_transfer << std::endl
            << "    Retraso del paciente: " << weights.patient_delay << std::endl
            << "    Cirugía no programada: " << weights.unscheduled_optional << std::endl;
}