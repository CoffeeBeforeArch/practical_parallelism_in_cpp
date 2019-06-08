# Practical Parallelism in C++
This repository contains all code from the YouTube series "Practical Parallelism in C++" by CoffeeBeforeArch. The general flow this course was taken from the book C++ Concurrency in Action.

## Contact

Suggestions for specific content can be sent to: CoffeeBeforeArch@gmail.com


## Environment 
Operating System: Windows 10 & Ubuntu 18.04

IDE: Visual Studio 2017

Text Editor: VIM

Thread Interfaces: Windows API, Pthreads, C++11 Standard, MPI

## Concepts covered in each video
# C++ Threads
| Video | Concepts | Files |
| ----- | -------- | ----- |
|<a href=https://youtu.be/diEAEFly9ak>Practical Parallelism in C++: Introduction </a>| Threads, Concurrency, Parallelism | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/intro/intro/call_from.cpp>call_from.cpp</a> |
|<a href=https://youtu.be/V9zrbR_OzIA>Practical Parallelism in C++: Mutex Basics </a>| Mutex Locks, Critical Sections | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/lock_basics/lock_basics/lock_basics.cpp>mutex_basics.cpp</a> |

# Windows Threads
| Video | Concepts | Files |
| ----- | -------- | ----- |
|<a href=https://youtu.be/vJMj7-yxAfQ>Practical Parallelism in C++: Windows Threads </a>| Windows Threads, Thread-Safe Functions | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/windows_threads/windows_threads/windows_threads.cpp>windows_threads.cpp</a> |

# Pthreads
| Video | Concepts | Files |
| ----- | -------- | ----- |
|<a href=https://youtu.be/vJMj7-yxAfQ>Practical Parallelism in C++: Basic Pthreads </a>| Pthreads | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/pthreads/posix_threads.cpp>posix_threads.cpp</a> |
|<a href=https://youtu.be/6WN-fHN5O7s>Practical Parallelism in C++: Broadcast Parallel Gaussian Elimination </a>| Gaussian Elimination, Broadcast Parallel, Pthread Barriers | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/pthreads/naive/gaussian.cpp>gaussian.cpp</a><br><a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/pthreads/naive/utils.h>utils.h</a><br><a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/common/common.h>common.h</a> |
|<a href=https://youtu.be/SPuBFkcUURY>Practical Parallelism in C++: Broadcast-Parallel Gaussian Elimination with Cyclic Mapping </a>| Gaussian Elimination, Broadcast Parallel, Pthread Barriers, Cyclic Striped Mapping | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/pthreads/cyclic_striped_mapping/gaussian.cpp>gaussian.cpp</a><br><a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/pthreads/naive/utils.h>utils.h</a><br><a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/common/common.h>common.h</a> |

# MPI
| Video | Concepts | Files |
| ----- | -------- | ----- |
|<a href=https://youtu.be/a0V8KpLu7EY>Practical Parallelism in C++: MPI Basics </a>| Message Passing, Shared Memory, Ranks, Communicators | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/mpi/mpi_hello.cpp>mpi_hello.cpp</a> |
|<a href=https://youtu.be/YVZq25G4p_g>Practical Parallelism in C++: MPI Synchronization </a>| Synchronization, Sending Messages, Receiving Messages, Buffering | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/mpi/mpi_hello_synchronize.cpp>mpi_hello_synchronize.cpp</a> |
|<a href=https://youtu.be/NApk1276GO4>Practical Parallelism in C++: MPI Gaussian Elimination Naive </a>| Broadcast, Scatter, Gather | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/mpi/naive/gaussian.cpp>gaussian.cpp</a> |
|<a href=https://youtu.be/HRe7gLr_80M>Practical Parallelism in C++: MPI Gaussian Elimination Cyclic Striped </a>| Broadcast, Scatter, Gather | <a href=https://github.com/CoffeeBeforeArch/practical_parallelism_in_cpp/blob/master/parallel_algorithms/gaussian_elimination/mpi/cyclic_striped_mapping/gaussian.cpp>gaussian.cpp</a> |
