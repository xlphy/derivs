//
//  func_obj.cpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#include "func_obj.hpp"
#include "BlackScholes.hpp"

double BSCall::operator()(double vol) const
{
    return bs_call(spot, strike, r, d, vol, ttx);
}

double BSPut::operator()(double vol) const
{
    return bs_put(spot, strike, r, d, vol, ttx);
}

double BSCall2::price(double vol) const
{
    return bs_call(spot, strike, r, d, vol, ttx);
}

double BSCall2::vega(double vol) const
{
    return bs_vega(spot, strike, r, d, vol, ttx);
}

double BSPut2::price(double vol) const
{
    return bs_put(spot, strike, r, d, vol, ttx);
}

double BSPut2::vega(double vol) const
{
    return bs_vega(spot, strike, r, d, vol, ttx);
}
