cmake_minimum_required(VERSION 3.14)
project(UniquePtrDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Centraliser la sortie des binaires
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 1️⃣ L'exécutable principal
# Assure-toi que ce fichier a un "int main()"
add_executable(unique_demo ../src/unique_ptr_demo.cpp)

# 2️⃣ L'exécutable de test
# SI tu utilises QtTest, il faut lier les libs, sinon le main sera manquant !
add_executable(uniqueptr_test ../tests/uniqueptr_test.cpp)

# --- Correction des flags (Windows / MSVC n'aime pas -Wall) ---
if(MSVC)
    target_compile_options(unique_demo PRIVATE /W4)
    target_compile_options(uniqueptr_test PRIVATE /W4)
else()
    target_compile_options(unique_demo PRIVATE -Wall -Wextra)
    target_compile_options(uniqueptr_test PRIVATE -Wall -Wextra)
endif()