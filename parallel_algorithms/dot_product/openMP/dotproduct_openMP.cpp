// This program implements a simple Dot-Product using OpenMP
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <assert.h>
#include "../common/common.h"
#include "openMP_func.h"

using namespace std;
using namespace std::chrono;

int main(){
    int n = 1 << 26;
    
    double res_serial;
    double res_parallel;

    double *a = (double*)malloc(n * sizeof(double));
    double *b = (double*)malloc(n * sizeof(double));

    vector_initializer(a, n);
    vector_initializer(b, n);

    auto start_serial = high_resolution_clock::now(); 
    dot_prod_serial(a, b, n, res_serial);
    auto end_serial = high_resolution_clock::now();

    auto duration_serial = duration_cast<microseconds>(end_serial - start_serial);
    cout << "Dot product serial result: " << res_serial << " in " <<
        duration_serial.count() << "us" << endl;
    
    auto start_parallel = high_resolution_clock::now(); 
    dot_prod_openMP(a, b, n, res_parallel);
    auto end_parallel = high_resolution_clock::now(); 
    
    auto duration_parallel = duration_cast<microseconds>(end_parallel - start_parallel);
    cout << "Dot product OpenMP result: " << res_parallel << "in " <<
        duration_parallel.count() << "us" << endl;

    return 0;
}
