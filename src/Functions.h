/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 * 
 * @author Javier Almenara Herrera
 * @brief Declaración de funciones auxiliares para el problema de asignación hospitalaria IHTP. 
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "Structs.h"
#include "ProblemInstance.h"

using json = nlohmann::json;

/**
 * Carga los ocupantes desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de ocupantes.
 */
std::vector<Occupant> loadOccupants(const json& data);

/**
 * Carga los pacientes desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de pacientes.
 */
std::vector<Patient> loadPatients(const json& data);

/**
 * Carga los cirujanos desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de cirujanos.
 */
std::vector<Surgeon> loadSurgeons(const json& data);

/**
 * Carga los quirófanos desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de quirófanos.
 */
std::vector<OperatingTheater> loadOperatingTheaters(const json& data);

/**
 * Carga las habitaciones desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de habitaciones.
 */
std::vector<Room> loadRooms(const json& data);

/**
 * Carga las enfermeras desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de enfermeras.
 */
std::vector<Nurse> loadNurses(const json& data);

/**
 * Carga los pesos desde JSON.
 * 
 * @param data Objeto JSON que contiene los datos de los pesos.
 * @return Estructura `Weights` con los pesos cargados.
 */
Weights loadWeights(const json& data);

#endif // FUNCTIONS_H