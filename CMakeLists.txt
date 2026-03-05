project(LRU)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LRU
        lru/src.hpp
        lru/class-integer.hpp
        lru/class-matrix.hpp
        lru/utility.hpp
        lru/exceptions.hpp
        lru/lru.hpp
        test/8.cpp
)