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
             const boost::optional<bool>& end_of_month = boost::none,
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
    bool has_is_regular() const {return !_is_regular.empty();}
    bool is_regular(std::size_t i) const {
        myQL_REQUIRE(has_is_regular(), "full interface (is_regular) not available");
        myQL_REQUIRE(i<=_is_regular.size() && i>0,
                     "index (" << i << ") must be in [1, " <<
                     _is_regular.size() << "]");
        return _is_regular[i-1];
    }
    const std::vector<bool>& is_regular() const {
        myQL_REQUIRE(!_is_regular.empty(), "full interface (is_regular) not available");
        return _is_regular;
    }
    
    //Other inspectors
    bool empty() const {return _dates.empty();}
    const Calendar& calendar() const {return _calendar;}
    const Date& start_date() const {return _dates.front();}
    const Date& end_date() const {return _dates.back();}
    bool has_tenor() const {return _tenor != boost::none;}
    const Period& tenor() const {
        myQL_REQUIRE(has_tenor(), "full interface (tenor) not available");
        return *_tenor;
    }
    BusinessDayConvention bizday_convention() const {return _convention;}
    bool has_termination_date_bizday_convention() const {
        return _termination_date_convention != boost::none;
    }
    BusinessDayConvention termination_date_bizday_convention() const {
        myQL_REQUIRE(has_termination_date_bizday_convention(), "full interface (termination date bdc) not available");
        return *_termination_date_convention;
    }
    bool has_rule() const {return _rule != boost::none;}
    DateGeneration::Rule rule() const {
        myQL_REQUIRE(has_rule(), "full interface (rule) not available");
        return *_rule;
    }
    bool has_end_of_month() const {return _end_of_month != boost::none;}
    bool end_of_month() const {
        myQL_REQUIRE(has_end_of_month(), "full interface (end of month) not available");
        return *_end_of_month;
    }
    
    //Iterators
    typedef std::vector<Date>::const_iterator const_iterator;
    const_iterator begin() const {return _dates.begin();}
    const_iterator end() const {return _dates.end();}
    const_iterator lower_bound(const Date& d= Date()) const {
        return std::lower_bound(_dates.begin(), _dates.end(), d);
    };
    
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

//! helper class
/*! This class provides a more comfortable interface to the
    argument list of Schedule's constructor.
*/
// e.g., one can write:
// Schedule s = MakeSchedule().from(start_date).to(end_date)
//              .withFrequency(Semiannual).withCalendar(TARGET())
//              .withNextToLastDate(stub_date)
//              .backwards()

class MakeSchedule {
public:
    MakeSchedule& from(const Date& effective_date){
        _effective_date = effective_date;
        return *this;
    }
    MakeSchedule& to(const Date& termination_date){
        _termination_date = termination_date;
        return *this;
    }
    MakeSchedule& with_tenor(const Period& tenor){
        _tenor = tenor;
        return *this;
    }
    MakeSchedule& with_frequency(Frequency frequency){
        _tenor = Period(frequency);
        return *this;
    }
    MakeSchedule& with_calendar(const Calendar& calendar){
        _calendar = calendar;
        return *this;
    }
    MakeSchedule& with_convention(BusinessDayConvention conv){
        _convention = conv;
        return *this;
    }
    MakeSchedule& with_termination_date_convention(BusinessDayConvention conv){
        _termination_date_convention = conv;
        return *this;
    }
    MakeSchedule& with_rule(DateGeneration::Rule r){
        _rule = r;
        return *this;
    }
    MakeSchedule& forwards(){
        _rule = DateGeneration::Forward;
        return *this;
    }
    MakeSchedule& backwards(){
        _rule = DateGeneration::Backward;
        return *this;
    }
    MakeSchedule& end_of_month(bool flag=true){
        _end_of_month = flag;
        return *this;
    }
    MakeSchedule& with_first_date(const Date& d){
        _first_date = d;
        return *this;
    }
    MakeSchedule& with_next_to_last_date(const Date& d){
        _next_to_last_date = d;
        return *this;
    }
    operator Schedule() const; // type-conversion to Schedule
private:
    // store meta data info for schedule, use info to make a concrete schedule
    Calendar _calendar;
    Date _effective_date, _termination_date;
    boost::optional<Period> _tenor;
    boost::optional<BusinessDayConvention> _convention;
    boost::optional<BusinessDayConvention> _termination_date_convention;
    DateGeneration::Rule _rule = DateGeneration::Backward;
    bool _end_of_month = false;
    Date _first_date, _next_to_last_date;
};


}


#endif /* schedule_hpp */
