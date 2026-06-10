cmake_minimum_required(VERSION 3.15)
project(hanoip83 CXX)

find_package(raylib REQUIRED)
find_package(EnTT REQUIRED)

add_executable(hanoip83 src/hanoip83.cpp src/main.cpp)

target_link_libraries(hanoip83 PRIVATE
    raylib
    EnTT::EnTT
)

install(TARGETS hanoip83 DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
