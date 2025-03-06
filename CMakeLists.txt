cmake_minimum_required(VERSION 3.29)
project(GenET_enrchssement)

set(GLUT_INCLUDE_DIR "C:/MinGW/include")
set(GLUT_LIBRARY "C:/MinGW/lib/libfreeglut.a")

include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY})

set(CMAKE_CXX_STANDARD 20)

add_executable(GenET_enrchssement main.cpp)
target_link_libraries(GenET_enrchssement opengl32 glu32)
target_link_libraries(GenET_enrchssement ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})