#include <iostream>
#include <iomanip>
#include <bitset>
#include <vector>
#include <string>
#include <cstring>

#define LENGTH 8
#define CORRECT_KEY "12345678"
#define LIMIT 100000000

using namespace std;

const int PC1[56] = {57, 49, 41, 33, 25, 17, 9,
                     1, 58, 50, 42, 34, 26, 18,
                     10, 2, 59, 51, 43, 35, 27,
                     19, 11, 3, 60, 52, 44, 36,
                     63, 55, 47, 39, 31, 23, 15,
                     7, 62, 54, 46, 38, 30, 22,
                     14, 6, 61, 53, 45, 37, 29,
                     21, 13, 5, 28, 20, 12, 4};

const int PC2[48] = {14, 17, 11, 24, 1, 5,
                     3, 28, 15, 6, 21, 10,
                     23, 19, 12, 4, 26, 8,
                     16, 7, 27, 20, 13, 2,
                     41, 52, 31, 37, 47, 55,
                     30, 40, 51, 45, 33, 48,
                     44, 49, 39, 56, 34, 53,
                     46, 42, 50, 36, 29, 32};

const int SHIFT[16] = {1, 1, 2, 2, 2, 2, 2, 2,
                       1, 2, 2, 2, 2, 2, 2, 1};

const int E[48] = {32, 1, 2, 3, 4, 5,
                   4, 5, 6, 7, 8, 9,
                   8, 9, 10, 11, 12, 13,
                   12, 13, 14, 15, 16, 17,
                   16, 17, 18, 19, 20, 21,
                   20, 21, 22, 23, 24, 25,
                   24, 25, 26, 27, 28, 29,
                   28, 29, 30, 31, 32, 1};

const int S[8][64] = {
    // S1
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
     0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
     4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
     15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},

    // S2
    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
     3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
     0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
     13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},

    // S3
    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
     13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
     13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
     1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},

    // S4
    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
     13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
     10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
     3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},

    // S5
    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
     14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
     4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
     11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},

    // S6
    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
     10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
     9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
     4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},

    // S7
    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
     13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
     1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
     6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},

    // S8
    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
     1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
     7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
     2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}};

const int P[32] = {16, 7, 20, 21,
                   29, 12, 28, 17,
                   1, 15, 23, 26,
                   5, 18, 31, 10,
                   2, 8, 24, 14,
                   32, 27, 3, 9,
                   19, 13, 30, 6,
                   22, 11, 4, 25};

const int IP[64] = {58, 50, 42, 34, 26, 18, 10, 2,
                    60, 52, 44, 36, 28, 20, 12, 4,
                    62, 54, 46, 38, 30, 22, 14, 6,
                    64, 56, 48, 40, 32, 24, 16, 8,
                    57, 49, 41, 33, 25, 17, 9, 1,
                    59, 51, 43, 35, 27, 19, 11, 3,
                    61, 53, 45, 37, 29, 21, 13, 5,
                    63, 55, 47, 39, 31, 23, 15, 7};

const int P_1[64] = {40, 8, 48, 16, 56, 24, 64, 32,
                     39, 7, 47, 15, 55, 23, 63, 31,
                     38, 6, 46, 14, 54, 22, 62, 30,
                     37, 5, 45, 13, 53, 21, 61, 29,
                     36, 4, 44, 12, 52, 20, 60, 28,
                     35, 3, 43, 11, 51, 19, 59, 27,
                     34, 2, 42, 10, 50, 18, 58, 26,
                     33, 1, 41, 9, 49, 17, 57, 25};

bitset<64> ipFunction(bitset<64> block)
{
    bitset<64> permutedBlock;
    for (int i = 0; i < 64; ++i)
    {
        permutedBlock[i] = block[IP[i] - 1];
    }
    return permutedBlock;
}

bitset<64> inverseIP(bitset<64> block)
{
    bitset<64> permutedBlock;
    for (int i = 0; i < 64; ++i)
    {
        permutedBlock[i] = block[P_1[i] - 1];
    }
    return permutedBlock;
}

bitset<48> expand(bitset<32> halfBlock)
{
    bitset<48> expandedBlock;
    for (int i = 0; i < 48; ++i)
    {
        expandedBlock[i] = halfBlock[E[i] - 1];
    }
    return expandedBlock;
}

bitset<32> substitute(bitset<48> input)
{
    bitset<32> output;
    for (int i = 0; i < 8; ++i)
    {
        int row = (input[i * 6] << 1) | input[i * 6 + 5];
        int col = (input[i * 6 + 1] << 3) | (input[i * 6 + 2] << 2) |
                  (input[i * 6 + 3] << 1) | input[i * 6 + 4];
        int sValue = S[i][row * 16 + col];
        for (int j = 0; j < 4; ++j)
        {
            output[i * 4 + j] = (sValue >> (3 - j)) & 1;
        }
    }
    return output;
}

bitset<32> f_function(bitset<32> right, bitset<48> key)
{
    bitset<48> expandedRight = expand(right);
    expandedRight ^= key;

    bitset<32> substitutedOutput = substitute(expandedRight);

    bitset<32> output;
    for (int i = 0; i < 32; ++i)
    {
        output[i] = substitutedOutput[P[i] - 1];
    }
    return output;
}

