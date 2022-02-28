//
//  daycounter_biz252.hpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#ifndef daycounter_biz252_hpp
#define daycounter_biz252_hpp

#include "calendar.hpp"
#include "calendar_us.hpp"
#include "daycounter.hpp"

namespace myQuantLib {

class Biz252: public DayCounter {
private:
    class Impl: public DayCounter::Impl {
    private:
        Calendar _calendar;
    public:
        std::string name() const override;
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
        double year_fraction(const Date& d1, const Date& d2,
                             const Date&, const Date&) const override;
        explicit Impl(Calendar c=UnitedStates(UnitedStates::NYSE)): _calendar(std::move(c)){}
    };
public:
    Biz252(const Calendar& c)
    : DayCounter(std::shared_ptr<DayCounter::Impl>(new Biz252::Impl(c))){}
};

}



#endif /* daycounter_biz252_hpp */
