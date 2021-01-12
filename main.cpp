#include <mpi/mpi.h>
#include <stdio.h>
#include <cmath>
#include <gmp/gmp.h>

using namespace std;

void fact(int i, mpf_t res)
{
    mpf_t temp;
    mpf_init(temp);
    mpf_set_ui(temp, 1);
    for (size_t j = 1; j < i + 1; j++)
    {
        mpf_mul_ui(temp, temp, j);
    }
    mpf_set(res, temp);
    mpf_clear(temp);
}

int main(int argc, char **argv)
{
    long unsigned int upper_limit;
    int rank, size, buf_size;
    int start = 0;
    int *buf;
    float local_pi = 0.0;
    mpf_t num;
    mpf_t den;
    MPI_Status status;
    mpf_init(num);
    mpf_init(den);
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Get User input
    if (rank == 0)
    {
        upper_limit = atoi(argv[1]);
        printf("This is a program to calculate the value of pi\nupper limit = %ld, no. of processes = %d\n", upper_limit, size);
        int start_buf_size = ceil(upper_limit / size); //get max size of start buffer
        buf_size = floor(upper_limit / size);          //get max size of buffer
        buf = new int[start_buf_size];
        int new_start = start + start_buf_size;
        for (int i = 1; i < size; i++)
        {
            MPI_Send(&buf_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);  //send no. of iterations
            MPI_Send(&new_start, 1, MPI_INT, i, 1, MPI_COMM_WORLD); //send start
            new_start += buf_size;
        }
        buf_size = start_buf_size;
    }
    else
    {
        MPI_Recv(&buf_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&start, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        buf = new int[buf_size];
    }

    //Calculate local part of pi
    for (size_t i = 0; i < buf_size; i++)
    {
        long long unsigned int value = start + i;
        fact(value, num);
        mpf_mul(num, num, num);
        fact(2 * value + 1, den);
        mpf_div(num, num, den);
        double r = mpf_get_d(num);
        local_pi += r * pow(2, value + 1);
    }

    //send/receive result
    if (rank != 0)
    {
        //send local part
        MPI_Send(&local_pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        //receive and print local part
        for (int i = 1; i < size; i++)
        {
            double ans = 0.0;
            MPI_Recv(&ans, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            local_pi += ans;
        }

        printf("pi = %lf\n", local_pi);
    }

    mpf_clear(num);
    mpf_clear(den);
    MPI_Finalize();
    
    return 0;
}
