#include <iostream>
#include <memory>
#include <vector>
#include "../include/Task.hpp"

int main()
{
    // Allocation de 3 pointeur en utilisant different smart pointer
    auto ptr1 = std::make_unique<std::string>("first");
    auto shared = std::make_shared<std::string>("second");
    std::weak_ptr<std::string> weak = shared;

    // print the content
    std::cout << *ptr1 << "\n";
    std::cout << *shared << "\n";

    // Démontre que l'objet pointé par weak existe encore
    if (!weak.expired())
    {
        std::cout << *weak.lock() << "\n";
    }

    std::cout << "Use count of 'shared' : " << shared.use_count() << "\n";
    return 0; 
}