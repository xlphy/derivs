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
#include "../interpolations/linearinterpolation.hpp"


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
                          Frequency frequency = Annual)
    : ZeroYieldStructure(dates.at(0), calendar, dc, jumps, jump_dates), InterpolatedCurve<Interpolator>(std::vector<double>(), yields, interpolator), _dates(dates)
    {
        initialize(compounding, frequency);
    }
    
    InterpolatedZeroCurve(const std::vector<Date>& dates,
                          const std::vector<double>& yields,
                          const DayCounter& dc,
                          const Calendar& calendar,
                          const Interpolator& interpolator,
                          Compounding compounding = Continuous,
                          Frequency frequency = Annual)
    : ZeroYieldStructure(dates.at(0), calendar, dc), InterpolatedCurve<Interpolator>(std::vector<double>(), yields, interpolator), _dates(dates)
    {
        initialize(compounding, frequency);
    }
    
    InterpolatedZeroCurve(const std::vector<Date>& dates,
                          const std::vector<double>& yields,
                          const DayCounter& dc,
                          const Interpolator& interpolator,
                          Compounding compounding = Continuous,
                          Frequency frequency = Annual)
    : ZeroYieldStructure(dates.at(0), Calendar(), dc), InterpolatedCurve<Interpolator>(std::vector<double>(), yields, interpolator), _dates(dates)
    {
        initialize(compounding, frequency);
    }
    
    // term structure interface
    Date max_date() const override {
        if(this->_max_date != Date())
            return this->_max_date;
        return _dates.back();
    }
    
    const std::vector<double>& times() const {return this->_times;}
    const std::vector<Date>& dates() const {return _dates;}
    const std::vector<double>& data() const {return this->_data;}
    const std::vector<double>& zero_rates() const {return this->_data;}
    std::vector<std::pair<Date, double>> nodes() const {
        std::vector<std::pair<Date, double>> results(_dates.size());
        for(std::size_t i=0; i<_dates.size(); ++i)
            results[i] = std::make_pair(_dates[i], this->_data[i]);
        return results;
    }
    
    
protected:
    explicit InterpolatedZeroCurve(const DayCounter& dc,
                                   const Interpolator& interpolator=Interpolator())
    :ZeroYieldStructure(dc), InterpolatedCurve<Interpolator>(interpolator){}
    
    InterpolatedZeroCurve(const Date& ref_date,
                          const DayCounter& dc,
                          const std::vector<Handle<Quote>> & jumps = std::vector<Handle<Quote>>(),
                          const std::vector<Date>& jump_dates=std::vector<Date>(),
                          const Interpolator& interpolator = Interpolator())
    : ZeroYieldStructure(ref_date, Calendar(), dc, jumps, jump_dates), InterpolatedCurve<Interpolator>(interpolator) {}
    
    InterpolatedZeroCurve(int settlement_days,
                          const Calendar& calendar,
                          const DayCounter& dc,
                          const std::vector<Handle<Quote>> & jumps = std::vector<Handle<Quote>>(),
                          const std::vector<Date>& jump_dates=std::vector<Date>(),
                          const Interpolator& interpolator = Interpolator())
    : ZeroYieldStructure(settlement_days, calendar, dc, jumps, jump_dates), InterpolatedCurve<Interpolator>(interpolator) {}
    
    // zero yield structure implementation, actual calculation
    double zero_yield_impl(double time) const override {
        if(time <= this->_times.back())
            return this->_interpolation(time, true);
        // flat fwd extrapolation
        double tmax = this->_times.back();
        double zmax = this->_data.back();
        double inst_fwd_max = zmax + tmax * this->_interpolation.derivative(tmax);
        return (zmax * tmax + inst_fwd_max * (time-tmax)) / time;
    }
    
    mutable std::vector<Date> _dates;
private:
    void initialize(const Compounding& compounding, const Frequency& frequency) {
        myQL_REQUIRE(_dates.size() >= Interpolator::required_points,
                     "not enough input dates given");
        myQL_REQUIRE(this->_data.size() == _dates.size(),
                     "dates/data count mismatch");
        this->_times.resize(_dates.size());
        this->_times[0] = 0.0;
        if(compounding != Continuous) {
            // We also have to convert the first rate.
            // The first time is 0.0, so we can't use it.
            // We fall back to about one day.
            double dt = 1.0 / 365;
            InterestRate r(this->_data[0], day_counter(), compounding, frequency);
            this->_data[0] = r.equivalent_rate(Continuous, NoFrequency, dt);
        }
        
        for(std::size_t i=1; i<_dates.size(); ++i) {
            myQL_REQUIRE(_dates[i] > _dates[i-1],
                         "invalid date (" << _dates[i] << ", vs" << _dates[i-1] << ")");
            this->_times[i] = day_counter().year_fraction(_dates[0], _dates[i]);
            myQL_REQUIRE(this->_times[i] - this->_times[i-1] > 0.000001,
                         "two dates correspond to the same time "
                         "under this curve's day count convention");
            
            // adjusting zero rates to match continuous compounding
            if (compounding != Continuous)
            {
                InterestRate r(this->_data[i], day_counter(), compounding, frequency);
                this->_data[i] = r.equivalent_rate(Continuous, NoFrequency, this->_times[i]);
            }
        }
        
        this->_interpolation = this->_interpolator.interpolate(this->_times.begin(),
                                                               this->_times.end(),
                                                               this->_data.begin());
        this->_interpolation.update();
    }
};

// Term structure based on linear interpolation of zero yields
typedef InterpolatedZeroCurve<Linear> ZeroCurve;


};


#endif /* zerocurve_hpp */
