//
//  interpolation.hpp
//  derivs
//
//  Created by Xin Li on 3/16/22.
//

#ifndef interpolation_hpp
#define interpolation_hpp

#include "extrapolation.hpp"
#include <vector>
#include <memory>
#include "../errors.hpp"
#include <algorithm>

namespace myQuantLib {

//! base class for 1-D interpolations.
/*! Classes derived from this class will provide interpolated
    values from two sequences of equal length, representing
    discretized values of a variable and a function of the former,
    respectively.

    \warning Interpolations don't copy their underlying data;
             instead, they store iterators through which they
             access them.  This allow them to see changes in the
             underlying data without having to propagate them
             manually, but adds the requirement that the lifetime
             of the underlying data exceeds or equals the lifetime
             of the interpolation. It is up to the user to ensure
             this: usually, a class will store as data members
             both the data and the interpolation (see, e.g., the
             InterpolatedCurve class) and call the update() method
             on the latter when the data change.
*/

class Interpolation : public Extrapolator {
protected:
    // abstract base class for implementation
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual void update() = 0;
        virtual double xmin() const = 0;
        virtual double xmax() const = 0;
        virtual std::vector<double> xvalues() const = 0;
        virtual std::vector<double> yvalues() const = 0;
        virtual bool is_in_range(double x) const = 0;
        // depending on implementation methods
        virtual double value(double x) const = 0;
        virtual double primitive(double) const = 0;
        virtual double derivative(double) const = 0;
        virtual double second_derivative(double) const = 0;
    };
    std::shared_ptr<Impl> _impl;
public:
    typedef double argument_type;
    typedef double result_type;
    
    // provide a basic template implementation,
    // leftout methods: value, primitive, derivative, second_derivative
    // takes in two iterator classes
    // I1 -> argument_type iterator, I2 -> result_type iterator
    template<class I1, class I2>
    class templateImpl: public Impl {
    public:
        templateImpl(const I1& xbegin, const I1& xend,
                     const I2& ybegin,
                     const int required_points = 2)
        :_xbegin(xbegin), _xend(xend), _ybegin(ybegin){
            myQL_REQUIRE(static_cast<int>(_xend - _xbegin) >= required_points,
                         "not enough points to interpolate: at least " << required_points << "required, " << static_cast<int>(_xend - _xbegin) << " provided");
        }
        
        double xmin() const override {return *_xbegin;}
        double xmax() const override {return *(_xend - 1);}
        std::vector<double> xvalues() const override {
            return std::vector<double>(_xbegin, _xend);
        }
        std::vector<double> yvalues() const override {
            return std::vector<double>(_ybegin, _ybegin + (_xend - _xbegin));
        }
        bool is_in_range(double x) const override {
            // check increasing order of input xvalues
            for (I1 i = _xbegin, j = _xbegin + 1; j != _xend; ++i, ++j){
                myQL_REQUIRE(*j > *i, "unsorted x values");
            }
            double x1 = xmin(), x2 = xmax();
            return x >= x1 && x<= x2;
        }
        
    protected:
        std::size_t locate(double x) const {
            for (I1 i=_xbegin, j=_xbegin+1; j!=_xend; ++i, ++j)
                myQL_REQUIRE(*j > *i, "unsorted x values");
            if (x < *_xbegin)
                return 0;
            else if (x > *(_xend - 1))
                return _xend - _xbegin - 2;
            else
                return std::upper_bound(_xbegin, _xend-1, x) - _xbegin - 1;
        }
        I1 _xbegin, _xend;
        I2 _ybegin;
    };
    
    // use Impl methods
    Interpolation() = default;
    ~Interpolation() override = default;
    bool empty() const {return !_impl;}
    double operator()(double x, bool allow_extrapolation = false) const {
        check_range(x, allow_extrapolation);
        return _impl->value(x);
    }
    double primitive(double x, bool allow_extrapolation = false) const {
        check_range(x, allow_extrapolation);
        return _impl->primitive(x);
    }
    double derivative(double x, bool allow_extrapolation = false) const {
        check_range(x,allow_extrapolation);
        return _impl->derivative(x);
    }
    double secondDerivative(double x, bool allowExtrapolation = false) const {
        check_range(x,allowExtrapolation);
        return _impl->second_derivative(x);
    }
    double xMin() const {
        return _impl->xmin();
    }
    double xMax() const {
        return _impl->xmax();
    }
    bool isInRange(double x) const {
        return _impl->is_in_range(x);
    }
    void update() {
        _impl->update();
    }
    
    
protected:
    void check_range(double x, bool extrapolate) const {
        myQL_REQUIRE(extrapolate || allow_extrapolation() || _impl->is_in_range(x),
                     "interpolation range is [" << _impl->xmin() << ", " << _impl->xmax() << "]: extrapolation at " << " not allowed");
    }
    
};





}

#endif /* interpolation_hpp */
