find_package(OpenGL REQUIRED)

if(NOT WIN32)
    set(CPM_LOCAL_PACKAGES_ONLY True)
endif()

set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_LIST_DIR}/third_party/CPM)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchCPM.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM/CPM.cmake)
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(NAME glfw3
    GITHUB_REPOSITORY glfw/glfw
    VERSION 3.3.1
    GIT_TAG ecda86fa4f89ecdd364e5a1a22645030fe0ced6e
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
        "BUILD_SHARED_LIBS ON"
        "GLFW_INSTALL OFF")

CPMAddPackage(NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.10.0
    GIT_TAG v3.10.0)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/glad)
