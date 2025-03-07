# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(GLFW_TEST)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile options
add_compile_options(-Wall -Wextra -O3)

# Allow the user to specify the GLFW path (useful for custom installations)
# Default to searching in standard locations
find_path(GLFW_INCLUDE_DIR NAMES glfw3.h PATHS /usr/include/GLFW)
find_library(GLFW_LIBRARIES glfw)

find_path(GLM_INCLUDE_DIR NAMES glm PATHS /usr/include/glm)
find_library(GLM_LIBRARIES glm)

# If the GLFW include directory is found, print its path
if (GLFW_INCLUDE_DIR)
	message(STATUS "GLFW include found at: ${GLFW_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "GLFW include directory not found.")
endif()

# If the GLFW library file is found, print its path
if (GLFW_LIBRARIES)
	message(STATUS "GLFW library file found at: ${GLFW_LIBRARIES}")
else()
	message(FATAL_ERROR "GLFW library file not found.")
endif()

# If the GLM include directory is found, print its path
if (GLM_INCLUDE_DIR)
	message(STATUS "GLM include found at: ${GLM_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "GLM include directory not found.")
endif()

# If the GLM library file is found, print its path
if (GLM_LIBRARIES)
	message(STATUS "GLM library file found at: ${GLM_LIBRARIES}")
else()
	message(FATAL_ERROR "GLM library file not found.")
endif()

# Include directories
include_directories(${GLFW_INCLUDE_DIR} include)
#include_directories(${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} include)

# Add the source files
add_executable(test main.cpp src/glad.c)

# Link the PAPI library
#target_link_libraries(test ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
target_link_libraries(test ${GLFW_LIBRARIES})

