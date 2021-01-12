#include <mpi/mpi.h>
#include <stdio.h>
#include <cmath>
#include <gmp/gmpxx.h>
#include <vector>

using namespace std;

vector<mpf_class> factorials;

mpf_class fact(int i)
{
    mpf_class temp = 1;
    if (i < factorials.size())
    {
        return factorials[i];
    }
    else
    {
        int index = factorials.size() - 1;
        temp = factorials[index];
        for (size_t j = index+1 ; j <= i; j++)
        {
            temp*=j;
            factorials.push_back(temp);
        }
    }
    return temp;
}

int main(int argc, char **argv)
{

    long unsigned int upper_limit;
    int rank, size, buf_size;
    int start = 0;
    int *buf;
    mpf_class local_pi = 0.0;
    mpf_class num;
    mpf_class den;
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (size_t i = 0; i < 2; i++)
    {
        factorials.push_back(1);
    }

    //Get User input
    if (rank == 0)
    {
        upper_limit = atoi(argv[1]);
        //upper_limit = 1000;
        printf("upper limit = %ld, no. of processes = %d\n", upper_limit, size);
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
        num = fact(value);
        num*=num;
        den = fact(2 * value + 1);
        num/=den;
        local_pi += num * pow(2, value + 1);
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

        printf("pi = %f\n", local_pi.get_d());
    }

    MPI_Finalize();

    return 0;
}