vector<bitset<48>> generateKeys(bitset<64> key)
{
    bitset<56> permutedKey;
    bitset<28> left;
    bitset<28> right;
    bitset<56> combinedKey;
    vector<bitset<48>> keys;

    // PC-1
    for (int i = 0; i < 56; ++i)
    {
        permutedKey[i] = key[PC1[i] - 1];
    }

    // C0 y D0
    for (int i = 0; i < 28; ++i)
    {
        left[i] = permutedKey[i];
        right[i] = permutedKey[i + 28];
    }

    // Generar subclaves
    for (int round = 0; round < 16; ++round)
    {
        left = (left << SHIFT[round]) | (left >> (28 - SHIFT[round]));
        right = (right << SHIFT[round]) | (right >> (28 - SHIFT[round]));

        for (int i = 0; i < 28; ++i)
        {
            combinedKey[i] = right[i];
            combinedKey[i + 28] = left[i];
        }

        // PC-2
        bitset<48> roundKey;
        for (int j = 0; j < 48; ++j)
        {
            roundKey[j] = combinedKey[PC2[j] - 1];
        }

        keys.push_back(roundKey);
    }

    return keys;
}

bitset<64> desEncrypt(bitset<64> block, vector<bitset<48>> &keys)
{
    bitset<32> left;
    bitset<32> right;
    bitset<64> combinedBlock;

    // IP
    block = ipFunction(block);

    // L0 y R0
    for (int i = 0; i < 32; ++i)
    {
        right[i] = block[i];
        left[i] = block[i + 32];
    }

    // Rondas
    for (int i = 0; i < 16; ++i)
    {
        bitset<32> temp = right;
        right = left ^ f_function(right, keys[i]);
        left = temp;
    }

    // Concatenar
    for (int i = 0; i < 32; ++i)
    {
        combinedBlock[i] = left[i];
        combinedBlock[i + 32] = right[i];
    }

    // IP^-1)
    return inverseIP(combinedBlock);
}

bitset<64> desDecrypt(bitset<64> block, vector<bitset<48>> &keys)
{
    // IP
    block = ipFunction(block);

    // L0 y R0
    bitset<32> left, right;
    for (int i = 0; i < 32; ++i)
    {
        right[i] = block[i];
        left[i] = block[i + 32];
    }

    // Rondas
    for (int i = 15; i >= 0; --i)
    {
        bitset<32> temp = right;
        right = left ^ f_function(right, keys[i]);
        left = temp;
    }

    // Concatenar
    bitset<64> combinedBlock;
    for (int i = 0; i < 32; ++i)
    {
        combinedBlock[i] = left[i];
        combinedBlock[i + 32] = right[i];
    }

    // IP^-1
    return inverseIP(combinedBlock);
}

vector<bitset<64>> stringToBitset(const string &text)
{
    vector<bitset<64>> blocks;
    for (size_t i = 0; i < text.length(); i += 8)
    {
        bitset<64> block;
        for (int j = 0; j < 8 && i + j < text.length(); ++j)
        {
            char c = text[i + j];
            for (int k = 0; k < 8; ++k)
            {
                block[63 - (j * 8 + k)] = (c >> k) & 1;
            }
        }
        blocks.push_back(block);
    }
    return blocks;
}

string bitsetToString(const vector<bitset<64>> &blocks)
{
    string result;
    for (const auto &block : blocks)
    {
        for (int i = 0; i < 8; ++i)
        {
            char c = 0;
            for (int j = 0; j < 8; ++j)
            {
                c |= block[63 - (i * 8 + j)] << j;
            }
            if (c != 0)
                result += c;
        }
    }
    return result;
}

bool tryKey(vector<bitset<48>> &keys, const vector<bitset<64>> &cipherBlocks, const string &searchPhrase)
{
    vector<bitset<64>> decryptedBlocks;
    for (const auto &block : cipherBlocks)
    {
        decryptedBlocks.push_back(desDecrypt(block, keys));
    }

    string decryptedText = bitsetToString(decryptedBlocks);

    bool found = decryptedText.find(searchPhrase) != string::npos;

    if (found)
    {
        cout << "Texto descifrado: " << decryptedText << endl;
    }

    return found;
}

bitset<64> convertKeyToBitset(const std::string &str)
{
    std::bitset<64> bits;
    for (size_t i = 0; i < str.size() && i < 8; ++i)
    {
        std::bitset<8> byte(str[i]);
        for (size_t j = 0; j < 8; ++j)
        {
            bits[i * 8 + j] = byte[j];
        }
    }
    return bits;
}

int main()
{
    std::clock_t start;
    double duration;
    string correctKey = CORRECT_KEY;
    bitset<64> key = convertKeyToBitset(correctKey);

    vector<bitset<48>> keys = generateKeys(key);

    string plaintext = "Esta es una prueba de proyecto 2";
    string searchPhrase = "Esta es una prueba de";

    vector<bitset<64>> plainBlocks = stringToBitset(plaintext);
    vector<bitset<64>> cipherBlocks;

    for (const auto &block : plainBlocks)
    {
        cipherBlocks.push_back(desEncrypt(block, keys));
    }

    cout << "Texto encriptado: ";
    for (const auto &block : cipherBlocks)
    {
        cout << hex << setw(16) << setfill('0') << block.to_ullong();
    }
    cout << endl;

    start = std::clock();

    for (long long i = 0; i < LIMIT; ++i)
    {
        string currentKeyString = std::to_string(i);
        while (currentKeyString.size() < LENGTH)
        {
            currentKeyString = "0" + currentKeyString;
        }

        bitset<64> currentKey = convertKeyToBitset(currentKeyString);

        vector<bitset<48>> newKeys = generateKeys(currentKey);

        if (tryKey(newKeys, cipherBlocks, searchPhrase))
        {
            std::cout << "Clave encontrada: " << currentKeyString << std::endl;
            duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
            std::cout << "Tiempo de ejecución: " << duration << " segundos" << std::endl;
            return 0;
        }
    }

    std::cout << "Error: No fue posible encontrar la clave." << std::endl;

    return 0;
}