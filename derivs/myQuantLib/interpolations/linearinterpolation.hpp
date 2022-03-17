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
    // constructor, value, primitive, derivative, second_derivative
    LinearInterpolationImpl(const I1& xbegin, const I1& xend, const I2& ybegin);
    void update() override {
        _primitive_const[0] = 0.0;
        for(std::size_t i=1; i < std::size_t(this->_xend - this->_xbegin); ++i){
            double dx = this->_xbegin[i] - this->_xbegin[i-1];
            _s[i-1] = (this->_ybegin[i] - this->_ybegin[i-1]) / dx;
            _primitive_const[i] = _primitive_const[i-1]
            + dx * (this->_ybegin[i-1] + 0.5 * dx * _s[i-1]);
        }
    }
    double value(double x) const override {
        std::size_t i = this->locate(x);
        return this->_ybegin[i] + (x - this->_xbegin[i]) * _s[i];
    }
    double primitive(double x) const override {
        std::size_t i = this->locate(x);
        double dx = x - this->_xbegin[i];
        return _primitive_const[i] + dx * (this->_ybegin[i] + 0.5 * dx * _s[i]);
    }
    double derivative(double x) const override {
        std::size_t i = this->locate(x);
        return _s[i];
    }
    double second_derivative(double x) const override {return 0.0;}
    
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

// inline definition
template<class I1, class I2>
LinearInterpolationImpl<I1, I2>::LinearInterpolationImpl(const I1& xbegin, const I1& xend,
                                                         const I2& ybegin)
:Interpolation::templateImpl<I1, I2>(xbegin, xend, ybegin, Linear::required_points),
_primitive_const(xend - xbegin), _s(xend - xbegin) {}

}


#endif /* linearinterpolation_hpp */
