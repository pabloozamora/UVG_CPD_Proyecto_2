#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <set>
#include <unordered_map>

class NumberGenerator {
public:
    NumberGenerator(long long lower, long long upper, long long maxBlockNumbers) 
        : lower(lower), upper(upper), cont(0), currentHop(lower), currentEndPosition(lower), maxBlockNumbers(maxBlockNumbers) {
        //std::srand(std::time(nullptr));
        std::srand(1);
    }

    long long getNextNumber() {
        // Si ya se generaron todos los números posibles, retornar -1
        if (cont >= (upper - lower)) {
            return -1; 
        }

        // Si el número actual está dentro del rango actual, retornarlo
        if (currentHop < currentEndPosition) {
            cont++;
            return currentHop++;
        }

        // Calcular un nuevo hop
        currentHop = randomNumber(lower, upper);

        while (true) {
            if (currentHop < lower) {
                currentHop = lower;
            }

            currentEndPosition = currentHop + maxBlockNumbers;
            auto nextHopIt = hops.upper_bound(currentHop);
            long long nextHop = nextHopIt != hops.end() ? *nextHopIt : -1;

            auto prevHopIt = hops.lower_bound(currentHop);
            long long prevHop = prevHopIt != hops.begin() ? *(--prevHopIt) : -1;

            if (hops.find(currentHop) != hops.end()) {
                // Hop actual ya existe, desplazarse al final de este
                currentHop = endPositions[currentHop] % upper;
            } else if (prevHop != -1 && currentHop >= prevHop && currentHop < endPositions[prevHop]) {
                // Colisión con bloque previo, desplazarse al final de este
                currentHop = endPositions[prevHop] % upper;
            } else if (nextHop != -1 && currentHop < nextHop && currentEndPosition > nextHop) {
                // Colisión con el bloque siguiente. Ajustar el inicio del bloque actual antes 
                // del siguiente bloque
                currentEndPosition = nextHop;
                hops.insert(currentHop);
                break;
            } else {
                hops.insert(currentHop);
                break;
            }
        }

        // Limitar currentEndPosition a upper
        if (currentEndPosition > upper) {
            currentEndPosition = upper;
        }
        
        // Guardar el fin del bloque actual
        endPositions[currentHop] = currentEndPosition;
        
        // Retornar el primer número del nuevo rango
        long long number = currentHop++;
        cont++; // contador de números generados
        return number;
    }

private:
    long long lower;
    long long upper;
    long long cont;
    long long currentHop; // Número actual
    long long currentEndPosition; // Fin del rango actual
    long long maxBlockNumbers; // Número máximo de números por bloque
    std::set<long long> hops;
    std::unordered_map<long long, long long> endPositions;

    long long randomNumber(long long min, long long max) {
        return min + std::rand() % (max - min);
    }
};