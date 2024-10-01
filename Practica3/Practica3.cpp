#include <mpi.h>
#include <iostream>

constexpr int TAM_VECTOR = 4;
constexpr int RANK_MAESTRO = 0;
constexpr int N_ELEMENTOS = 1;

/*
	Genera un vector de tamaño TAM_VECTOR con números aleatorios entre 0 y 9.
*/
void generateArray(int* array);
/*
	Imprime un vector de tamaño TAM_VECTOR.
*/
void printArray(int* array);

/*
	Función principal.
*/
int main(int argc, char* argv[]) {
    int rank, size = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != TAM_VECTOR) {
        if (rank == RANK_MAESTRO) {
            printf("El número de procesos debe ser igual al tamaño del vector (%d).\n", TAM_VECTOR);
        }
        MPI_Finalize();
        return -1;
    }

    int A[TAM_VECTOR], B[TAM_VECTOR];
    int localA, localB = 0;
    int producto_parcial, producto_final, max_parcial, min_parcial, max_final, min_final = 0;

	// Semilla para los números aleatorios
    srand(time(NULL));
    if (rank == RANK_MAESTRO) {
        generateArray(A);
        generateArray(B);
        printf("Vector A: ");
        printArray(A);
        printf("Vector B: ");
        printArray(B);
    }

	// Iniciar el tiempo de ejecución
	double startTime = MPI_Wtime();

    // Repartir los elementos de los vectores
    MPI_Scatter(A, N_ELEMENTOS, MPI_INT, &localA, N_ELEMENTOS, MPI_INT, RANK_MAESTRO, MPI_COMM_WORLD);
    MPI_Scatter(B, N_ELEMENTOS, MPI_INT, &localB, N_ELEMENTOS, MPI_INT, RANK_MAESTRO, MPI_COMM_WORLD);

    // Calcular el producto parcial
    producto_parcial = localA * localB;
    max_parcial = producto_parcial;
    min_parcial = producto_parcial;

    // Reducir los productos parciales al proceso maestro
    MPI_Reduce(&producto_parcial, &producto_final, N_ELEMENTOS, MPI_INT, MPI_SUM, RANK_MAESTRO, MPI_COMM_WORLD);
    MPI_Reduce(&max_parcial, &max_final, N_ELEMENTOS, MPI_INT, MPI_MAX, RANK_MAESTRO, MPI_COMM_WORLD);
    MPI_Reduce(&min_parcial, &min_final, N_ELEMENTOS, MPI_INT, MPI_MIN, RANK_MAESTRO, MPI_COMM_WORLD);

	// Finalizar el tiempo de ejecución
	double endTime = MPI_Wtime();

    if (rank == RANK_MAESTRO) {
        printf("El producto escalar es: %d\n", producto_final);
        printf("El valor máximo de los productos parciales es: %d\n", max_final);
        printf("El valor mínimo de los productos parciales es: %d\n", min_final);
		printf("Tiempo de ejecución: %f\n", endTime - startTime);
    }

	// Sincronizar los procesos
	MPI_Barrier(MPI_COMM_WORLD);

	// Liberar memoria
	free(A);
	free(B);

    MPI_Finalize();
    return 0;
}

void generateArray(int* array) {
    for (int i = 0; i < TAM_VECTOR; i++) {
        array[i] = rand() % 10; // Números aleatorios entre 0 y 9
    }
}

void printArray(int* array) {
    printf("\n[");
    for (int i = 0; i < TAM_VECTOR; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("%d", array[i]);
    }
    printf("]\n");
}