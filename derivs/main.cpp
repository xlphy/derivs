//
//  main.cpp
//  derivs
//
//  Created by Xin Li on 1/11/22.
//
#include <iostream>
#include "test.hpp"


//#include <boost/math/special_functions/erf.hpp>

#include <memory>



int main(int argc, const char * argv[]) {
    int * p = new int(10);
    
    std::shared_ptr<int> a(p);
    std::cout << (*a.get()) << "\n";
    std::cout << (*a) << "\n";
    
    std::cout << a.use_count() << "\n";
    std::cout << (a.get() == nullptr) << "\n";
    
    {
        std::shared_ptr<int> b = a;
        std::cout << b.use_count() << "\n";
    }
    
    std::cout << a.use_count() << "\n";
    
    
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

