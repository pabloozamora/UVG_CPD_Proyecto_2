# Proyecto 2: Computación Paralela y Distribuida - Fuerza Bruta para Descifrado de Claves
Universidad del Valle de Guatemala - Programación Paralela y Distribuida

Diego Andrés Morales Aquino - 21762 </br>
Erick Stiv Junior Guerra - 21781 </br>
Pablo Andrés Zamora Vasquez - 21780 </br>

Este proyecto tiene como objetivo implementar un algoritmo de fuerza bruta utilizando MPI (Message Passing Interface) para descifrar un texto cifrado mediante el algoritmo DES (Data Encryption Standard). El programa probará todas las posibles combinaciones de llaves hasta encontrar aquella que descifra correctamente el texto, validando si el texto descifrado contiene una palabra o frase clave conocida.

## Requisitos
- OpenMPI instalado en los nodos de trabajo.
- Librería OpenSSL y OpenMP
- Entorno de desarrollo en C/C++.

## Instrucciones de compilación y ejecución


* **Versión secuencial:** bruteforce_file.cpp

```
g++ -o bruteforce_file ./bruteforce_file.cpp -lcrypto
```

```
./bruteforce_file <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

**<archivo_texto_plano>**:Ruta del archivo a encriptar.\
**<llave_ascii_comas>**: Llave para encriptar texto. Debe ser proveída en formato "ascii_num1, ascii_num2, ... ascii_numN", en donde cada elemento serparado por comas representa el valor ascii de un caracter de la llave.\
**<patron_busqueda>**: Palabra a buscar en el texto desencriptado para saber si se ha logrado descifrar.

* **Versión paralela naive:** bruteforce_mpi_file.cpp

```
mpic++ -o bruteforce_mpi_file ./bruteforce_mpi_file.cpp -lcrypto
```

```
mpirun -np 4 bruteforce_mpi_file <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

**<archivo_texto_plano>**:Ruta del archivo a encriptar.\
**<llave_ascii_comas>**: Llave para encriptar texto. Debe ser proveída en formato "ascii_num1, ascii_num2, ... ascii_numN", en donde cada elemento serparado por comas representa el valor ascii de un caracter de la llave.\
**<patron_busqueda>**: Palabra a buscar en el texto desencriptado para saber si se ha logrado descifrar.

* **Versión paralela con saltos:** bruteforce_mpi_file_hops.cpp

```
mpic++ -o bruteforce_mpi_file_hops ./bruteforce_mpi_file_hops.cpp -lcrypto
```

```
mpirun -np 4 bruteforce_mpi_file_hops <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

**<archivo_texto_plano>**:Ruta del archivo a encriptar.\
**<llave_ascii_comas>**: Llave para encriptar texto. Debe ser proveída en formato "ascii_num1, ascii_num2, ... ascii_numN", en donde cada elemento serparado por comas representa el valor ascii de un caracter de la llave.\
**<patron_busqueda>**: Palabra a buscar en el texto desencriptado para saber si se ha logrado descifrar.

* **Versión paralela con openMP:** bruteforce_mpi_file_parallel.cpp

```
mpic++ -o bruteforce_mpi_file_parallel ./bruteforce_mpi_file_parallel.cpp -lcrypto -fopenmp
```

```
mpirun -np 4 bruteforce_mpi_file_parallel <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

**<archivo_texto_plano>**:Ruta del archivo a encriptar.\
**<llave_ascii_comas>**: Llave para encriptar texto. Debe ser proveída en formato "ascii_num1, ascii_num2, ... ascii_numN", en donde cada elemento serparado por comas representa el valor ascii de un caracter de la llave.\
**<patron_busqueda>**: Palabra a buscar en el texto desencriptado para saber si se ha logrado descifrar.

[!NOTE]
Para ejecutar MPI en distintos equipos se debe utilizar la bandera **--hosts manager,worker**. En donde manager y worker corresponden a las direcciones IP de los equipos correspondientes. Pueden ser más de dos equipos, agregando más direcciones a la lista.
