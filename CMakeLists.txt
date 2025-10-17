cmake_minimum_required(VERSION 3.10)
project(Space_Shooter C)

set(CMAKE_C_STANDARD 11)

# On cherche SDL2, la version stable
find_package(SDL2 REQUIRED)

add_executable(Space_Shooter WIN32 main.c)

# On lie à la cible SDL2::SDL2
target_link_libraries(Space_Shooter PRIVATE SDL2::SDL2main SDL2::SDL2)
