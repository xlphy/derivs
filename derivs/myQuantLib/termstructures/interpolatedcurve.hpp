//
//  interpolatedcurve.hpp
//  derivs
//
//  Created by Xin Li on 3/16/22.
//

#ifndef interpolatedcurve_hpp
#define interpolatedcurve_hpp

#include <vector>
#include "../interpolations/interpolation.hpp"
#include "../date.hpp"

namespace myQuantLib {

//! Helper class to build interpolated term structures
/*! Interpolated term structures can use proected or private
    inheritance from this class to obtain the relevant data
    members and implement correct copy behavior.
*/

// Interpolate: 1. traits class; 2. factory that produces interpolation instance given data points

template <class Interpolator>
class InterpolatedCurve {
protected:
    InterpolatedCurve(std::vector<double> times,
                      std::vector<double> data,
                      const Interpolator& i = Interpolator())
    : _times(std::move(times)), _data(std::move(data)), _interpolator(i) {}
    
    InterpolatedCurve(const std::vector<double>& times,
                      const Interpolator& i = Interpolator())
    : _times(times), _data(times.size()), _interpolator(i){}
    
    InterpolatedCurve(std::size_t n,
                      const Interpolator& i = Interpolator())
    : _times(n), _data(n), _interpolator(i) {}
    
    InterpolatedCurve(const Interpolator& i = Interpolator()): _interpolator(i) {}
    
    // copy
    InterpolatedCurve(const InterpolatedCurve& c)
    : _times(c._times), _data(c._data), _interpolator(c._interpolator){
        setup_interpolation();
    }
    
    InterpolatedCurve& operator=(const InterpolatedCurve& c) {
        _times = c._times;
        _data = c._data;
        _interpolator = c._interpolator;
        setup_interpolation();
        return *this;
    }
    
    void setup_interpolation() {
        _interpolation = _interpolator.interpolate(_times.begin(),
                                                   _times.end(),
                                                   _data.begin());
    }
    
    mutable std::vector<double> _times;
    mutable std::vector<double> _data;
    mutable Interpolation _interpolation;
    Interpolator _interpolator;
    
    // Usually, the maximum date is the one corresponding to the
    // last node. However, it might happen that a bit of
    // extrapolation is used by construction; for instance, when a
    // curve is bootstrapped and the last relevant date for an
    // instrument is after the corresponding pillar.
    // We provide here a slot to store this information, so that
    // it's available to all derived classes (we should have
    // probably done the same with the dates_ vector, but moving
    // it here might not be entirely backwards-compatible).
    Date _max_date;
};



}


#endif /* interpolatedcurve_hpp */
