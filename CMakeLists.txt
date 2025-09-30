cmake_minimum_required(VERSION 3.15)
project(Courage VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${SRC_DIR}/*.cpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${INCLUDE_DIR}
)

option(ENABLE_DEBUG "Enable debug logging in the code" ON)
if(ENABLE_DEBUG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_DEBUG)
endif()

find_package(ZLIB)

if (ZLIB_FOUND)
    message(STATUS "Using system ZLIB")
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
else()
    message(STATUS "System ZLIB not found")
endif()

find_package(nlohmann_json QUIET)

if (nlohmann_json_FOUND)
    message(STATUS "Using system nlohmann_json")
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
else()
    message(STATUS "System nlohmann_json not found, falling back to external headers")
endif()

# mbedTLS
find_package(MbedTLS QUIET)

if (MbedTLS_FOUND)
    message(STATUS "Using system mbedTLS")
    target_link_libraries(${PROJECT_NAME} PRIVATE MbedTLS::mbedcrypto)
else()
    message(STATUS "System mbedTLS not found, using manual link")
    target_link_libraries(${PROJECT_NAME} PRIVATE mbedcrypto)
endif()

find_library(UUID_LIB uuid)
if (UUID_LIB)
    message(STATUS "Using system libuuid")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${UUID_LIB})
else()
    message(FATAL_ERROR "libuuid not found, please install uuid-dev")
endif()

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION include)
