#include "DESCrypt.h"
#include <ostream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>

std::string asciiToString(const std::string& asciiValues) {
    std::stringstream ss(asciiValues);
    std::string item;
    std::string result;

    while (std::getline(ss, item, ',')) {
        // Convierte cada valor ASCII a un carácter y lo agrega al resultado
        char c = static_cast<char>(std::stoi(item));
        result += c;
    }

    return result;
}

int main(int argc, char* argv[]) {
    try {
        // Verifica si se pasaron suficientes argumentos
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <key (ASCII values separated by commas)> <plaintext>" << std::endl;
            return 1;
        }

        std::string asciiKey = argv[1];
        std::string plaintext = argv[2];

        // Convierte la clave ASCII a un string
        std::string key = asciiToString(asciiKey);

        DESCrypt des(key);
        
        std::cout << "key (ascii): " << asciiKey << " :: " << key << std::endl;
        std::cout << "Original text: " << plaintext << std::endl;

        // Encripta el texto
        auto ciphertext = des.encrypt(plaintext);

        // Imprime el texto cifrado como enteros
        std::cout << "Encrypted (integers): ";
        for (unsigned char c : ciphertext) {
            std::cout << static_cast<int>(c) << ", ";
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
