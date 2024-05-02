#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include <omp.h>

typedef unsigned long long ull;

// Prototype declarations
int nqueens(int proc, ull i, ull n);
ull factorial(ull n);

/**
 * int main(int argc, char*argv[])
 * - The starting point of the program, calculates 
 * the number of solutions for the n-queens problem
 * given an input n for an n x n board using MPI.
 **/
int main(int argc, char*argv[]) {

  ull i = 0 , n = 0, total = 0;
  double start_time, end_time;
  int num_workers;

  start_time = omp_get_wtime();

  // find out if program has correct command line arguments, prompt if not
  if(argc != 3) {
      printf("Invalid number of command line arguments.\nFormat should be ./nqueens <n>\n\n");
  } else {
    // set value of n from command line and create an array of length n
    n = atoi(argv[1]);
    num_workers = atoi(argv[2]);
    omp_set_num_threads(num_workers);

    // find the max size for the loop below
    ull max = factorial(n);
    
#pragma omp parallel for
    for( i = 0; i < max; i++) {
    int res = nqueens(0, i, n);
#pragma omp atomic
      total += res;
    } 
  }

  // find ending time
  end_time = omp_get_wtime();
  
  printf("The execution time is %g sec\n", end_time - start_time);
  printf("Total number of solutions found: %llu\n\n", total);
  fflush(stdout);
  return 0;
}

/**
 * int nqueens(int proc, int*vals, ull i, ull n)
 * - nqueens takes an MPI rank, the current ith permutation
 * and the size of the board and finds the i8th permutation, 
 * and validates that no queens are in the diagonal of the 
 * current queen, if there is, we short-circuit and return 0,
 * otherwise we return 1; 
 **/
int nqueens(int proc, ull i, ull n) {

  // declare some variables and allocate arrays
  int a, b = 0;
  int *fact = (int *)calloc(n, sizeof(int));
  int *perm = (int *)calloc(n, sizeof(int));

  // calculate factorial based on i, store in perm
  fact[b] = 1;
  while(++b < (int)n) {
    fact[b] = fact[b-1]*b;
  }
  
  for(b = 0; b < (int)n; ++b) {
    perm[b] = i / fact[n - 1 - b];
    i = i % fact[n - 1 - b];
  }

  for(b = n - 1; b > 0; --b) {
    for(a = b - 1; a >= 0; --a) {
      if(perm[a] <= perm[b]) {
	perm[b]++;
      }
    }
  }

  // free up fact array now
  free(fact);

  // loop through all elements of the permutation
  for(ull j = 0; j < n; j++) {
    int val = perm[j];

    // check everything ahead of j
    for(int k = j+1, dist = 1; k < (int)n; k++, dist++) {
      // check if the value +/- dist is equal (means its a diagnoal)
      if(val - dist == perm[k] || val + dist  == perm[k]) {
	free(perm);
	return 0;
      }
    }

    // check everything behind j
    for(int k = j-1, dist = 1; k >= 0; k--, dist++) {
      // check if the value +/- dist is equal (means its a diagonal)
      if(val - dist == perm[k] || val + dist  == perm[k]) {
	free(perm);
	return 0;
      }
    }
  }

  //iff we made it to here, free and return 1
  free(perm);
  return 1;
}
 
/**
 * ull factorial(ull n)
 * - factorial is a one-line function to recursively calculate
 * the factorial for a given n value.
 **/
ull factorial(ull n) {
  return (n == 1 || n == 0) ? 1 : factorial( n - 1 ) * n;
}