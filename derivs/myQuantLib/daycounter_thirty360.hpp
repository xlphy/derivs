//
//  daycounter_thirty360.hpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#ifndef daycounter_thirty360_hpp
#define daycounter_thirty360_hpp

#include "daycounter.hpp"

namespace myQuantLib {

//! 30/360 day count convention
/*! The 30/360 day count can be calculated according to a
    number of conventions.

    US convention: if the starting date is the 31st of a month or
    the last day of February, it becomes equal to the 30th of the
    same month.  If the ending date is the 31st of a month and the
    starting date is the 30th or 31th of a month, the ending date
    becomes equal to the 30th.  If the ending date is the last of
    February and the starting date is also the last of February,
    the ending date becomes equal to the 30th.
    Also known as "30/360" or "360/360".

    Bond Basis convention: if the starting date is the 31st of a
    month, it becomes equal to the 30th of the same month.
    If the ending date is the 31st of a month and the starting
    date is the 30th or 31th of a month, the ending date
    also becomes equal to the 30th of the month.
    Also known as "US (ISMA)".

    European convention: starting dates or ending dates that
    occur on the 31st of a month become equal to the 30th of the
    same month.
    Also known as "30E/360", or "Eurobond Basis".

    Italian convention: starting dates or ending dates that
    occur on February and are greater than 27 become equal to 30
    for computational sake.

    ISDA convention: starting or ending dates on the 31st of the
    month become equal to 30; starting dates or ending dates that
    occur on the last day of February also become equal to 30,
    except for the termination date.  Also known as "30E/360
    ISDA", "30/360 ISDA", or "30/360 German".

    NASD convention: if the starting date is the 31st of a
    month, it becomes equal to the 30th of the same month.
    If the ending date is the 31st of a month and the starting
    date is earlier than the 30th of a month, the ending date
    becomes equal to the 1st of the next month, otherwise the
    ending date becomes equal to the 30th of the same month.
*/

class Thirty360: public DayCounter {
public:
    enum Convention {
        USA,
        BondBasis,
        European,
        EurobondBasis,
        Italian,
        German,
        ISMA,
        ISDA,
        NASD
    };
    
    // construct Thirty360 with a specific convention
    explicit Thirty360(Convention c, const Date& terminationDate = Date())
    : DayCounter(implementation(c, terminationDate, false)) {}

private:
    class Thirty360_Impl: public DayCounter::Impl {
    public:
        double year_fraction(const Date& d1, const Date& d2,
                             const Date&, const Date&) const override {
            return day_count(d1, d2) / 360.0;
        }
    };
    // concrete implementation
    class US_Impl : public Thirty360_Impl {
      public:
        std::string name() const override { return std::string("30/360 (US)"); }
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
    };
    class ISMA_Impl : public Thirty360_Impl {
      public:
        std::string name() const override { return std::string("30/360 (Bond Basis)"); }
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
    };
    class EU_Impl : public Thirty360_Impl {
      public:
        std::string name() const override { return std::string("30E/360 (Eurobond Basis)"); }
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
    };
    class IT_Impl : public Thirty360_Impl {
      public:
        std::string name() const override { return std::string("30/360 (Italian)"); }
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
    };
    
    class ISDA_Impl : public Thirty360_Impl {
      public:
        explicit ISDA_Impl(const Date& termination_date, bool is_last_period)
        : _termination_date(termination_date), _is_last_period(is_last_period) {}
        std::string name() const override { return std::string("30E/360 (ISDA)"); }
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
      private:
        Date _termination_date;
        bool _is_last_period;
    };
    class NASD_Impl : public Thirty360_Impl {
      public:
        std::string name() const override { return std::string("30/360 (NASD)"); }
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
    };
    // method
    static std::shared_ptr<DayCounter::Impl>
    implementation(Convention c, const Date& termination_date, bool is_last_period);
};



}

#endif /* daycounter_thirty360_hpp */
