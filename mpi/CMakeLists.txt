add_executable(mpi_hello mpi_hello.cpp)
target_link_libraries(mpi_hello PRIVATE Threads::Threads)

add_executable(mpi_hello_synchronize mpi_hello_synchronize.cpp)
target_link_libraries(mpi_hello_synchronize PRIVATE Threads::Threads)