#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <openssl/des.h>
#include <openssl/sha.h>

#define LENGTH 5
#define CORRECT_KEY "9Easz"

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

bool generateKeys(int pos, std::string& currentKey, const std::string& ciphertext, const std::string& searchPhrase) {
    if (pos == LENGTH) {
        return tryKey(currentKey, ciphertext, searchPhrase);
    }

    for (int i = 0; i < 256; ++i) {
        currentKey[pos] = static_cast<char>(i);
        if (generateKeys(pos + 1, currentKey, ciphertext, searchPhrase)) {
            return true;
        }
    }
    return false;
}

int main() {
    std::clock_t start;
    double duration;
    std::string plaintext = "Esta es una prueba de proyecto 2";
    std::string searchPhrase = "es una prueba de";

    std::string correctKey = CORRECT_KEY;
    std::string ciphertext = encrypt(correctKey, plaintext);

    std::cout << "Texto encriptado: " << ciphertext << std::endl;

    std::string currentKey(LENGTH, '\0');

    start = std::clock();
    bool found = generateKeys(0, currentKey, ciphertext, searchPhrase);
    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

    if (found) {
        std::cout << "Clave encontrada: " << currentKey << std::endl;
        std::cout << "Tiempo de ejecución: " << duration << " segundos" << std::endl;
    } else {
        std::cout << "Error: No se ha podido encontrar la clave." << std::endl;
    }

    return 0;
}
