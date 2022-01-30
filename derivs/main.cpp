//
//  main.cpp
//  derivs
//
//  Created by Xin Li on 1/11/22.
//
#include <iostream>
#include "test.hpp"

int main(int argc, const char * argv[]) {
    
    //test_simpleMC();
    //test_exoticEngine();
    //test_tree();
    test_solver();
    
    double tmp;
    std::cout << "enter a number to exit:";
    std::cin >> tmp; // wait for an input to exit
    
    return 0;
}

