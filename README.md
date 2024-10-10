# Programación Paralela y Distribuida - Proyecto 2

Universidad del Valle de Guatemala

Diego Andrés Morales Aquino - 21762 </br>
Erick Stiv Junior Guerra - 21781 </br>
Pablo Andrés Zamora Vasquez - 21780 </br>

Este proyecto consiste en el uso de MPI para encontrar, mediante fuerza bruta, la llave de un texto cifrado con DES. Luego se utiliza esta llave para descifrar el texto.

## Instrucciones de compilación y ejecución

* bruteforce_file

```
g++ -o ./gen/bruteforce_file ./bruteforce_file.cpp -lcrypto
```

```
./gen/bruteforce_file <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

* bruteforce_mpi

```
mpic++ -o ./gen/bruteforce_mpi ./bruteforce_mpi.cpp -lcrypto
```

```
mpirun -np 4 ./gen/bruteforce_mpi
```

* bruteforce_mpi_file

```
mpic++ -o ./gen/bruteforce_mpi_file ./bruteforce_mpi_file.cpp -lcrypto
```

```
mpirun -np 4 ./gen/bruteforce_mpi_file <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

* bruteforce_mpi_file_hops

```
mpic++ -o ./gen/bruteforce_mpi_file ./bruteforce_mpi_file.cpp -lcrypto
```

```
mpirun -np 4 ./gen/bruteforce_mpi_file <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

* bruteforce_mpi_file_parallel

```
mpic++ -o ./gen/bruteforce_mpi_file ./bruteforce_mpi_file.cpp -lcrypto -fopenmp
```

```
mpirun -np 4 ./gen/bruteforce_mpi_file <archivo_texto_plano> <llave_ascii_comas> <patron_busqueda>
```

## Parámetros
<archivo_texto_plano>: Directorio y nombre del archivo con el texto a descifrar </br>
<llave_ascii_comas>: Llave a utilizar para cifrar el texto, en octetos de un número binario, de menor a mayor (por ejemplo, "1,0,0,0,0,0,0,0") </br>
<patron_busqueda>: String a utilizar para determinar si la llave a probar es la correcta </br>
