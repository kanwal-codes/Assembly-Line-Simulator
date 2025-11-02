cmake_minimum_required(VERSION 3.10)
project(AssemblyLineSimulator 
        VERSION 1.0.0
        DESCRIPTION "Advanced C++ Assembly Line Simulation"
        LANGUAGES CXX)

# Set C++ standard and compiler requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Library source files (core and infrastructure, excluding main.cpp)
set(CORE_SOURCES
    src/core/Station.cpp
    src/core/CustomerOrder.cpp
    src/core/Workstation.cpp
    src/core/LineManager.cpp
    src/core/Utilities.cpp
)

set(INFRA_SOURCES
    src/infrastructure/Logger.cpp
    src/infrastructure/Config.cpp
    src/infrastructure/Database.cpp
)

set(LIBRARY_SOURCES
    ${CORE_SOURCES}
    ${INFRA_SOURCES}
)

set(HEADERS
    include/seneca/Station.h
    include/seneca/CustomerOrder.h
    include/seneca/Workstation.h
    include/seneca/LineManager.h
    include/seneca/Utilities.h
    include/seneca/Logger.h
    include/seneca/Config.h
    include/seneca/Database.h
    include/seneca/Exceptions.h
)

# Find SQLite3
find_library(SQLITE3_LIB sqlite3)

# Create library from core sources
add_library(assembly_line_lib ${LIBRARY_SOURCES} ${HEADERS})
target_include_directories(assembly_line_lib PUBLIC include)
target_link_libraries(assembly_line_lib ${SQLITE3_LIB})

# Main executable
add_executable(assembly_line 
    src/main.cpp
)
target_link_libraries(assembly_line assembly_line_lib)

# Set target properties
set_target_properties(assembly_line PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "assembly_line"
)

# Enable testing
enable_testing()

# Test executables - link against the library
add_executable(test_station 
    tests/tester_1.cpp
)
target_link_libraries(test_station assembly_line_lib)

add_executable(test_customer_order 
    tests/tester_2.cpp
)
target_link_libraries(test_customer_order assembly_line_lib)

add_executable(test_full_system 
    tests/tester_3.cpp
)
target_link_libraries(test_full_system assembly_line_lib)

# Register tests with CTest
add_test(NAME StationTests 
         COMMAND test_station 
         ${CMAKE_SOURCE_DIR}/data/Stations1.txt 
         ${CMAKE_SOURCE_DIR}/data/Stations2.txt
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME CustomerOrderTests 
         COMMAND test_customer_order 
         ${CMAKE_SOURCE_DIR}/data/Stations1.txt 
         ${CMAKE_SOURCE_DIR}/data/Stations2.txt 
         ${CMAKE_SOURCE_DIR}/data/CustomerOrders.txt
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME FullSystemTests 
         COMMAND test_full_system 
         ${CMAKE_SOURCE_DIR}/data/Stations1.txt 
         ${CMAKE_SOURCE_DIR}/data/Stations2.txt 
         ${CMAKE_SOURCE_DIR}/data/CustomerOrders.txt 
         ${CMAKE_SOURCE_DIR}/data/AssemblyLine.txt
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Custom target for running all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS test_station test_customer_order test_full_system
    COMMENT "Running all tests"
)

# Installation rules (optional)
install(TARGETS assembly_line
        RUNTIME DESTINATION bin
        COMPONENT Runtime)

install(FILES ${HEADERS}
        DESTINATION include/assembly_line
        COMPONENT Development)

install(DIRECTORY data/
        DESTINATION share/assembly_line/data
        COMPONENT Runtime)