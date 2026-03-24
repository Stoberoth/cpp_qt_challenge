cmake_minimum_required(VERSION 3.14)
project(UniquePtrDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Centraliser la sortie des binaires
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_AUTOMOC ON)
set(Qt6_DIR "C:/Qt/6.11.0/mingw_64/lib/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Core)

# 1️⃣ L'exécutable principal
# Assure-toi que ce fichier a un "int main()"
add_executable(unique_demo ../src/main.cpp ../include/Task.hpp)

# 2️⃣ L'exécutable de test
# SI tu utilises QtTest, il faut lier les libs, sinon le main sera manquant !
add_executable(uniqueptr_test ../tests/uniqueptr_test.cpp)


target_link_libraries(unique_demo PRIVATE Qt6::Core)
target_link_libraries(unique_demo PRIVATE Qt6::Core)

# --- Correction des flags (Windows / MSVC n'aime pas -Wall) ---
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
    target_compile_options(unique_demo PRIVATE /W4)
    target_compile_options(uniqueptr_test PRIVATE /W4)
else()
    target_compile_options(unique_demo PRIVATE)
    target_compile_options(uniqueptr_test PRIVATE)
endif()