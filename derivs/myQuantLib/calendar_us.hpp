//
//  calendar_us.hpp
//  derivs
//
//  Created by Xin Li on 2/24/22.
//

#ifndef calendar_us_hpp
#define calendar_us_hpp

//! United States calendars
/*! Public holidays (see https://www.opm.gov/policy-data-oversight/pay-leave/federal-holidays):
    <ul>
    <li>Saturdays</li>
    <li>Sundays</li>
    <li>New Year's Day, January 1st (possibly moved to Monday if
        actually on Sunday, or to Friday if on Saturday)</li>
    <li>Martin Luther King's birthday, third Monday in January (since
        1983)</li>
    <li>Presidents' Day (a.k.a. Washington's birthday),
        third Monday in February</li>
    <li>Memorial Day, last Monday in May</li>
    <li>Juneteenth, June 19th (moved to Monday if Sunday or
        Friday if Saturday)</li>
    <li>Independence Day, July 4th (moved to Monday if Sunday or
        Friday if Saturday)</li>
    <li>Labor Day, first Monday in September</li>
    <li>Columbus Day, second Monday in October</li>
    <li>Veterans' Day, November 11th (moved to Monday if Sunday or
        Friday if Saturday)</li>
    <li>Thanksgiving Day, fourth Thursday in November</li>
    <li>Christmas, December 25th (moved to Monday if Sunday or Friday
        if Saturday)</li>
    </ul>

    Note that since 2015 Independence Day only impacts Libor if it
    falls on a  weekday (see <https://www.theice.com/iba/libor>,
    <https://www.theice.com/marketdata/reports/170> and
    <https://www.theice.com/publicdocs/LIBOR_Holiday_Calendar_2015.pdf>
    for the fixing and value date calendars).

    Holidays for the stock exchange (data from http://www.nyse.com):
    <ul>
    <li>Saturdays</li>
    <li>Sundays</li>
    <li>New Year's Day, January 1st (possibly moved to Monday if
        actually on Sunday)</li>
    <li>Martin Luther King's birthday, third Monday in January (since
        1998)</li>
    <li>Presidents' Day (a.k.a. Washington's birthday),
        third Monday in February</li>
    <li>Good Friday</li>
    <li>Memorial Day, last Monday in May</li>
    <li>Independence Day, July 4th (moved to Monday if Sunday or
        Friday if Saturday)</li>
    <li>Labor Day, first Monday in September</li>
    <li>Thanksgiving Day, fourth Thursday in November</li>
    <li>Presidential election day, first Tuesday in November of election
        years (until 1980)</li>
    <li>Christmas, December 25th (moved to Monday if Sunday or Friday
        if Saturday)</li>
    <li>Special historic closings (see
        http://www.nyse.com/pdfs/closings.pdf)</li>
    </ul>

    Holidays for the government bond market (data from
    http://www.bondmarkets.com):
    <ul>
    <li>Saturdays</li>
    <li>Sundays</li>
    <li>New Year's Day, January 1st (possibly moved to Monday if
        actually on Sunday)</li>
    <li>Martin Luther King's birthday, third Monday in January (since
        1983)</li>
    <li>Presidents' Day (a.k.a. Washington's birthday),
        third Monday in February</li>
    <li>Good Friday</li>
    <li>Memorial Day, last Monday in May</li>
    <li>Independence Day, July 4th (moved to Monday if Sunday or
        Friday if Saturday)</li>
    <li>Labor Day, first Monday in September</li>
    <li>Columbus Day, second Monday in October</li>
    <li>Veterans' Day, November 11th (moved to Monday if Sunday or
        Friday if Saturday)</li>
    <li>Thanksgiving Day, fourth Thursday in November</li>
    <li>Christmas, December 25th (moved to Monday if Sunday or Friday
        if Saturday)</li>
    </ul>

    Holidays for the North American Energy Reliability Council
    (data from http://www.nerc.com/~oc/offpeaks.html):
    <ul>
    <li>Saturdays</li>
    <li>Sundays</li>
    <li>New Year's Day, January 1st (possibly moved to Monday if
        actually on Sunday)</li>
    <li>Memorial Day, last Monday in May</li>
    <li>Independence Day, July 4th (moved to Monday if Sunday)</li>
    <li>Labor Day, first Monday in September</li>
    <li>Thanksgiving Day, fourth Thursday in November</li>
    <li>Christmas, December 25th (moved to Monday if Sunday)</li>
    </ul>
*/

#include "calendar.hpp"

namespace myQuantLib {
// different markets have different implementations of calendar
class UnitedStates: public Calendar {
public:
  //! US calendars
  enum Market { Settlement,     //!< generic settlement calendar
                NYSE,           //!< New York stock exchange calendar
                GovernmentBond, //!< government-bond calendar
                NERC,           //!< off-peak days for NERC
                LiborImpact,    //!< Libor impact calendar
                FederalReserve  //!< Federal Reserve Bankwire System
  };

  explicit UnitedStates(Market market);

private:
    class SettlementImpl: public Calendar::WesternImpl {
    public:
        std::string name() const override {return "US settlement";}
        bool is_biz_day(const Date&) const override;
    };
    class LiborImpactImpl : public SettlementImpl {
      public:
        std::string name() const override { return "US with Libor impact"; }
        bool is_biz_day(const Date&) const override;
    };
    class NyseImpl : public Calendar::WesternImpl {
      public:
        std::string name() const override { return "New York stock exchange"; }
        bool is_biz_day(const Date&) const override;
    };
    class GovernmentBondImpl : public Calendar::WesternImpl {
      public:
        std::string name() const override { return "US government bond market"; }
        bool is_biz_day(const Date&) const override;
    };
    class NercImpl : public Calendar::WesternImpl {
      public:
        std::string name() const override {
            return "North American Energy Reliability Council";
        }
        bool is_biz_day(const Date&) const override;
    };
    class FederalReserveImpl : public Calendar::WesternImpl {
      public:
        std::string name() const override { return "Federal Reserve Bankwire System"; }
        bool is_biz_day(const Date&) const override;
    };
  
};

}





#endif /* calendar_us_hpp */
