#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include "DESCrypt.h"
#include <sstream>
#include <chrono>

// Función que intenta descifrar el texto cifrado con una clave dada
int tryKey(const std::string keyStr, std::vector<unsigned char> cipher, const std::string& search) {
    DESCrypt desCrypt(keyStr);
    std::string decryptedText;

    try {
        decryptedText = desCrypt.decrypt(cipher);  // Intenta desencriptar
    } catch (const std::runtime_error& e) {
        return 0;  // La clave no es válida, retorna 0
    }

    // Verifica si el texto descifrado contiene el patrón de búsqueda
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
        exit(1);  // Termina el programa si no se puede abrir el archivo
    }

    return plainText;
}

// Convierte una cadena de valores ASCII a una clave
std::string asciiToString(const std::string& asciiValues) {
    std::stringstream ss(asciiValues);
    std::string item;
    std::string result;

    while (std::getline(ss, item, ',')) {
        char c = static_cast<char>(std::stoi(item));  // Convierte cada valor ASCII a un carácter
        result += c;
    }

    return result;
}

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();  // Inicia el cronómetro

    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>" << std::endl;
        return 1;
    }

    // Argumentos de línea de comando
    std::string plainTextFile = argv[1];  // Primer argumento: nombre del archivo de texto original
    std::string asciiKey = argv[2];       // Segundo argumento: clave ASCII separada por comas
    std::string search = argv[3];         // Tercer argumento: patrón de búsqueda

    // Limitar el rango de claves DES a 2^56
    long upper = (1L << 56);  // Límite superior para claves DES
    long found = -1;          // Inicialización del estado de búsqueda

    // Leer y cifrar el mensaje
    std::string plainText = readPlainTextFromFile(plainTextFile);  // Lee el archivo de texto
    std::string key = asciiToString(asciiKey);  // Convierte la clave ASCII en una cadena de 8 caracteres

    // Verificación de la lectura del archivo y la clave
    std::cout << "Archivo leído: " << plainText << std::endl;
    std::cout << "Llave convertida (ASCII): " << asciiKey << " -> '" << key << "'" << std::endl;

    // Cifrar el texto usando la clave proporcionada
    DESCrypt desCrypt(key);
    std::vector<unsigned char> cipherText = desCrypt.encrypt(plainText);  // Cifrar el texto original

    // Verificación del texto cifrado
    std::cout << "Texto cifrado (enteros): ";
    for (unsigned char c : cipherText) {
        std::cout << static_cast<int>(c) << ", ";  // Mostrar la versión cifrada en formato entero
    }
    std::cout << std::endl;

    // Probar descifrado
    std::cout << "Texto descifrado (directo): " << desCrypt.decrypt(cipherText) << std::endl;

    // Búsqueda de la clave mediante fuerza bruta
    for (long i = 0; i < upper; ++i) {
        if (i % 1000000 == 0) {  // Progreso de depuración cada 1 millón de iteraciones
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time = end - start;
            std::cout << "Iteración " << i << ", tiempo transcurrido: " << time.count() << " seg" << std::endl;
        }

        // Construir la clave a partir del número de iteración
        std::string keyStr(8, '\0');
        for (int j = 0; j < 8; ++j) {
            keyStr[j] = (i >> (j * 8)) & 0xFF;  // Extrae cada byte de la clave
        }

        // Intentar descifrar con la clave generada
        if (tryKey(keyStr, cipherText, search)) {
            found = i;
            std::cout << "Clave encontrada: " << i << std::endl;
            break;
        }
    }

    // Verificar si se encontró una clave
    if (found != -1) {
        std::string keyStr(8, '\0');
        for (int j = 0; j < 8; ++j) {
            keyStr[j] = (found >> (j * 8)) & 0xFF;  // Construir la clave a partir del valor encontrado
        }

        std::cout << "Llave encontrada: " << keyStr << std::endl;
        DESCrypt desCrypt(keyStr);
        std::string decryptedText = desCrypt.decrypt(cipherText);
        std::cout << "Texto desencriptado: " << decryptedText << std::endl;
    } else {
        std::cout << "Clave no encontrada en el rango dado." << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Tiempo de ejecución: " << duration.count() << " segundos" << std::endl;

    return 0;
}
