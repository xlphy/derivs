//
//  calendar.hpp
//  derivs
//
//  Created by Xin Li on 2/23/22.
//

#ifndef calendar_hpp
#define calendar_hpp

#include "date.hpp"
#include <string>
#include <vector>
#include <set>
#include <memory>

namespace myQuantLib {

enum BusinessDayConvention {
    // ISDA
    Following,                   /*!< Choose the first business day after
                                      the given holiday. */
    ModifiedFollowing,           /*!< Choose the first business day after
                                      the given holiday unless it belongs
                                      to a different month, in which case
                                      choose the first business day before
                                      the holiday. */
    Preceding,                   /*!< Choose the first business
                                      day before the given holiday. */
    // NON ISDA
    ModifiedPreceding,           /*!< Choose the first business day before
                                      the given holiday unless it belongs
                                      to a different month, in which case
                                      choose the first business day after
                                      the holiday. */
    Unadjusted,                  /*!< Do not adjust. */
    HalfMonthModifiedFollowing,  /*!< Choose the first business day after
                                      the given holiday unless that day
                                      crosses the mid-month (15th) or the
                                      end of month, in which case choose
                                      the first business day before the
                                      holiday. */
    Nearest                      /*!< Choose the nearest business day
                                      to the given holiday. If both the
                                      preceding and following business
                                      days are equally far away, default
                                      to following business day. */
};

/*! \relates BusinessDayConvention */
std::ostream& operator<<(std::ostream&,
                         BusinessDayConvention);


class Period;

class Calendar {
protected:
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual std::string name() const = 0;
        virtual bool is_biz_day(const Date&) const = 0;
        virtual bool is_weekend(Weekday) const=0;
        std::set<Date> added_holidays, removed_holidays;
    };
    std::shared_ptr<Impl> _impl;
public:
    Calendar() = default;
    bool empty() const;
    std::string name() const;
    
    const std::set<Date>& added_holidays() const;
    const std::set<Date>& removed_holidays() const;
    
    void add_holiday(const Date&);
    void remove_holiday(const Date&);
    
    bool is_biz_day(const Date& d) const;
    bool is_holiday(const Date& d) const;
    bool is_weekend(Weekday w) const;
    bool is_end_of_month(const Date& d) const;
    Date end_of_month(const Date& d) const;
    
    std::vector<Date> holiday_list(const Date& from,
                                   const Date& to,
                                   bool include_weekends = false) const;
    
    std::vector<Date> bizday_list(const Date& from, const Date& to) const;
    
    Date adjust(const Date&,
                BusinessDayConvention convention = Following) const;
    
    Date advance(const Date&, int n, TimeUnit unit,
                 BusinessDayConvention convention = Following,
                 bool end_of_month = false) const;
    
    Date advance(const Date& date, const Period& period,
                 BusinessDayConvention convention = Following,
                 bool end_of_month = false) const;
    
    Date::serial_type bizdays_between(const Date& from, const Date& to,
                                      bool include_first = true,
                                      bool include_last = false) const;
    
protected:
    // partial calendar implementation
    class WesternImpl: public Impl {
    public:
        bool is_weekend(Weekday) const override;
        static Day easter_monday(Year);
    };
    
    class OrthodoxImpl: public Impl {
    public:
        bool is_weekend(Weekday) const override;
        static Day easter_monday(Year);
    };
};

bool operator==(const Calendar&, const Calendar&);
bool operator!=(const Calendar&, const Calendar&);
std::ostream& operator<<(std::ostream&, const Calendar&);

//TODO: inline definitions


}


#endif /* calendar_hpp */
