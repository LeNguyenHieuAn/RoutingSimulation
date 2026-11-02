cmake_minimum_required(VERSION 3.16)
project(RoutingSimulation CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/*.hpp")


if (CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    message(STATUS "Build Mode: DEBUG")
    add_compile_options(
        -pedantic -Wall -Wextra -Wshadow -Wconversion -Wnon-virtual-dtor -Werror -g
    )
else()
    message(STATUS "Build Mode: RELEASE")
    add_compile_options(
        -pedantic -Wall -Wextra -Wnon-virtual-dtor -O3 -DNDEBUG
    )
endif()


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    add_executable(app main.cpp ${CORE_SOURCES} ${CORE_HEADERS})
    target_include_directories(app PRIVATE 
        include
        include/management
        include/routing
        include/core
    )
endif()


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp")
    add_executable(test_app tests/test_main.cpp ${CORE_SOURCES} ${CORE_HEADERS})
    target_include_directories(test_app PRIVATE 
        include
        include/management
        include/routing
        include/core
    )
endif()