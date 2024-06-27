cmake_minimum_required(VERSION 3.27)
project(opengl_learn)

set(CMAKE_CXX_STANDARD 17)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    message(STATUS "Found GLEW: ${GLEW_INCLUDE_DIRS}")
endif()

find_package(glfw3 3.3 REQUIRED)
if (glfw3_FOUND)
    message(STATUS "Found GLFW: ${GLFW_INCLUDE_DIRS}")
endif()

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    message(STATUS "Found OpenGL: ${OPENGL_INCLUDE_DIR}")
endif()

include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_executable(opengl_learn main.cpp
        utils.h)

target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} glfw OpenGL::GL)