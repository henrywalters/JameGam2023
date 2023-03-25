cmake_minimum_required(VERSION 3.24)
project(JameGam)

find_package(PkgConfig REQUIRED)

if(EMSCRIPTEN)
    MESSAGE("BUILDING FOR THE WEB")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    find_package(X11)
    if(NOT X11_FOUND)
        message("ERROR: x11 not found")
    endif(NOT X11_FOUND)
endif()

find_package(PkgConfig REQUIRED)
#
find_package(OpenGL REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(HaGame2 REQUIRED)

add_executable(JameGam main.cpp src/game.hpp src/assets.hpp src/scenes/runtime.hpp src/utils/dice.h src/map.h)

# add_library(JameGam SHARED library.cpp components/target.h scripts/JameGam.cpp)

include_directories(${X11_INCLUDE_DIR})

target_include_directories(JameGam PUBLIC ${HAGAME2_INCLUDE_DIR})
target_link_libraries(JameGam ${HAGAME2_LIBRARY})
target_link_libraries(JameGam ${GLFW_LIBRARIES})
target_link_libraries(JameGam ${OPENGL_LIBRARIES})
target_link_libraries(JameGam ${X11_LIBRARIES})
target_link_libraries(JameGam ${BOX2D_LIBRARY})

# set(CMAKE_TOOLCHAIN_FILE /home/henry/emsdk/)
if(EMSCRIPTEN)
    set_target_properties(JameGam PROPERTIES LINK_FLAGS "-DCMAKE_EXE_LINKER_FLAGS=-s -sMAX_WEBGL_VERSION=2 -sUSE_GLFW=3 -sTOTAL_MEMORY=512MB --preload-file ./../assets")
endif()