#include <iostream>
#include <cstring>   // Para memcpy y strstr
#include <string>
#include <ctime>     // Para medir el tiempo de ejecución

// Simulación de una función de cifrado/descifrado DES (esto debería reemplazarse por una implementación real o una librería)
std::string encrypt(const std::string& key, const std::string& plaintext) {
    // En este ejemplo, simplemente se agrega la clave al inicio del texto como parte del cifrado
    return key + plaintext;  // Simulación simple (esto debe ser reemplazado por DES real)
}

std::string decrypt(const std::string& key, const std::string& ciphertext) {
    // Se retira la clave del texto cifrado
    return ciphertext.substr(key.size());  // Simulación (esto debe ser reemplazado por DES real)
}

// Función de fuerza bruta para encontrar la clave
bool tryKey(const std::string& key, const std::string& ciphertext, const std::string& searchPhrase) {
    std::string decrypted = decrypt(key, ciphertext);  // Desencriptar el texto usando la clave
    return decrypted.find(searchPhrase) != std::string::npos;  // Verificar si la frase clave está en el texto descifrado
}

int main() {
    // Texto original y palabra clave que se busca
    std::string plaintext = "Esta es una prueba de proyecto 2";
    std::string searchPhrase = "es una prueba de";
    
    // Generar el texto cifrado (simulado)
    std::string correctKey = "123456";  // Esta es la clave correcta que se intentará encontrar
    std::string ciphertext = encrypt(correctKey, plaintext);

    // Variables para medir el tiempo de ejecución
    std::clock_t start;
    double duration;

    start = std::clock();

    // Búsqueda de la clave por fuerza bruta (se itera por claves posibles)
    for (long long i = 0; i < 1000000; ++i) {  // Ajustar el límite según el tamaño de las claves que se quieran probar
        // Se convierte el número en una clave (simulada como una cadena de longitud fija de 6 dígitos)
        std::string currentKey = std::to_string(i);

        // Ajustar el tamaño de la clave a 6 dígitos
        while (currentKey.size() < 6) {
            currentKey = "0" + currentKey;
        }

        // Intentar descifrar usando la clave actual
        if (tryKey(currentKey, ciphertext, searchPhrase)) {
            std::cout << "Clave encontrada: " << currentKey << std::endl;
            break;  // Salimos del ciclo si encontramos la clave
        }
    }

    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "Tiempo de ejecución: " << duration << " segundos" << std::endl;

    return 0;
}
