//
//  main.cpp
//  derivs
//
//  Created by Xin Li on 1/11/22.
//
#include <iostream>
#include "test.hpp"


//#include <boost/math/special_functions/erf.hpp>



int main(int argc, const char * argv[]) {
    
    std::cout << (7 % 7 + 1) << "\n";
    //test_simpleMC();
    //test_exoticEngine();
    //test_tree();
    //test_solver();
    //test_factory();
    //std::cout << boost::math::erf(0.5) << std::endl;
    
    double tmp;
    std::cout << "\nenter a number to exit:";
    std::cin >> tmp; // wait for an input to exit
    
    return 0;
}

