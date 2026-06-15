cmake_minimum_required(VERSION 3.15)
project(hanoip83 CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(raylib REQUIRED)
find_package(EnTT REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(hanoip83 ${SOURCES})

target_include_directories(hanoip83 PRIVATE src)

target_link_libraries(hanoip83 PRIVATE
    raylib
    EnTT::EnTT
)

install(TARGETS hanoip83 DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
