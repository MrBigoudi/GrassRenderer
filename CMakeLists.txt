cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(grassRendering)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
                           "${PROJECT_BINARY_DIR}"
                          )

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenMP REQUIRED)

add_subdirectory(src)
add_subdirectory(dep)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} OpenMP::OpenMP_CXX)