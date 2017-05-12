#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>

#define NTHREADS 8

/*---------------------------------------------------------------------------*/

void generate_bitonic_sequence(unsigned int *elem, long long int n);
void header(long long int n);
void swap(unsigned int *elem, unsigned int i, unsigned int k);
void verify(unsigned int *elem, long long int n);

/*---------------------------------------------------------------------------*/

void
generate_bitonic_sequence(unsigned int *elem, long long int n) {
    unsigned int i = 0;
    unsigned int j = n/2;
    unsigned int k = 0;

    while (k < n) {
        if (i < n/2) {
            elem[k] = i;
            i++;
        }
        else {
            elem[k] = j;
            j--;
        }

        k++;
    }
}


int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

/*---------------------------------------------------------------------------*/

void
header(long long int n) {
    printf("*********************************\n");
    printf("- Problem: Bitonic Sort\n");
    printf("- N: %lld\n", n);
    printf("- Threads: %d\n", NTHREADS);
    printf("*********************************\n\n");
}

/*---------------------------------------------------------------------------*/

void
swap(unsigned int *elem, unsigned int i, unsigned int k) {
    unsigned int aux;
    printf("ElementoK[%d] = %d\nElementoP[%d] = %d\n", i, elem[i], i+k, elem[i+k]);
    if (elem[i] > elem[i+k]) {
        aux = elem[i+k];
        elem[i+k] = elem[i];
        elem[i] = aux;
    }
}

/*---------------------------------------------------------------------------*/

void
ChangeAscending(unsigned int *elem, unsigned int i, unsigned int k) {
    unsigned int aux;
    if (elem[i] < elem[i+k]) {
        aux = elem[i+k];
        elem[i+k] = elem[i];
        elem[i] = aux;
    }
}

/*---------------------------------------------------------------------------*/

void
ChangeDescending(unsigned int *elem, unsigned int i, unsigned int k) {
    unsigned int aux;
    if (elem[i] > elem[i+k]) {
        aux = elem[i+k];
        elem[i+k] = elem[i];
        elem[i] = aux;
    }
}
/*---------------------------------------------------------------------------*/

void
verify(unsigned int *elem, long long int n) {
    unsigned int i;

    for (i = 0; i < n; i++) {
        if (elem[i] > elem[i+1] && i+1 < n) {
            printf("Array isn't sorted!\n");
            exit(1);
        }
    }
    printf("Array is sorted!\n");
}

/*---------------------------------------------------------------------------*/

// http://stackoverflow.com/questions/10192903/time-in-milliseconds
float delta(struct timeval t0, struct timeval t1) {
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

/*---------------------------------------------------------------------------*/

int
main(int argc, char *argv[]) {
    unsigned int i, j, k, l, term, descending = 1;
    long long int n;
    unsigned int *elem;//[]={3,7,4,8,6,2,1,5};

    float elapsed;
    struct timeval t0, t1;

    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        exit(1);
    }
    else {
        n = atoll(argv[1]);
    }

    elem = malloc(n * sizeof(unsigned int));

    if (!elem) {
        printf("Failed to allocate memory!\n");
        exit(1);
    }

    header(n);

    gettimeofday(&t0, 0);
    for(i = 0; i < n; i++){
        elem[i] = randint(2*n);
    }
    //generate_bitonic_sequence(elem, n);
    gettimeofday(&t1, 0);
    elapsed = delta(t0, t1);
    printf("Random Fill in %.2f ms\n\n", elapsed);


    gettimeofday(&t0, 0);
    for(i = 0; i < (int) log2(n); i++){
        printf("Step I:%d\n", i);
        for(j = i; j < -1; j--){
            int changeDirection = 0;
            int changeK = 0;
            descending = 1;
            for(k = 0; k < n; k++){
                if(descending)
                    ChangeDescending(elem, k, (int) pow(2, j));
                else
                    ChangeAscending(elem, k, (int) pow(2, j));
                changeDirection ++;
                changeK ++;

                if(changeK == (int) pow(2, j)){
                    k = k + (int) pow(2, j);
                    changeK = 0;
                }

                if(changeDirection == pow(2, i)){
                    descending *= -1;
                    descending += 1;
                    changeDirection = 0;
                }
            }
        }
    }

    gettimeofday(&t1, 0);
    elapsed = delta(t0, t1);

    printf("k = %u\nTime (ms): %.2f\n\n", k, elapsed);
    // log n steps
    /*for (k = n/2; k >= 1; k /= 2) {
        gettimeofday(&t0, 0);

        // loop through halves
        #pragma omp parallel for collapse(2) schedule(guided) num_threads(NTHREADS)
        for (i = 0; i < n; i += 2*k) {
            // loop through elements of a half
            for (j = 0; j < k; j++) {
                swap(elem, i+j, k);
            }
        }

        gettimeofday(&t1, 0);
        elapsed = delta(t0, t1);

        printf("k = %u\nTime (ms): %.2f\n\n", k, elapsed);
    }*/
    // verify if array is sorted
    verify(elem, n);
}