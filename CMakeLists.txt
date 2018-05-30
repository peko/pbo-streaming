cmake_minimum_required(VERSION 3.6)

set(APP_NAME pbo-webcam)

# libendencies
# glfw
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

# flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wformat -g")
#set(CMAKE_BUILD_TYPE "Debug")

# source files
file(GLOB src   "src/*.h" "src/*.cpp")
file(GLOB imgui "lib/imgui/*.h" "lib/imgui/*.cpp")
file(GLOB gl3w  "lib/gl3w/GL/*.h" "lib/gl3w/GL/*.c")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# includes
include_directories(src lib/imgui lib/gl3w)

# APP
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(${APP_NAME} ${src} ${imgui} ${gl3w})
target_link_libraries(
        ${APP_NAME}
        ${GLFW_LIBRARIES}
        GL pthread dl)


