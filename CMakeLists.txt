cmake_minimum_required(VERSION 3.16)
project(RoutingSimulation CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(include)


file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")


if (CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    message(STATUS "Build Mode: DEBUG (Kích hoạt -Werror và AddressSanitizer)")
    add_compile_options(
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnon-virtual-dtor -Werror -g
    )
else()
    message(STATUS "Build Mode: RELEASE (Tối ưu hiệu năng, phục vụ vấn đáp)")
    add_compile_options(-Wall -Wextra -Wnon-virtual-dtor)
endif()


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    add_executable(app main.cpp ${CORE_SOURCES})
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp")
    add_executable(test_app tests/test_main.cpp ${CORE_SOURCES})
endif()