/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Trabajo Fin de Grado
 * 
 * @author Javier Almenara Herrera
 * @brief Definición de funciones auxiliares para el problema de asignación hospitalaria IHTP. 
 */

#include "Functions.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <set>

/**
 * Carga los ocupantes desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de ocupantes.
 */
std::vector<Occupant> loadOccupants(const json& data) {
  std::vector<Occupant> occupants;
  for (const auto& o : data["occupants"]) {
    occupants.push_back({
      o["id"],
      o["gender"],
      o["age_group"],
      o["length_of_stay"],
      o["workload_produced"].get<std::vector<int>>(),
      o["skill_level_required"].get<std::vector<int>>(),
      o["room_id"]
    });
  }
  return occupants;
}

/**
 * Carga los pacientes desde un archivo JSON.
 * 
 * @param data Datos en formato JSON.
 * @return Vector de pacientes.
 */
std::vector<Patient> loadPatients(const json& data) {
  std::vector<Patient> patients;
  for (const auto& p : data["patients"]) {
    // Leer valores básicos
    std::string id = p["id"];
    bool mandatory = p["mandatory"];
    std::string gender = p["gender"];
    std::string age_group = p["age_group"];
    int length_of_stay = p["length_of_stay"];
    int surgery_release_day = p["surgery_release_day"];
    int surgery_duration = p["surgery_duration"];
    std::string surgeon_id = p["surgeon_id"];
    std::vector<std::string> incompatible_room_ids = p["incompatible_room_ids"].get<std::vector<std::string>>();
    std::vector<int> workload_produced = p["workload_produced"].get<std::vector<int>>();
    std::vector<int> skill_level_required = p["skill_level_required"].get<std::vector<int>>();

    // Manejar `surgery_due_day` condicionalmente
    int surgery_due_day = mandatory ? p.value("surgery_due_day", -1) : -1;

    // Crear el objeto Patient explícitamente
    Patient patient = {
      id,
      mandatory,
      gender,
      age_group,
      length_of_stay,
      surgery_release_day,
      surgery_due_day,
      surgery_duration,
      surgeon_id,
      incompatible_room_ids,
      workload_produced,
      skill_level_required
    };

    // Añadir el paciente al vector
    patients.push_back(patient);
  }
  return patients;
}

/**
 * Función para cargar cirujanos desde JSON.
 * 
 * @param data Objeto JSON que contiene los datos de los cirujanos.
 * @return Vector de cirujanos cargados desde el JSON.
 */
std::vector<Surgeon> loadSurgeons(const json& data) {
  std::vector<Surgeon> surgeons;
  for (const auto& s : data["surgeons"]) {
    surgeons.push_back({
      s["id"],
      s["max_surgery_time"].get<std::vector<int>>()
    });
  }
  return surgeons;
}

/**
 * Función para cargar quirófanos desde JSON.
 * 
 * @param data Objeto JSON que contiene los datos de los quirófanos.
 * @return Vector de quirófanos cargados desde el JSON.
 */
std::vector<OperatingTheater> loadOperatingTheaters(const json& data) {
  std::vector<OperatingTheater> theaters;
  for (const auto& t : data["operating_theaters"]) {
    theaters.push_back({
      t["id"],                                
      t["availability"].get<std::vector<int>>()
    });
  }
  return theaters;
}

/**
 * Función para cargar habitaciones desde JSON.
 * 
 * @param data Objeto JSON que contiene los datos de las habitaciones.
 * @return Vector de habitaciones cargadas desde el JSON.
 */
std::vector<Room> loadRooms(const json& data) {
  std::vector<Room> rooms;
  for (const auto& r : data["rooms"]) {
    rooms.push_back({
      r["id"],
      r["capacity"]
    });
  }
  return rooms;
}

/**
 * Función para cargar enfermeras desde JSON.
 * 
 * @param data Objeto JSON que contiene los datos de las enfermeras.
 * @return Vector de enfermeras cargadas desde el JSON.
 */
std::vector<Nurse> loadNurses(const json& data) {
  std::vector<Nurse> nurses;
  for (const auto& n : data["nurses"]) {
    // Cargar turnos de trabajo
    std::vector<Nurse::WorkingShift> shifts;
    for (const auto& s : n["working_shifts"]) {
      shifts.push_back({
        s["day"],
        s["shift"],
        s["max_load"]
      });
    }
    // Crear la enfermera
    nurses.push_back({
      n["id"],
      n["skill_level"],
      shifts
    });
  }
  return nurses;
}

/**
 * Función para cargar los pesos desde JSON.
 * 
 * @param data Objeto JSON que contiene los datos de los pesos.
 * @return Estructura `Weights` con los pesos cargados.
 */
Weights loadWeights(const json& data) {
  const auto& weightsData = data["weights"];
  return {
    weightsData["room_mixed_age"],
    weightsData["room_nurse_skill"],
    weightsData["continuity_of_care"],
    weightsData["nurse_eccessive_workload"],
    weightsData["open_operating_theater"],
    weightsData["surgeon_transfer"],
    weightsData["patient_delay"],
    weightsData["unscheduled_optional"]
  };
}