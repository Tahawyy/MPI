#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mpi.h"

bool isPrime(int n)
{
    if (n == 1 || n == 0)
        return false;
    for (int i = 2; i <= n / 2; i++)
        if (n % i == 0)
            return false;
    return true;
}

int main(int argc, char *argv[])
{
    int my_rank; /* rank of process */
    int p;       /* number of process */
    int source;  /* rank of sender */
    int dest;    /* rank of reciever */
    int x, y, r, lb, modu, ub, range, sum = 0; 
    int cnt = 0; /* count of primes */

    /* Start up MPI */
    MPI_Init(&argc, &argv);
    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    int primes[p]; // array to store the number of primes in each process
    if (my_rank != 0)
    {
        dest = 0;
        if(my_rank == p - 1) // if the process is the last one        
            MPI_Recv(&modu, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive the remainder
        MPI_Recv(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive lower bound
        MPI_Recv(&r, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive upper bound
        lb = (my_rank - 1) * r + x;
        ub = (my_rank * r) + x; // upper bound 
        if (my_rank == p - 1) // if the process is the last one
            ub += modu; // add the remainder to the upper bound 
        for (int i = lb; i < ub; i++) // count the number of primes in the range
            if (isPrime(i))
                cnt++;
        MPI_Send(&cnt, 1, MPI_INT, dest, 0, MPI_COMM_WORLD); // send the count to the master process
    }
    else
    {
        scanf("%d", &x); // input lower bound
        scanf("%d", &y); // input upper bound
        if (p == 1) // if there is only one process
        {
            for (int i = x; i <= y; i++)
                if (isPrime(i))
                    cnt++;           
            printf("Total number of prime number in Master Process: %d\n", cnt);
            return 0;
        }
        r = (y - x) / (p - 1); // range for each process
        modu = (y - x) % (p - 1); // remainder
        
        MPI_Send(&modu, 1, MPI_INT, p - 1, 0, MPI_COMM_WORLD); // send the remainder to the last process
        for (source = 1; source < p; source++) // send the range to each process
        {
            MPI_Send(&x, 1, MPI_INT, source, 0, MPI_COMM_WORLD); // send the lower bound
            MPI_Send(&r, 1, MPI_INT, source, 0, MPI_COMM_WORLD); // send the range
            MPI_Recv(&cnt, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive the count of primes
            primes[source - 1] = cnt; // store the count in the array
            sum += primes[source - 1]; // add the count to the total
        }
        printf("Total number of prime numbers is: %d\n", sum); // print the total number of primes
        for (source = 1; source < p; source++) // print the number of primes in each process
            printf("Total number of prime number in P%d: %d\n", source, primes[source - 1]);
    }
    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}
