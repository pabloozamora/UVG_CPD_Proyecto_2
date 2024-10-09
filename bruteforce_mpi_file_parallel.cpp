#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <mpi.h>
#include "DESCrypt.h"
#include <sstream>
#include <omp.h>

// Función que intenta descifrar el texto cifrado con una clave dada
int tryKey(const std::string keyStr, std::vector<unsigned char> cipher, const std::string& search) {
    DESCrypt desCrypt(keyStr);
    std::string decryptedText;

    try {
        decryptedText = desCrypt.decrypt(cipher);
    } catch (const std::runtime_error& e) {
        return 0;  // La clave no es válida, retorna 0
    }

    return decryptedText.find(search) != std::string::npos;
}

// Convierte un vector de int a unsigned char
std::vector<unsigned char> convertToUChar(const std::vector<int>& intVec) {
    std::vector<unsigned char> ucharVec;
    ucharVec.reserve(intVec.size()); // Reservar espacio para mejorar la eficiencia

    for (int i : intVec) {
        ucharVec.push_back(static_cast<unsigned char>(i)); // Convertir y añadir al vector
    }

    return ucharVec;
}

// Función para leer el archivo y convertir a vector de unsigned char
std::string readPlainTextFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string plainText;

    if (file.is_open()) {
        // Lee todo el contenido del archivo
        std::getline(file, plainText, '\0');  // Leer todo el archivo en una sola línea
        file.close();
    } else {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);  // Aborta la ejecución si no se puede leer el archivo
    }

    return plainText;
}

// Convierte una cadena de valores ASCII a una clave
std::string asciiToString(const std::string& asciiValues) {
    std::stringstream ss(asciiValues);
    std::string item;
    std::string result;

    while (std::getline(ss, item, ',')) {
        char c = static_cast<char>(std::stoi(item));
        result += c;
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>" << std::endl;
        return 1;
    }

    std::string plainTextFile = argv[1];  // Primer argumento: nombre del archivo de texto original
    std::string asciiKey = argv[2];       // Segundo argumento: clave ASCII separada por comas
    std::string search = argv[3];         // Tercer argumento: patrón de búsqueda


    int num_threads = 4; // Número de hilos por proceso
    omp_set_num_threads(num_threads);

    int N, id;
    long upper = (1L << 56);  // Límite superior para claves DES (2^56)
    long mylower, myupper;

    // Tags de mensajes
    int keyFindedTag = 1;
    int rankFinishedTag = 2;
    
    MPI_Request req;
    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &N);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // Cifrar el mensaje
    std::string plainText = readPlainTextFromFile(plainTextFile);
    std::string key = asciiToString(asciiKey);

    DESCrypt desCrypt(key);
    std::vector<unsigned char> cipherText = desCrypt.encrypt(plainText);

    // Mostrar la clave y el texto cifrado solo en el proceso 0
    if (id == 0) {
        std::cout << "Llave (ASCII): " << asciiKey << " <" << key << ">" << std::endl;
        std::cout << "Texto original: " << plainText << std::endl;
        std::cout << "Texto cifrado (enteros): ";
        for (unsigned char c : cipherText) {
            std::cout << static_cast<int>(c) << ", ";
        }
        std::cout << std::endl;
    }

    // Cálculo del rango de claves para cada nodo
    long range_per_node = upper / N;
    mylower = range_per_node * id;
    myupper = range_per_node * (id + 1);
    std::cout << "Nodo: " << id << ", lower: " << mylower << ", upper: " << myupper << std::endl;
    if (id == N - 1) {
        myupper = upper;  // Compensar el residuo
    }

    long found = -1;  // -1: no se ha encontrado
    MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, keyFindedTag, MPI_COMM_WORLD, &req);

    // Búsqueda de la clave mediante fuerza bruta usando OpenMP
    #pragma omp parallel shared(found)
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // Cálculo del rango de iteraciones para cada hilo
        long range_per_thread = (myupper - mylower) / num_threads;
        long thread_lower = mylower + range_per_thread * thread_id;
        long thread_upper = (thread_id == num_threads - 1) ? myupper : (thread_lower + range_per_thread);

        for (long i = thread_lower; i < thread_upper; ++i) {

            // Verificar si se ha encontrado la llave
            #pragma omp flush(found)
            if (found != -1) {
                break;  // Salir del bucle si se encontró la llave
            }

            if (i % 1000000 == 0) {
                #pragma omp critical
                std::cout << "Rank " << id << ", iteración " << i << std::endl;
            }

            std::string keyStr(8, '\0');
            for (int j = 0; j < 8; ++j) {
                keyStr[j] = (i >> (j * 8)) & 0xFF;  // Extrae cada byte de la clave
            }

            if (tryKey(keyStr, cipherText, search)) {
                #pragma omp critical
                {
                    if (found == -1) {
                        found = i;
                        std::cout << "Key encontrada en rank " << id << " i=" << found << std::endl;
                        // Notifica a todos los nodos que se encontró la clave
                        for (int node = 0; node < N; node++) {
                            MPI_Isend(&found, 1, MPI_LONG, node, keyFindedTag, MPI_COMM_WORLD, &req);
                        }
                    }
                }
                #pragma omp flush(found)
            }

            if (found == -1) {
                // Verificar si otro nodo ha encontrado la clave
                int messageReceived;
                MPI_Test(&req, &messageReceived, MPI_STATUS_IGNORE);
                if (messageReceived) {
                    #pragma omp critical
                    std::cout << "Key encontrada en otro nodo, saliendo del bucle " << id << std::endl;
                    #pragma omp flush(found)
                }
            }
        }

    }

    // El nodo principal descifra el texto cifrado con la clave encontrada
    MPI_Barrier(MPI_COMM_WORLD);

    if (id == 0) {

        if(found != -1){
            std::string keyStr(8, '\0');
            for (int j = 0; j < 8; ++j) {
                keyStr[j] = (found >> (j * 8)) & 0xFF;  // Extrae cada byte de la clave
            }

            // Descifra el texto encontrado y muestra el resultado
            std::cout << "Llave: " << keyStr << std::endl;
            DESCrypt desCrypt(keyStr);
            std::string decryptedText = desCrypt.decrypt(cipherText);
            std::cout <<"Texto desencriptado: " << decryptedText << std::endl;
        }

    } else {
        // Enviar mensaje a 0 de que rank finalizó
        int node = 0;
        MPI_Send(NULL, 0, MPI_LONG, node, rankFinishedTag, MPI_COMM_WORLD);
    }

    std::cout << "Finalizando rank " << id << std::endl;

    // Finaliza la ejecución de MPI
    MPI_Finalize();
    return 0;
}
