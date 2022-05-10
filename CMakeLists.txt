cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_STANDARD 11)

add_compile_options(
       "-Wall"
       "$<$<CONFIG:RELEASE>:-O3>"
       "$<$<CONFIG:DEBUG>:-DENABLE_OPENGL_DEBUG -g>"
)

project(Physarum)

SET(Physarum_SRC
    src/main.c
    src/deps/glad/glad.c
)

SET(Physarum_INCLUDE_DIRS
    include/deps/
    include/deps/glad/
)

find_package(SDL2 REQUIRED SDL2)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

include_directories(${Physarum_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRs})

add_executable(Physarum_Executable ${Physarum_SRC})

if(OpenGL_FOUND)
    target_link_libraries(Physarum_Executable ${OPENGL_LIBRARIES})
endif(OpenGL_FOUND)

if (SDL2_FOUND)
    target_link_libraries(Physarum_Executable SDL2::SDL2)
endif (SDL2_FOUND)