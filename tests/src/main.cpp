//
//  main.cpp
//  toybox - tests
//
//  Created by Fredrik on 2025-10-12.
//

#include "shared.hpp"

int main(int argc, const char * argv[]) {

    test_array_and_vector();
    test_dynamic_vector();
    
    test_display_list();
    
    test_algorithms();
    
    test_math();
    test_math_functions();
    
    test_lifetime();

    printf("All pass.\n\r");
#ifndef TOYBOX_HOST
    while (getc(stdin) != ' ');
#endif
    return 0;
}
