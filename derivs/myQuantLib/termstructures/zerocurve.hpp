//
//  zerocurve.hpp
//  derivs
//
//  Created by Xin Li on 3/16/22.
//

#ifndef zerocurve_hpp
#define zerocurve_hpp

#include "zeroyieldstructure.hpp"
#include "interpolatedcurve.hpp"

namespace myQuantLib {

// YieldTermStructure based on interpolation of zero rates
// Interpolator: traits class; factory to produce interpolation instance

template <class Interpolator>
class InterpolatedZeroCurve : public ZeroYieldStructure,
                              protected InterpolatedCurve<Interpolator>
{
public:
    // constructor
    InterpolatedZeroCurve(const std::vector<Date>& dates,
                          const std::vector<double>& yields,
                          const DayCounter& dc,
                          const Calendar& calendar=Calendar(),
                          const std::vector<Handle<Quote>>& jumps = std::vector<Handle<Quote>>(),
                          const std::vector<Date>& jump_dates = std::vector<Date>(),
                          const Interpolator& interpolator = Interpolator(),
                          Compounding compounding = Continuous,
                          Frequency frequency = Annual);
    
    InterpolatedZeroCurve(const std::vector<Date>& dates,
                          const std::vector<double>& yields,
                          const DayCounter& dc,
                          const Calendar& calendar,
                          const Interpolator& interpolator,
                          Compounding compounding = Continuous,
                          Frequency frequency = Annual);
    InterpolatedZeroCurve(const std::vector<Date>& dates,
                          const std::vector<double>& yields,
                          const DayCounter& dc,
                          const Interpolator& interpolator,
                          Compounding compounding = Continuous,
                          Frequency frequency = Annual);
    
    // term structure interface
    Date max_date() const override;
    
    const std::vector<double>& times() const;
    const std::vector<Date>& dates() const;
    const std::vector<double>& data() const;
    const std::vector<double>& zero_rates() const;
    std::vector<std::pair<Date, double>> nodes() const;
    
protected:
    explicit InterpolatedZeroCurve(const DayCounter&,
                                   const Interpolator& interpolator=Interpolator());
    InterpolatedZeroCurve(const Date& ref_date,
                          const DayCounter&,
                          const std::vector<Handle<Quote>> & jumps = std::vector<Handle<Quote>>(),
                          const std::vector<Date>& jump_dates=std::vector<Date>(),
                          const Interpolator& interpolator = Interpolator());
    InterpolatedZeroCurve(int settlement_days,
                          const Calendar&,
                          const DayCounter&,
                          const std::vector<Handle<Quote>> & jumps = std::vector<Handle<Quote>>(),
                          const std::vector<Date>& jump_dates=std::vector<Date>(),
                          const Interpolator& interpolator = Interpolator());
    
    // zero yield structure implementation
    double zero_yield_impl(double time) const override;
    
    mutable std::vector<Date> _dates;
private:
    void initialize(const Compounding& compounding, const Frequency& frequency);
};

//TODO: implementation

};


#endif /* zerocurve_hpp */
