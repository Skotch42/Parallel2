#include <iostream>
#include <mpi.h>

const int Length = 1234567;

double scalar(double* x, double* y, int size)
{
    double result = 0;

    for (int i = 0; i < size; ++i)
    {
        result += x[i] * y[i];
    }

    return result;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double* x = 0;
    double* y = 0;
    double start;

    if (rank == 0)
    {
        srand(time(0));
        std::cout << "Process count : " << size << "\n\n";

        x = new double[Length];
        y = new double[Length];

        for (int i = 0; i < Length; i++)
        {
            x[i] = (rand() % 1000) / 1000.0;
            y[i] = (rand() % 1000) / 1000.0;
        }
    }

    int div_length = Length / size;
    double* div_x = new double[div_length];
    double* div_y = new double[div_length];

    MPI_Scatter(x, div_length, MPI_DOUBLE, div_x, div_length, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, div_length, MPI_DOUBLE, div_y, div_length, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    start = MPI_Wtime();

    double answer = scalar(div_x, div_y, div_length);
    double result;
    MPI_Reduce(&answer, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double pTime = MPI_Wtime() - start;

    if (rank == 0)
    {
        std::cout << "Parallel time : " << pTime << "s\n";
        std::cout << "Answer : " << result << "\n\n";

        start = MPI_Wtime();
        result = scalar(x, y, Length);

        double spTime = MPI_Wtime() - start;
        std::cout << "Single process time : " << spTime << "s\n";
        std::cout << "Answer : " << result << "\n\n";
        std::cout << "Single process is " << spTime / pTime << " times slower then Parallel." << "\n";

        delete[] x;
        delete[] y;
    }

    delete[] div_x;
    delete[] div_y;

    MPI_Finalize();

	return EXIT_SUCCESS;
}