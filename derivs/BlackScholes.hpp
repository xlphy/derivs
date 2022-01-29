//
//  BlackScholes.hpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#ifndef BlackScholes_hpp
#define BlackScholes_hpp

double bs_call(double spot, double strike, double r, double d, double vol, double ttx);
double bs_put(double spot, double strike, double r, double d, double vol, double ttx);
double bs_digital_call(double spot, double strike, double r, double d, double vol, double ttx);
double bs_call_vega(double spot, double strike, double r, double d, double vol, double ttx);

#endif /* BlackScholes_hpp */
