# UVG_CPD_Proyecto_2
Universidad del Valle de Guatemala - Programación Paralela y Distribuida - Proyecto 2

## Instrucciones de compilación y ejecución

* bruteforce_mpi

```
mpic++ -o ./gen/bruteforce_mpi ./bruteforce_mpi.cpp -lcrypto
```

```
mpirun -np 4 ./gen/bruteforce_mpi
```