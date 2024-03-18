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
    int x, y, r, lb, ub, sum = 0;
    int cnt = 0;
    /* storage for  */

    /* recieve */

    /* Start up MPI */
    MPI_Init(&argc, &argv);
    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    int primes[p];
    if (my_rank != 0)
    {
        dest = 0;
        MPI_Recv(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&r, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        lb = (my_rank - 1) * r;
        ub = my_rank * r;
        for (int i = lb; i < ub; i++)
            if (isPrime(i))
                cnt++;
        MPI_Send(&cnt, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    else
    {
        scanf("%d", &x);
        scanf("%d", &y);
        r = (y - x) / (p-1);
        for (source = 1; source < p; source++)
        {
            MPI_Send(&x, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
            MPI_Send(&r, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
        }
        for (source = 1; source < p; source++)
        {
            MPI_Recv(&cnt, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            primes[source - 1] = cnt;
        }
        for (source = 1; source < p; source++)
            sum += primes[source - 1];
        
        printf("Total number of prime numbers is: %d\n", sum);
        for (source = 1; source < p; source++)
            printf("Total number of prime number in P%d: %d\n", source, primes[source - 1]);
        
    }
    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}
