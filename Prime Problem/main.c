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
        MPI_Recv(&lb, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&ub, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = lb; i <= ub; i++)
            if (isPrime(i))
                cnt++;
        MPI_Send(&cnt, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    else
    {
        scanf("%d", &x);
        scanf("%d", &y);
        if (p == 1)
        {
            for (int i = x; i <= y; i++)
            {
                if (isPrime(i))
                    cnt++;
            }
            printf("Total number of prime number in Master Process: %d\n", cnt);
            return 0;
        }
        r = (y - x + 1) / (p - 1);
        modu = (y - x + 1) % (p - 1);
        lb = x;
        for (source = 1; source < p; source++)
        {
            range = r;
            MPI_Send(&lb, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
            if (source > (p - 1) - modu)
                range++;
            ub = lb + range - 1;
            MPI_Send(&ub, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
            lb = ub + 1;
            MPI_Recv(&cnt, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            primes[source - 1] = cnt;
            sum += primes[source - 1];
        }

        printf("Total number of prime numbers is: %d\n", sum);
        for (source = 1; source < p; source++)
            printf("Total number of prime number in P%d: %d\n", source, primes[source - 1]);
    }
    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}
