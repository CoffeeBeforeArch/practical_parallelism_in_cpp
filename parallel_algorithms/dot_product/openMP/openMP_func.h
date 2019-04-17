// This header includes the openMP implementation of Dot-Product
// By: Nick from CoffeeBeforeArch

#include <omp.h>

void dot_prod_openMP(double *a, double *b, int n, double &result){
    result = 0;

    #pragma omp for reduction(+:result) 
    for(int i = 0; i < n; i++){
        result += a[i] * b[i];
    }
}
