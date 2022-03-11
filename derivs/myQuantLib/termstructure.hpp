//
//  termstructure.hpp
//  derivs
//
//  Created by Xin Li on 3/7/22.
//

#ifndef termstructure_hpp
#define termstructure_hpp

#include "observer.hpp"
#include "interpolations/extrapolation.hpp"
#include "daycounter.hpp"
#include "calendar.hpp"


namespace myQuantLib {

// Basic term-structure functionality
// 1. keep track of its own reference date
// 2. convert dates to real-valued times, starting t = 0 at the reference date
// 3. check whether a given date or time belongs to the domain covered by the term structure,
// require max_date(), min_date = reference date


class TermStructure: public virtual Observer, public virtual Observable, public Extrapolator {
public:
    // constructors
    /*
     There are three ways in which a term structure can keep
     track of its reference date.  The first is that such date
     is fixed; the second is that it is determined by advancing
     the current date of a given number of business days; and
     the third is that it is based on the reference date of
     some other structure.

     In the first case, the constructor taking a date is to be
     used; the default implementation of referenceDate() will
     then return such date.
     
     In the second case, the constructor taking a number of days and a calendar is to be used;
     referenceDate() will return a date calculated based on the
     current evaluation date, and the term structure and its
     observers will be notified when the evaluation date
     changes.
     
     In the last case, the referenceDate() method must
     be overridden in derived classes so that it fetches and
     return the appropriate date.
     */
    
    // default
    explicit TermStructure(DayCounter dc = DayCounter());
    // initialize with a fixed reference date
    explicit TermStructure(const Date& ref_date,
                           Calendar calendar = Calendar(),
                           DayCounter dc = DayCounter());
    // calculate the reference date based on the global evaluation date
    TermStructure(int settlement_days, Calendar, DayCounter dc=DayCounter());
    
    ~TermStructure() override = default;
    
    virtual DayCounter day_counter() const {return _day_counter;}
    double time_from_ref(const Date& date) const {
        return day_counter().year_fraction(ref_date(), date);
    }
    
    virtual Date max_date() const = 0;
    virtual double max_time() const {return time_from_ref(max_date());}
    
    virtual const Date& ref_date() const;
    virtual Calendar calendar() const {return _calendar;}
    virtual int settlement_days() const {return _settlement_days;}
    
    
    // observer interface
    void update() override;
    
protected:
    void check_range(const Date& d, bool extrapolate) const;
    void check_range(double time, bool extrapolate) const;
    bool _moving = false;
    mutable bool _updated = true;
    Calendar _calendar;
    
private:
    mutable Date _ref_date;
    int _settlement_days;
    DayCounter _day_counter;
};


}



#endif /* termstructure_hpp */
