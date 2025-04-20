#!/usr/bin/env python3
import subprocess
import os
import logging

# Configurar logging para consola y fichero
log_file_path = 'run_tests.log'
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s %(levelname)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    handlers=[
        logging.FileHandler(log_file_path, mode='w'),
        logging.StreamHandler()
    ]
)

def run_instance(instance_path, build_dir, validator_path, log_handle):
    """
    Ejecuta el ejecutable principal y luego el validador para una instancia dada,
    redirigiendo su salida al handle proporcionado.
    """
    main_executable = os.path.join(build_dir, 'main')
    best_solution = 'bestSolution.json'
    
    # Ejecutar el algoritmo metaheurístico
    logging.info(f"Ejecutando: {main_executable} {instance_path}")
    subprocess.run(
        [main_executable, instance_path],
        check=True,
        stdout=log_handle,
        stderr=log_handle
    )
    
    # Ejecutar el validador
    logging.info(f"Validando: {validator_path} {instance_path} {best_solution} -v")
    subprocess.run(
        [validator_path, instance_path, best_solution, '-v'],
        check=True,
        stdout=log_handle,
        stderr=log_handle
    )

def main():
    build_dir = 'build'
    validator_path = os.path.join('validator', 'IHTP_Validator')
    instances_dir = os.path.join('instances', 'ihtc2024_test_dataset')
    
    # Abrimos el fichero de log en modo escritura
    with open(log_file_path, 'w') as log_handle:
        # Iterar de test01.json a test10.json
        for i in range(1, 11):
            instance_file = f'test{str(i).zfill(2)}.json'
            instance_path = os.path.join(instances_dir, instance_file)
            
            # Ejecutar 2 veces por instancia
            for run_number in range(1, 3):
                logging.info(f"Instancia: {instance_file} (ejecución {run_number}/2)")
                run_instance(instance_path, build_dir, validator_path, log_handle)

if __name__ == '__main__':
    main()
