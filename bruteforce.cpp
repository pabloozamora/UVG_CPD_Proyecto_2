#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <openssl/des.h>
#include <openssl/sha.h>

#define LENGTH 8
#define CORRECT_KEY "12345678"
#define LIMIT 100000000

// Genera una clave DES válida a partir de una clave de cualquier tamaño
std::string generateDesKey(const std::string& key) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(key.c_str()), key.length(), hash);
    
    return std::string(reinterpret_cast<char*>(hash), 8);
}

std::string encrypt(const std::string& key, const std::string& plaintext) {
    std::string des_key = generateDesKey(key);

    DES_key_schedule schedule;
    DES_set_key((DES_cblock*)des_key.c_str(), &schedule);

    int padded_length = ((plaintext.length() + 7) / 8) * 8;
    unsigned char* padded_plaintext = new unsigned char[padded_length];
    memcpy(padded_plaintext, plaintext.c_str(), plaintext.length());
    memset(padded_plaintext + plaintext.length(), 0, padded_length - plaintext.length());

    unsigned char* ciphertext = new unsigned char[padded_length];
    for (int i = 0; i < padded_length; i += 8) {
        DES_ecb_encrypt((DES_cblock*)(padded_plaintext + i), (DES_cblock*)(ciphertext + i), &schedule, DES_ENCRYPT);
    }

    std::string result(reinterpret_cast<char*>(ciphertext), padded_length);
    delete[] padded_plaintext;
    delete[] ciphertext;

    return result;
}

std::string decrypt(const std::string& key, const std::string& ciphertext) {
    std::string des_key = generateDesKey(key);

    DES_key_schedule schedule;
    DES_set_key((DES_cblock*)des_key.c_str(), &schedule);

    int padded_length = ciphertext.length();
    unsigned char* decryptedtext = new unsigned char[padded_length];

    for (int i = 0; i < padded_length; i += 8) {
        DES_ecb_encrypt((DES_cblock*)(ciphertext.c_str() + i), (DES_cblock*)(decryptedtext + i), &schedule, DES_DECRYPT);
    }

    std::string result(reinterpret_cast<char*>(decryptedtext), padded_length);
    delete[] decryptedtext;

    return result.erase(result.find_last_not_of('\0') + 1);
}

bool tryKey(const std::string& key, const std::string& ciphertext, const std::string& searchPhrase) {
    std::string decrypted = decrypt(key, ciphertext);

    bool found = decrypted.find(searchPhrase) != std::string::npos;

    if (found) {
        std::cout << "Texto desencriptado: " << decrypted << std::endl;
    }

    return found;
}

int main() {
    std::string plaintext = "Esta es una prueba de proyecto 2";
    std::string searchPhrase = "es una prueba de";

    std::string correctKey = CORRECT_KEY;
    std::string ciphertext = encrypt(correctKey, plaintext);

    std::cout << "Texto encriptado: " << ciphertext << std::endl;

    std::clock_t start;
    double duration;

    start = std::clock();

    for (long long i = 0; i < LIMIT; ++i) {
        std::string currentKey = std::to_string(i);

        while (currentKey.size() < LENGTH) {
            currentKey = "0" + currentKey;
        }

        if (tryKey(currentKey, ciphertext, searchPhrase)) {
            std::cout << "Clave encontrada: " << currentKey << std::endl;
            duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
            std::cout << "Tiempo de ejecución: " << duration << " segundos" << std::endl;
            return 0;
        }
    }

    std::cout << "Error: No se ha podido encontrar la clave." << std::endl;

    return 0;
}
