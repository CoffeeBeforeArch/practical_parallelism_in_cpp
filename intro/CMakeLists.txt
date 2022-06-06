add_executable(call_from call_from.cpp)
target_link_libraries(call_from PRIVATE Threads::Threads)

add_executable(lock_basics lock_basics.cpp)
target_link_libraries(lock_basics PRIVATE Threads::Threads)

add_executable(modern_lock modern_lock.cpp)
target_link_libraries(modern_lock PRIVATE Threads::Threads)