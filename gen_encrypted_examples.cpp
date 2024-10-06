#include "DESCrypt.h"
#include <ostream>
#include <iostream>
  
int main() {
    try {
        std::string key = "pepa";
        DESCrypt des(key);

        std::string plaintext = "Pepito en miami";
        std::cout << "Original text: " << plaintext << std::endl;

        
        auto ciphertext = des.encrypt(plaintext);
        std::cout << "Encrypted (integers): ";
        for (unsigned char c : ciphertext) {
            std::cout << static_cast<int>(c) << ", ";  // Imprime como enteros
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
