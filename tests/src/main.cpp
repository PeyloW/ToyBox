//
//  main.cpp
//  toybox - tests
//
//  Created by Fredrik on 2025-10-12.
//

#include "shared.hpp"

int main(int argc, const char * argv[]) {
    // Test collections
    test_array_and_vector();
    test_dynamic_vector();
    test_list();
    
    // Test display list
    test_display_list();
    
    // Test algorithms
    test_algorithms();
    
    // Test math, especially fix16_t
    test_math();
    test_math_functions();
    
    // Test copy/move works as expected to lifetimes
    test_lifetime();

    printf("All pass.\n\r");
#ifndef TOYBOX_HOST
    while (getc(stdin) != ' ');
#endif
    return 0;
}
