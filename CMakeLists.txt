cmake_minimum_required(VERSION 3.10)
project(PhysarumEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 1. Ask the Operating System to find the graphics libraries you just installed
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# 2. Tell CMake to build your executable AND compile the external GLAD C file
add_executable(PhysarumEngine main.cpp extern/glad/src/glad.c)

# 3. Tell your compiler where to look for the GLAD headers and our Engine.hpp
target_include_directories(PhysarumEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/extern/glad/include
)

# 4. Link the OpenGL and GLFW system libraries directly to your engine
target_link_libraries(PhysarumEngine PRIVATE OpenGL::GL glfw)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Add it to the executible
target_link_libraries(PhysarumEngine PRIVATE OpenMP::OpenMP_CXX)