#include <iostream>
#include <cassert>

void test_unique_ptr_destruction() {
    // Ton test ici
    assert(true); 
    std::cout << "Test destruction : OK" << std::endl;
}

int main() {
    std::cout << "Lancement des tests unitaires..." << std::endl;
    
    test_unique_ptr_destruction();
    
    std::cout << "Tous les tests ont réussi !" << std::endl;
    return 0; // Succès
}