#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <mpi.h>
#include "DESCrypt.h"
#include <sstream>
#include <chrono>

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
    ucharVec.reserve(intVec.size());

    for (int i : intVec) {
        ucharVec.push_back(static_cast<unsigned char>(i));
    }

    return ucharVec;
}

// Función para leer el archivo de texto plano
std::string readPlainTextFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string plainText;

    if (file.is_open()) {
        std::getline(file, plainText, '\0');  // Leer todo el archivo en una sola línea
        file.close();
    } else {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
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
    auto start = std::chrono::high_resolution_clock::now();

    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>" << std::endl;
        return 1;
    }

    std::string plainTextFile = argv[1];  // Primer argumento: nombre del archivo de texto original
    std::string asciiKey = argv[2];       // Segundo argumento: clave ASCII separada por comas
    std::string search = argv[3];         // Tercer argumento: patrón de búsqueda

    int N, id;
    long upper = (1L << 10);  // Límite superior para claves DES (2^56)
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
    if (id == N - 1) {
        myupper = upper;  // Compensar el residuo
    }

    long found = -1;  // -1: no se ha encontrado
    MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, keyFindedTag, MPI_COMM_WORLD, &req);

    // Búsqueda de la clave mediante fuerza bruta
    for (long i = mylower; i < myupper && found == -1; ++i) {
        if (i % 50000000 == 0) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time = end - start;
            std::cout << "Rank " << id << ", iteración " << i << ", tiempo transcurrido: " << time.count() << " seg" << std::endl;
        }

        std::string keyStr(8, '\0');
        for (int j = 0; j < 8; ++j) {
            keyStr[j] = (i >> (j * 8)) & 0xFF;  // Extrae cada byte de la clave
        }

        if (tryKey(keyStr, cipherText, search)) {
            found = i;
            std::cout << "Key encontrada en rank " << id << " i=" << found << std::endl;
            // Notifica a todos los nodos que se encontró la clave
            for (int node = 0; node < N; node++) {
                MPI_Isend(&found, 1, MPI_LONG, node, keyFindedTag, MPI_COMM_WORLD, &req);
            }
            break;
        }

        int messageReceived;
        MPI_Test(&req, &messageReceived, MPI_STATUS_IGNORE);

        if (messageReceived) {
            MPI_Wait(&req, MPI_STATUS_IGNORE);

            std::cout << "Key encontrada en otro nodo, saliendo del bucle " << id << std::endl;
            break;
        }
    }

     MPI_Barrier(MPI_COMM_WORLD);

    if (id == 0) {

        if (found != -1) {
            std::string keyStr(8, '\0');
            for (int j = 0; j < 8; ++j) {
                keyStr[j] = (found >> (j * 8)) & 0xFF;
            }

            DESCrypt desCrypt(keyStr);
            std::string decryptedText = desCrypt.decrypt(cipherText);
            std::cout << "Texto desencriptado: " << decryptedText << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Tiempo de ejecución: " << duration.count() << " segundos" << std::endl;

    }
        std::cout << "Finalizando rank " << id << std::endl;

    MPI_Finalize();
    return 0;
}
