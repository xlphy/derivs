//
//  interestrate.hpp
//  derivs
//
//  Created by Xin Li on 3/13/22.
//

#ifndef interestrate_hpp
#define interestrate_hpp

#include "daycounter.hpp"


namespace myQuantLib {
//! Concrete interest rate class
/*! This class encapsulate the interest rate compounding algebra.
    It manages day-counting conventions, compounding conventions,
    conversion between different conventions, discount/compound factor
    calculations, and implied/equivalent rate calculations.

    \test Converted rates are checked against known good results
*/

//! Interest rate coumpounding rule
enum Compounding { Simple = 0,          //!< \f$ 1+rt \f$
                   Compounded = 1,      //!< \f$ (1+r)^t \f$
                   Continuous = 2,      //!< \f$ e^{rt} \f$
                   SimpleThenCompounded, //!< Simple up to the first period then Compounded
                   CompoundedThenSimple //!< Compounded up to the first period then Simple
};


class InterestRate {
public:
    InterestRate(): _r{0}{}
    InterestRate(double r, DayCounter dc, Compounding comp, Frequency freq)
    : _r(r), _dc(std::move(dc)), _comp(comp), _freq_makes_sense(false){
        if(_comp == Compounded || _comp == SimpleThenCompounded || _comp == CompoundedThenSimple){
            _freq_makes_sense = true;
            myQL_REQUIRE(freq != Once && freq != NoFrequency,
                         "frequency not allowed for this interest rate");
            _freq = double(_freq);
        }
    }
    
    operator double() const {return _r;} // convert to double
    
    double rate() const {return _r;}
    const DayCounter& day_counter() const {return _dc;}
    Compounding compounding() const {return _comp;}
    Frequency frequency() const {
        return _freq_makes_sense ? Frequency(int(_freq)): NoFrequency;
    }
    
    //! discount factor implied by the rate compounded at time t.
    /*! \warning Time must be measured using InterestRate's own
                 day counter.
    */
    double compound_factor(double time) const;
    double compound_factor(const Date& d1,
                           const Date& d2,
                           const Date& ref_start=Date(),
                           const Date& ref_end = Date()) const {
        myQL_REQUIRE(d2 >= d1,
                     "d1 (" << d1 << ") "
                     "later than d2 (" << d2 << ")");
        double t = _dc.year_fraction(d1, d2, ref_start, ref_end);
        return compound_factor(t);
    }
    
    double discount_factor(double time) const { return 1.0 / compound_factor(time);}
    double discount_factor(const Date& d1,
                           const Date& d2,
                           const Date& ref_start=Date(),
                           const Date& ref_end=Date()) const {
        myQL_REQUIRE(d2 >= d1,
                     "d1 (" << d1 << ") "
                     "later than d2 (" << d2 << ")");
        double t = _dc.year_fraction(d1, d2, ref_start, ref_end);
        return discount_factor(t);
    }
    
    
    //! implied interest rate for a given compound factor at a given time.
    /*! The resulting InterestRate has the day-counter provided as input.

        \warning Time must be measured using the day-counter provided
                 as input.
    */
    static InterestRate implied_rate(double compound,
                                     const DayCounter& resultDC,
                                     Compounding comp,
                                     Frequency freq,
                                     double time);

    //! implied rate for a given compound factor between two dates.
    /*! The resulting rate is calculated taking the required
        day-counting rule into account.
    */
    static InterestRate implied_rate(double compound,
                                     const DayCounter& resultDC,
                                     Compounding comp,
                                     Frequency freq,
                                     const Date& d1,
                                     const Date& d2,
                                     const Date& ref_start = Date(),
                                     const Date& ref_end = Date()) {
        myQL_REQUIRE(d2>=d1,
                     "d1 (" << d1 << ") "
                     "later than d2 (" << d2 << ")");
        double t = resultDC.year_fraction(d1, d2, ref_start, ref_end);
        return implied_rate(compound, resultDC, comp, freq, t);
    }
    
    //! equivalent interest rate for a compounding period t.
    /*! The resulting InterestRate shares the same implicit
        day-counting rule of the original InterestRate instance.

        \warning Time must be measured using the InterestRate's
                 own day counter.
    */
    InterestRate equivalent_rate(Compounding comp,
                                 Frequency freq,
                                 double time) const {
        return implied_rate(compound_factor(time), _dc, comp, freq, time);
    }
    
    //! equivalent rate for a compounding period between two dates
    /*! The resulting rate is calculated taking the required
        day-counting rule into account.
    */
    InterestRate equivalent_rate(const DayCounter& resultDC,
                                 Compounding comp,
                                 Frequency freq,
                                 Date d1,
                                 Date d2,
                                 const Date& ref_start = Date(),
                                 const Date& ref_end = Date()) const {
        myQL_REQUIRE(d2>=d1,
                     "d1 (" << d1 << ") "
                     "later than d2 (" << d2 << ")");
        double t1 = _dc.year_fraction(d1, d2, ref_start, ref_end);
        double t2 = resultDC.year_fraction(d1, d2, ref_start, ref_end);
        return implied_rate(compound_factor(t1), resultDC, comp, freq, t2);
    }
    
    
private:
    double _r;
    DayCounter _dc;
    Compounding _comp;
    bool _freq_makes_sense;
    double _freq;
};

std::ostream& operator<<(std::ostream&, const InterestRate&);

}


#endif /* interestrate_hpp */
