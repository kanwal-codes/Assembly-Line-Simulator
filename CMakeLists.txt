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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Source files
set(SOURCES
    src/Station.cpp
    src/CustomerOrder.cpp
    src/Workstation.cpp
    src/LineManager.cpp
    src/Utilities.cpp
)

set(HEADERS
    src/Station.h
    src/CustomerOrder.h
    src/Workstation.h
    src/LineManager.h
    src/Utilities.h
)

# Main executable target
add_executable(assembly_line 
    ${SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(assembly_line PRIVATE src)

# Set target properties
set_target_properties(assembly_line PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "assembly_line"
)

# Enable testing
enable_testing()

# Test executables
add_executable(test_station 
    tests/tester_1.cpp
    ${SOURCES}
)
target_include_directories(test_station PRIVATE src)

add_executable(test_customer_order 
    tests/tester_2.cpp
    ${SOURCES}
)
target_include_directories(test_customer_order PRIVATE src)

add_executable(test_full_system 
    tests/tester_3.cpp
    ${SOURCES}
)
target_include_directories(test_full_system PRIVATE src)

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

# Package configuration (optional)
set(CPACK_PACKAGE_NAME "AssemblyLineSimulator")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

include(CPack)