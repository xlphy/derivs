//
//  exotic_engine.cpp
//  derivs
//
//  Created by Xin Li on 1/28/22.
//

#include "exotic_engine.hpp"

/*Black-Scholes engine: dS(t) = (r(t) - d(t)) * S(t) * dt + vol(t) * S(t) * dWt
 can be solved or integrated out analytically
 */
ExoticBSEngine::ExoticBSEngine
(const Wrapper<PathDependent>& _product,
  const Parameters& _r,  // interest rate
  const Parameters& _d,  // dividends
  const Parameters& _vol,
  const Wrapper<RandomBase>& _generator,
  double _spot0):ExoticEngine(_product, _r), generator(_generator)
{
    MJArray times(product->get_lookat_times());
    num_times = times.size();
    generator->reset_dim(num_times);
    drifts.resize(num_times);
    stds.resize(num_times);
    
    double var = _vol.integrate_square(0.0, times[0]);
    drifts[0] = _r.integrate(0.0, times[0]) - _d.integrate(0.0, times[0]) - 0.5 * var;
    stds[0] = std::sqrt(var);
    for(unsigned long j=1; j<num_times; ++j){
        double dvar = _vol.integrate_square(times[j-1], times[j]);
        drifts[j] = _r.integrate(times[j-1], times[j]) -_d.integrate(times[j-1], times[j]) - 0.5 * dvar;
        stds[j] = std::sqrt(dvar);
    }
    
    log_spot = std::log(_spot0);
    variates.resize(num_times);
}

void ExoticBSEngine::get_one_path(MJArray& spot_values)
{
    generator->get_gaussians(variates);
    double current_log_spot = log_spot;
    for(unsigned long j=0; j<num_times; ++j){
        current_log_spot += drifts[j];
        current_log_spot += stds[j] * variates[j];
        spot_values[j] = std::exp(current_log_spot);
    }
    return;
}
