#include "DESCrypt.h"
#include <iostream>
#include <sstream>
#include <vector>

std::vector<unsigned char> parseNumbers(const std::string& input) {
    std::vector<unsigned char> result;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(static_cast<unsigned char>(std::stoi(item))); // Convierte cada número a unsigned char
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <clave_como_numeros> <texto_cifrado_como_numeros>" << std::endl;
        return 1;
    }

    // Convierte la clave y el texto cifrado de cadenas de números a vectores de unsigned char
    std::string keyStr = argv[1];
    std::string ciphertextStr = argv[2];
    
    std::vector<unsigned char> keyVec = parseNumbers(keyStr);
    std::vector<unsigned char> ciphertext = parseNumbers(ciphertextStr);

    if (keyVec.size() > 8) {
        std::cerr << "Error: La clave debe tener menos de 8 bytes" << std::endl;
        return 1;
    }

    try {
        // Crea la clave a partir del vector de números
        std::string key(keyVec.begin(), keyVec.end());

        DESCrypt descrypt(key);

        // Descifra el texto cifrado
        std::string decryptedText = descrypt.decrypt(ciphertext);

        std::cout << "Texto descifrado: " << decryptedText << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
