// This program shows off some of the basics of OpenMP
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;

int main(){
    // Size of a table to initialize
    const int size = 256;

    // Allocate an array for a sin function table
    double table[size];

    // Normal way to calculate a table
    for(int i = 0; i < size; i++){
        table[i] = sin(2 * M_PI * i / size);
    }

    // Calculate with multiple threads
    #pragma omp parallel for
    for(int i = 0; i < size; i++){
        table[i] = sin(2 * M_PI * i / size);
    }

    // Calculate with a parallel floating point library
    #pragma omp simd
    for(int i = 0; i < size; i++){
        table[i] = sin(2 * M_PI * i / size);
    }

    // Calculate with multiple threads on another device
    #pragma omp target teams distribute parallel for map(from:table[0:256])
    for(int i = 0; i < size; i++){
        table[i] = sin(2 * M_PI * i / size);
    }

    // Print out the table
    for(int i = 0; i < size; i++){
        cout << table[i] << " ";
    }
    cout << endl;

    return 0;
}
