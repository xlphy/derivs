//
//  schedule.hpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#ifndef schedule_hpp
#define schedule_hpp

#include "calendar.hpp"
#include <boost/optional.hpp>
#include <vector>


namespace myQuantLib {

// Payment schedule
class Schedule {
public:
    /*! constructor that takes any list of dates, and optionally
        meta information that can be used by client classes. Note
        that neither the list of dates nor the meta information is
        checked for plausibility in any sense. i.e., assuming what passed in are making senses
     */
    // list of supplied dates + meta info
    Schedule(const std::vector<Date>&,
             Calendar calendar = NullCalendar(),
             BusinessDayConvention convention = Unadjusted,
             const boost::optional<BusinessDayConvention>& termination_date_convention = boost::none,
             const boost::optional<Period>& tenor = boost::none,
             const boost::optional<DateGeneration::Rule>& rule = boost::none,
             std::vector<bool> is_regular = std::vector<bool>(0)
             );
    // rule-based constructor
    Schedule(Date effective_date,
             const Date& termination_date,
             const Period& tenor,
             Calendar calendar,
             BusinessDayConvention convention,
             BusinessDayConvention termination_date_convention,
             DateGeneration::Rule rule,
             bool end_of_month,
             const Date& first_date = Date(),
             const Date& next_to_last_date = Date()
             );
    
    Schedule() = default;
    
    //Date access
    std::size_t size() const {return _dates.size();}
    const Date& operator[](std::size_t i) const {return _dates[i];}
    const Date& at(std::size_t i) const {return _dates.at(i);}
    const Date& date(std::size_t i) const {return _dates.at(i);}
    Date prev_date(const Date& ref_date) const;
    Date next_date(const Date& ref_date) const;
    const std::vector<Date>& dates() const {return _dates;}
    bool has_is_regular() const;
    bool is_regular(std::size_t i) const;
    const std::vector<bool>& is_regular() const;
    
    //Other inspectors
    bool empty() const {return _dates.empty();}
    const Calendar& calendar() const {return _calendar;}
    const Date& start_date() const {return _dates.front();}
    const Date& end_date() const {return _dates.back();}
    bool has_tenor() const {return _tenor != boost::none;}
    const Period& tenor() const;
    BusinessDayConvention bizday_convention() const;
    bool has_termination_date_bizday_convention() const;
    BusinessDayConvention termination_date_bizday_convention() const;
    bool has_rule() const;
    DateGeneration::Rule rule() const;
    bool has_end_of_month() const;
    bool end_of_month() const;
    
    //Iterators
    typedef std::vector<Date>::const_iterator const_iterator;
    const_iterator begin() const {return _dates.begin();}
    const_iterator end() const {return _dates.end();}
    const_iterator lower_bound(const Date& d= Date()) const;
    
    //Utilities
    //truncated schedule
    Schedule after(const Date& truncation_date) const;
    Schedule until(const Date& truncation_date) const;
    
private:
    boost::optional<Period> _tenor;
    Calendar _calendar;
    BusinessDayConvention _convention;
    boost::optional<BusinessDayConvention> _termination_date_convention;
    boost::optional<DateGeneration::Rule> _rule;
    boost::optional<bool> _end_of_month;
    Date _first_date, _next_to_last_date;
    std::vector<Date> _dates;
    std::vector<bool> _is_regular;
};




}


#endif /* schedule_hpp */
