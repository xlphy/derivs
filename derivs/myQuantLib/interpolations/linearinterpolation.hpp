//
//  linearinterpolation.hpp
//  derivs
//
//  Created by Xin Li on 3/16/22.
//

#ifndef linearinterpolation_hpp
#define linearinterpolation_hpp

#include "interpolation.hpp"

namespace myQuantLib {

template<class I1, class I2>
class LinearInterpolationImpl: public Interpolation::templateImpl<I1, I2>{
public:
    //TODO: implement constructor, value, primitive, derivative, second_derivative
    
private:
    std::vector<double> _primitive_const, _s;
};

class LinearInterpolation: public Interpolation{
public:
    template <class I1, class I2>
    LinearInterpolation(const I1& xbegin, const I1& xend,
                        const I2& ybegin){
        _impl = std::shared_ptr<Interpolation::Impl>
        (new LinearInterpolationImpl<I1, I2>(xbegin, xend, ybegin));
        _impl->update();
    }
};

class Linear {
public:
    template <class I1, class I2>
    Interpolation interpolate(const I1& xbegin, const I1& xend,
                              const I2& ybegin) const {
        return LinearInterpolation(xbegin, xend, ybegin);
    }
    static const bool global = false;
    static const std::size_t required_points = 2;
};

}


#endif /* linearinterpolation_hpp */
