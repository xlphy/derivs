//
//  BlackScholes.cpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#include "BlackScholes.hpp"
#include <cmath>
#include "normals.hpp"

double bs_call(double spot, double strike, double r, double d, double vol, double ttx)
{
    double std = vol * std::sqrt(ttx);
    double lnm = std::log(spot/strike);
    double d1 = (lnm + (r-d)*ttx + 0.5*std*std) / std;
    double d2 = d1 - std;
    return spot * std::exp(-d*ttx) * cum_norm(d1) - strike * std::exp(-r*ttx) * cum_norm(d2);
}

double bs_put(double spot, double strike, double r, double d, double vol, double ttx)
{
    double std = vol * std::sqrt(ttx);
    double lnm = std::log(spot/strike);
    double d1 = (lnm + (r-d)*ttx + 0.5*std*std) / std;
    double d2 = d1 - std;
    return strike*std::exp(-r*ttx)*(1.0 - cum_norm(d2)) - spot*std::exp(-d*ttx) * (1.0-cum_norm(d1));
}

double bs_digital_call(double spot, double strike, double r, double d, double vol, double ttx)
{
    double std = vol * std::sqrt(ttx);
    double lnm = std::log(spot/strike);
    double d2 = (lnm + (r-d)*ttx - 0.5*std*std) / std;
    return std::exp(-r*ttx)*cum_norm(d2);
}

double bs_vega(double spot, double strike, double r, double d, double vol, double ttx)
{
    double std = vol * std::sqrt(ttx);
    double lnm = std::log(spot/strike);
    double d1 = (lnm + (r-d)*ttx + 0.5*std*std) / std;
    return spot * std::exp(-d*ttx) * std::sqrt(ttx) * norm_density(d1);
}
