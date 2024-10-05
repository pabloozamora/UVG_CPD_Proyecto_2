#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <mpi.h>  // Biblioteca para MPI

// Simulación de una función de cifrado/descifrado DES (esto debería reemplazarse por una implementación real o una librería)
std::string encrypt(const std::string& key, const std::string& plaintext) {
    return key + plaintext;  // Simulación simple (esto debe ser reemplazado por DES real)
}

std::string decrypt(const std::string& key, const std::string& ciphertext) {
    return ciphertext.substr(key.size());  // Simulación (esto debe ser reemplazado por DES real)
}

// Función de fuerza bruta para encontrar la clave
bool tryKey(const std::string& key, const std::string& ciphertext, const std::string& searchPhrase) {
    std::string decrypted = decrypt(key, ciphertext);  // Desencriptar el texto usando la clave
    return decrypted.find(searchPhrase) != std::string::npos;  // Verificar si la frase clave está en el texto descifrado
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Inicializa el entorno MPI

    int world_size;  // Número total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;  // Identificador del proceso actual (rank)
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Texto original y palabra clave que se busca
    std::string plaintext = "Esta es una prueba de proyecto 2";
    std::string searchPhrase = "es una prueba de";

    // Generar el texto cifrado (simulado)
    std::string correctKey = "123456";  // Esta es la clave correcta que se intentará encontrar
    std::string ciphertext = encrypt(correctKey, plaintext);

    // Definir los límites de la búsqueda de claves
    long long total_keys = 1000000;  // Número total de claves a probar
    long long keys_per_process = total_keys / world_size;  // Cuántas claves prueba cada proceso
    long long start_key = world_rank * keys_per_process;  // Clave inicial para este proceso
    long long end_key = (world_rank == world_size - 1) ? total_keys : start_key + keys_per_process;  // Clave final

    // Variables para medir el tiempo de ejecución
    std::clock_t start;
    double duration;
    if (world_rank == 0) {
        start = std::clock();  // Solo el proceso 0 mide el tiempo
    }

    // Búsqueda de la clave por fuerza bruta en paralelo
    std::string found_key = "";
    bool key_found = false;
    for (long long i = start_key; i < end_key; ++i) {
        std::string currentKey = std::to_string(i);
        while (currentKey.size() < 6) {
            currentKey = "0" + currentKey;
        }

        if (tryKey(currentKey, ciphertext, searchPhrase)) {
            found_key = currentKey;
            key_found = true;
            break;
        }
    }

    // Si algún proceso encuentra la clave, la comunica al proceso 0
    bool global_key_found = false;
    MPI_Allreduce(&key_found, &global_key_found, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);

    if (key_found) {
        MPI_Send(found_key.c_str(), found_key.size() + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);  // Enviar la clave encontrada al proceso 0
    }

    // El proceso 0 recibe la clave encontrada y muestra el resultado
    if (world_rank == 0) {
        if (global_key_found) {
            char received_key[10];
            MPI_Status status;
            MPI_Recv(received_key, 10, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            std::cout << "Clave encontrada: " << received_key << std::endl;
        } else {
            std::cout << "Clave no encontrada" << std::endl;
        }

        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
        std::cout << "Tiempo de ejecución: " << duration << " segundos" << std::endl;
    }

    MPI_Finalize();  // Finaliza el entorno MPI
    return 0;
}
