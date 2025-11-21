cmake_minimum_required(VERSION 3.10)
project(Space_Shooter C)

set(CMAKE_C_STANDARD 11)

# On cherche les bibliothèques
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(Space_Shooter main.c player.c
        background.c
        background.h
        projectile.c
        projectile.h
        enemy.c
        enemy.h)

target_link_libraries(Space_Shooter PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image)