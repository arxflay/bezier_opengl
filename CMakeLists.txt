cmake_minimum_required(VERSION 3.10.2)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake)
project(bezier VERSION ${VER_MAJOR}.${VER_MINOR} LANGUAGES C CXX)

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

list(APPEND SOURCES
    ${SOURCE_PATH}/main.cpp
    ${SOURCE_PATH}/Shader.cpp
    ${SOURCE_PATH}/AppConfiguration.cpp
    ${SOURCE_PATH}/Point.cpp
    ${SOURCE_PATH}/Color.cpp
    ${SOURCE_PATH}/Size.cpp
    ${SOURCE_PATH}/MathExt.cpp
)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
set(OpenGL_GL_PREFERENCE LEGACY)

add_executable(bezier ${SOURCES})

target_compile_options(bezier PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17>"  
                                                        "$<${msvc_cxx}:/W3;/std:c++17>")

target_include_directories(bezier PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(${CMAKE_CURRENT_SOURCE_DIR}/libs/ProjectLibs.cmake)

if (WIN32)
    target_link_libraries(bezier PUBLIC glm glfw nlohmann_json::nlohmann_json glad OpenGL::GL)
else()
    target_link_libraries(bezier PUBLIC glm glfw nlohmann_json::nlohmann_json glad OpenGL::GL m pthread)
endif()
