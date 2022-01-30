//
//  solver.hpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#ifndef solver_hpp
#define solver_hpp

/*generic solver functions, find a root of an equation, func(x) = target;
 use template to allow passing unkown types
 */

#include <cmath>

// bisection method
// require T implement operator()
template<typename T>
double bisection(double target, double low, double high, double tol, T func){
    double x = 0.5 * (low + high);
    double y = func(x);
    do{
        if (y < target)
            low = x;
        if (y > target)
            high = x;
        x = 0.5 * (low + high);
        y = func(x);
    }while(std::fabs(y-target) > tol);
    return x;
}

// Newton-Raphson method
// require T implement value and derivative member functions
template<typename T,
        double (T::*value)(double) const,
        double (T::*derivative)(double) const>
double newton(double target, double start, double tol, const T& obj){
    double y = (obj.*value)(start);
    double x = start;
    while(std::fabs(y-target) > tol){
        double d = (obj.*derivative)(x);
        x += (target - y) / d;
        y = (obj.*value)(x);
    }
    return x;
}



#endif /* solver_hpp */
