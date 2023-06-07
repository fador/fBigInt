cmake_minimum_required(VERSION 3.10)
project(FBigInt VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Create a variable containing all your sources
set(SOURCES
        primetest.cpp
        )

# Create a variable containing all your headers
set(HEADERS
        fbigint.hpp
        )

# Adding the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(primetest ${SOURCES})

if(UNIX)
  install(FILES ${HEADERS} DESTINATION /usr/local/include)
elseif(WIN32)
  install(FILES ${HEADERS} DESTINATION C:/Program Files/${PROJECT_NAME}/include)
endif()


# Define the testing logic
enable_testing()
add_test(NAME Primetest COMMAND primetest)