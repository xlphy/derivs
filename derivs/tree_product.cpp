//
//  tree_product.cpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#include "tree_product.hpp"
#include <cmath>

double TreeAmerican::pre_final_value(double spot, double time, double disc_fut_val) const
{
    return std::fmax(payoff(spot), disc_fut_val);
}

double TreeEuropean::pre_final_value(double spot, double time, double disc_fut_val) const
{
    return disc_fut_val;
}


