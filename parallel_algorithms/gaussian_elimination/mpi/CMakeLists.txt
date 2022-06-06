add_executable(mpi_cyclic_striped_mapping_gaussian cyclic_striped_mapping/gaussian.cpp)
target_link_libraries(mpi_cyclic_striped_mapping_gaussian PRIVATE Threads::Threads MPI::MPI_CXX)

add_executable(mpi_naive_gaussian naive/gaussian.cpp)
target_link_libraries(mpi_naive_gaussian PRIVATE Threads::Threads MPI::MPI_CXX)
