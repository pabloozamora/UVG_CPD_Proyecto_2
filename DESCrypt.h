#pragma once
#include <string>
#include <vector>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <cstring>

class DESCrypt {
private:
    DES_cblock key;                // Almacena la clave de cifrado
    DES_key_schedule schedule;      // Estructura para programar la clave

    // Método estático para añadir padding PKCS7
    static std::vector<unsigned char> addPKCS7Padding(const std::string& data, size_t blockSize) {
        size_t paddingLength = blockSize - (data.length() % blockSize);
        std::vector<unsigned char> padded(data.begin(), data.end());
        padded.insert(padded.end(), paddingLength, static_cast<unsigned char>(paddingLength));
        return padded;  // Devuelve los datos con padding
    }

    // Método estático para eliminar el padding PKCS7
    static std::string removePKCS7Padding(const std::vector<unsigned char>& data) {
        if (data.empty())  // Verifica si los datos están vacíos
            throw std::runtime_error("Empty data");

        unsigned char lastByte = data.back();  // Obtiene el último byte para determinar la long longitud del padding
        size_t paddingLength = static_cast<size_t>(lastByte);  // long longitud del padding

        // Verifica la validez del padding
        if (paddingLength == 0 || paddingLength > data.size())
            throw std::runtime_error("Invalid padding");

        for (size_t i = data.size() - paddingLength; i < data.size(); ++i) {
            if (data[i] != lastByte)  // Comprueba que todos los bytes de padding son válidos
                throw std::runtime_error("Invalid padding");
        }

        // Devuelve los datos sin padding
        return std::string(data.begin(), data.end() - paddingLength);
    }

public:
    // Constructor que inicializa la clave de cifrado
    DESCrypt(const std::string& keyStr) {
        std::string paddedKey = keyStr;
        if (paddedKey.length() < 8) {  // Si la clave es más corta de 8 bytes
            paddedKey.append(8 - paddedKey.length(), '\0');  // Rellena con ceros
        } else if (paddedKey.length() > 8) {  // Si la clave es más larga de 8 bytes
            paddedKey = paddedKey.substr(0, 8);  // Corta a 8 bytes
        }
        std::memcpy(key, paddedKey.c_str(), 8);  // Copia la clave
        DES_set_key_unchecked(&key, &schedule);  // Establece la clave en la programación DES
    }

    // Método para cifrar un texto plano
    std::vector<unsigned char> encrypt(const std::string& plaintext) {
        std::vector<unsigned char> paddedText = addPKCS7Padding(plaintext, 8);  // Añade padding al texto
        std::vector<unsigned char> ciphertext(paddedText.size() + 8);  // Crea un vector para el texto cifrado con espacio para IV
        DES_cblock ivec;  // Vector de inicialización

        // Genera un IV aleatorio
        if (RAND_bytes(reinterpret_cast<unsigned char*>(&ivec), 8) != 1) {
            throw std::runtime_error("Failed to generate random IV");
        }

        // Copia el IV al inicio del texto cifrado
        std::memcpy(ciphertext.data(), &ivec, 8);

        // Cifra el texto utilizando DES en modo CBC
        DES_ncbc_encrypt(
            paddedText.data(),
            ciphertext.data() + 8,  // Comienza después del IV
            static_cast<long long>(paddedText.size()),
            &schedule,
            &ivec,
            DES_ENCRYPT
        );

        return ciphertext;  // Devuelve el texto cifrado
    }

    // Método para descifrar un texto cifrado
    std::string decrypt(const std::vector<unsigned char>& ciphertext) {
        if (ciphertext.size() <= 8) {  // Verifica que el texto cifrado tenga un tamaño válido
            throw std::runtime_error("Ciphertext too short");
        }

        std::vector<unsigned char> plaintext(ciphertext.size() - 8);  // Crea un vector para el texto plano
        DES_cblock ivec;  // Vector de inicialización
        std::memcpy(&ivec, ciphertext.data(), 8);  // Copia el IV del texto cifrado

        // Descifra el texto utilizando DES en modo CBC
        DES_ncbc_encrypt(
            ciphertext.data() + 8,
            plaintext.data(),
            static_cast<long long>(ciphertext.size() - 8),
            &schedule,
            &ivec,
            DES_DECRYPT
        );

        return removePKCS7Padding(plaintext);  // Elimina el padding y devuelve el texto plano
    }
};