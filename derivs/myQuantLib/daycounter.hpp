//
//  daycounter.hpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#ifndef daycounter_hpp
#define daycounter_hpp

#include "date.hpp"
#include "errors.hpp"

namespace myQuantLib {

//! day counter class
/*! This class provides methods for determining the length of a time
    period according to given market convention, both as a number
    of days and as a year fraction.

    The Bridge pattern is used to provide the base behavior of the
    day counter.

*/

class DayCounter {
protected:
    // abstract base class for day counter implementation
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual std::string name() const = 0;
        // to be overloaded by more complex day counters
        virtual Date::serial_type day_count(const Date& d1, const Date& d2) const {
            return (d2 - d1);
        }
        virtual double year_fraction(const Date& d1, const Date& d2,
                                     const Date& ref_period_start,
                                     const Date& ref_period_end) const = 0;
    };
    
    std::shared_ptr<Impl> _impl;
    // constructor invoked by a derived class which defines a concrete implementation of Impl
    explicit DayCounter(std::shared_ptr<Impl> impl): _impl(std::move(impl)){}
    
public:
    // key interface delegated to inner _impl class
    DayCounter() = default;
    
    bool empty() const {return !_impl;}
    std::string name() const{
        myQL_REQUIRE(_impl, "no day counter implementation provided!");
        return _impl->name();
    }
    
    // Returns the number of days between two dates.
    Date::serial_type day_count(const Date& d1, const Date& d2) const {
        myQL_REQUIRE(_impl, "no day counter implementation provided!");
        return _impl->day_count(d1, d2);
    }
    
    // Returns the period between two dates as a fraction of year.
    double year_fraction(const Date& d1, const Date& d2,
                         const Date& ref_period_start=Date(),
                         const Date& ref_period_end=Date()) const {
        myQL_REQUIRE(_impl, "no day counter implementation provided!");
        return _impl->year_fraction(d1, d2, ref_period_start, ref_period_end);
    }
};

// operators on DayCounter
inline bool operator==(const DayCounter& dc1, const DayCounter& dc2) {
    return (dc1.empty() && dc2.empty())
    || (!dc1.empty() && !dc2.empty() && dc1.name() == dc2.name());
}

inline bool operator!=(const DayCounter& dc1, const DayCounter& dc2) {
    return !(dc1 == dc2);
}

inline std::ostream& operator<<(std::ostream& out, const DayCounter& dc){
    return out << dc.name();
}



}


#endif /* daycounter_hpp */
