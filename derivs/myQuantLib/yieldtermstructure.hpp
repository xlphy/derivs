//
//  yieldtermstructure.hpp
//  derivs
//
//  Created by Xin Li on 3/13/22.
//

#ifndef yieldtermstructure_hpp
#define yieldtermstructure_hpp

#include "termstructure.hpp"
#include "quote.hpp"
#include "handle.hpp"
#include "interestrate.hpp"

#include <vector>

namespace myQuantLib {

// Interest-rate term structure

class YieldTermStructure: public TermStructure {
public:
    explicit YieldTermStructure(const DayCounter& dc = DayCounter()) : TermStructure(dc) {}
    YieldTermStructure(const Date& ref_date,
                       const Calendar& cal = Calendar(),
                       const DayCounter& dc = DayCounter(),
                       std::vector<Handle<Quote>> jumps = std::vector<Handle<Quote>>(),
                       const std::vector<Date>& jump_dates = std::vector<Date>()
                       )
    : TermStructure(ref_date, cal, dc), _jumps(std::move(jumps)), _jump_dates(jump_dates),
    _jump_times(jump_dates.size()), n_jumps(_jumps.size()){
        set_jumps(YieldTermStructure::ref_date());
        for(std::size_t i = 0; i < n_jumps; ++i)
            register_with(_jumps[i]);  // trigger update whenever any one of the jumps changes
    }
    
    YieldTermStructure(int settlement_days,
                       const Calendar& cal,
                       const DayCounter& dc = DayCounter(),
                       std::vector<Handle<Quote>> jumps = std::vector<Handle<Quote>>(),
                       const std::vector<Date>& jump_dates = std::vector<Date>()
                       )
    : TermStructure(settlement_days, cal, dc), _jumps(std::move(jumps)), _jump_dates(jump_dates),
    _jump_times(jump_dates.size()), n_jumps(_jumps.size()){
        set_jumps(YieldTermStructure::ref_date());
        for(std::size_t i = 0; i < n_jumps; ++i)
            register_with(_jumps[i]);  // trigger update whenever any one of the jumps changes
    }
    
    /*! \name Discount factors

        These methods return the discount factor from a given date or time
        to the reference date.  In the latter case, the time is calculated
        as a fraction of year from the reference date.
    */
    double discount(const Date& d, bool extrapolate = false) const {
        return discount(time_from_ref(d), extrapolate);
    }
    double discount(double time, bool extrapolate = false) const;
    /*! \name Zero-yield rates

        These methods return the implied zero-yield rate for a
        given date or time.  In the former case, the time is
        calculated as a fraction of year from the reference date.
    */
    InterestRate zero_rate(const Date& d,
                           const DayCounter& resultDC,
                           Compounding comp,
                           Frequency freq = Annual,
                           bool extrapolate = false) const;
    InterestRate zero_rate(double time,
                           Compounding comp,
                           Frequency freq = Annual,
                           bool extrapolate = false) const;
    /*! \name Forward rates

        These methods returns the forward interest rate between two dates
        or times.  In the former case, times are calculated as fractions
        of year from the reference date.

        If both dates (times) are equal the instantaneous forward rate is
        returned.
    */
    InterestRate forward_rate(const Date& d1,
                              const Date& d2,
                              const DayCounter& resultDC,
                              Compounding comp,
                              Frequency freq = Annual,
                              bool extrapolate = false) const;
    InterestRate forward_rate(const Date& d,
                              const Period& p,
                              const DayCounter& resultDC,
                              Compounding comp,
                              Frequency freq = Annual,
                              bool extrapolate = false) const {
        return forward_rate(d, d + p, resultDC, comp, freq, extrapolate);
    }
    
    InterestRate forward_rate(double t1,
                              double t2,
                              Compounding comp,
                              Frequency freq = Annual,
                              bool extrapolate = false) const;
    
    const std::vector<Date>& jump_dates() const {return this->_jump_dates;}
    const std::vector<double>& jump_times() const {return this->_jump_times;}
    
    
    // Observer interface
    void update() override;
    
protected:
    /*! \name Calculations

        This method must be implemented in derived classes to
        perform the actual calculations. When it is called,
        range check has already been performed; therefore, it
        must assume that extrapolation is required.
    */
    virtual double discount_impl(double) const = 0;
private:
    // method
    void set_jumps(const Date& ref_date);
    // data members
    std::vector<Handle<Quote>> _jumps;
    std::vector<Date> _jump_dates;
    std::vector<double> _jump_times;
    std::size_t n_jumps = 0;
    Date _latest_ref;
};


}


#endif /* yieldtermstructure_hpp */
