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

find_package(ZLIB)

if (ZLIB_FOUND)
    message(STATUS "Using system ZLIB")
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
else()
    message(STATUS "System ZLIB not found")
endif()

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION include)
