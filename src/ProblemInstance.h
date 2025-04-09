/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 * 
 * @author Javier Almenara Herrera
 * @brief Declaración de clase que representa una instancia completa del problema IHTP.
 * @file ProblemInstance.h
 */

#ifndef PROBLEM_INSTANCE_H
#define PROBLEM_INSTANCE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Functions.h"

/**
 * Clase que representa una instancia completa del problema.
 */
class ProblemInstance {
 public:
  // Datos inherentes al problema
  int days;                                // Número de días
  int skill_levels;                        // Niveles de habilidad
  std::vector<std::string> shift_types;    // Tipos de turno (early, late, night)
  std::vector<std::string> age_groups;     // Grupos de edad
  std::vector<Occupant> occupants;         // Ocupantes
  std::vector<Patient> patients;           // Pacientes
  std::vector<Surgeon> surgeons;           // Cirujanos
  std::vector<OperatingTheater> theaters;  // Quirófanos
  std::vector<Room> rooms;                 // Habitaciones
  std::vector<Nurse> nurses;               // Enfermeras
  Weights weights;                         // Pesos para restricciones blandas
  // Métodos
  void loadFromJSON(const std::string& filename);
  void printSummary() const;
  
  // Método de asignar
  void operator=(const ProblemInstance& other);
};

#endif // PROBLEM_INSTANCE_H