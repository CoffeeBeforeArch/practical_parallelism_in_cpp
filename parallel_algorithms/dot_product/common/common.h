// This is a header file including utilities and a serial versiion of
// Dot-Product
// By: Nick from CoffeeBeforeArch

#include <math.h>

// Serial implementation of Dot-Product
// Takes two vectors and their length, and returns the resulting scalar
// value
void dot_prod_serial(double *a, double *b, int n, double &result){
    result = 0;
    for(int i = 0; i < n; i++){
        result += (a[i] * b[i]);
    }
}

void vector_initializer(double *a, int n){
    for(int i = 0; i < n; i++){
        a[i] = rand() % 100;
    }
}
