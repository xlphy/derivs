//
//  schedule.cpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#include "schedule.hpp"

namespace myQuantLib {

namespace {

Date next_twentieth(const Date& d, DateGeneration::Rule rule){
    Date result = Date(20, d.month(), d.year());
    if(result < d) result += 1*Months;
    if(rule == DateGeneration::TwentiethIMM ||
       rule == DateGeneration::OldCDS ||
       rule == DateGeneration::CDS ||
       rule == DateGeneration::CDS2015) {
        Month m = result.month();
        if(m % 3 != 0){
            // not a main IMM month
            int skip = 3 - m%3;
            result += skip * Months;
        }
    }
    return result;
}

bool allows_end_of_month(const Period& tenor) {
    return (tenor.units() == Months || tenor.units() == Years) && tenor >= 1*Months;
}
}

Schedule::Schedule(const std::vector<Date>& dates,
                   Calendar calendar,
                   BusinessDayConvention convention,
                   const boost::optional<BusinessDayConvention>& termination_date_convention,
                   const boost::optional<Period>& tenor,
                   const boost::optional<DateGeneration::Rule>& rule,
                   const boost::optional<bool>& end_of_month,
                   std::vector<bool> is_regular)
: _tenor(tenor), _calendar(std::move(calendar)), _convention(convention),
_termination_date_convention(termination_date_convention), _rule(rule), _dates(dates),
_is_regular(std::move(is_regular)){
    if (tenor != boost::none && !allows_end_of_month(*tenor))
        _end_of_month = false;
    else
        _end_of_month = end_of_month;
    
    myQL_REQUIRE(_is_regular.empty() || _is_regular.size() == dates.size() - 1,
                 "is_regular size (" << _is_regular.size()
                 << ") must be zero or equal to the number of dates - 1 ("
                 << dates.size() - 1 << ")");
}

// major work is here, to create a schedule by a rule, details with edge cases
Schedule::Schedule(Date effective_date,
                   const Date& termination_date,
                   const Period& tenor,
                   Calendar cal,
                   BusinessDayConvention convention,
                   BusinessDayConvention termination_date_convention,
                   DateGeneration::Rule rule,
                   bool end_of_month,
                   const Date& first,
                   const Date& next_to_last)
: _tenor(tenor), _calendar(std::move(cal)), _convention(convention),
_termination_date_convention(termination_date_convention), _rule(rule),
_end_of_month(allows_end_of_month(tenor) ? end_of_month: false),
_first_date(first == effective_date ? Date(): first),
_next_to_last_date(next_to_last == termination_date ? Date(): next_to_last){
    // sanity checks
    myQL_REQUIRE(termination_date != Date(), "null termination date");
    
    // in many cases (e.g. non-expired bonds) the effective date is not really necessary.
    // In these cases a decent placeholder is enough
    if(effective_date == Date() && first == Date() && rule == DateGeneration::Backward){
        // null effective_date, requires to define a sensible placeholder, skip it for now
//        Date evalDate = Settings::instance().evaluationDate();
//        myQL_REQUIRE(evalDate < termination_date, "null effective date");
//        Natural y;
//        if (nextToLast != Date()) {
//            y = (nextToLast - evalDate)/366 + 1;
//            effective_date = nextToLast - y*Years;
//        } else {
//            y = (termination_date - evalDate)/366 + 1;
//            effective_date = termination_date - y*Years;
//        }
        
    } else
        myQL_REQUIRE(effective_date != Date(), "null effective date");
    
    myQL_REQUIRE(effective_date < termination_date,
                 "effective date (" << effective_date
                 << ") later than or equal to termination date ("
                 << termination_date << ")");
    
    if(tenor.length()==0) _rule = DateGeneration::Zero;
    else
        myQL_REQUIRE(tenor.length()>0,
                     "non positive tenor (" << tenor << ") not allowed");
    // first_date not null
    if(_first_date != Date()){
        switch(*_rule){
            case DateGeneration::Backward:
            case DateGeneration::Forward:
                myQL_REQUIRE(_first_date > effective_date &&
                             _first_date <= termination_date,
                             "first date (" << _first_date <<
                             ") out of effective-termination date range (" <<
                             effective_date << ", " << termination_date << "]");
                // we should ensure that the above condition is still verified after adjustment
                break;
            case DateGeneration::ThirdWednesday:
                //myQL_REQUIRE(IMM::isIMMdate(_first_date, false),
                //           "first date (" << _first_date <<
                //           ") is not an IMM date");
                break;
            case DateGeneration::Zero:
            case DateGeneration::Twentieth:
            case DateGeneration::TwentiethIMM:
            case DateGeneration::OldCDS:
            case DateGeneration::CDS:
            case DateGeneration::CDS2015:
                myQL_FAIL("first date incompatible with " << *_rule << " date generation rule");
            default:
                myQL_FAIL("unknown rule (" << int(*_rule) << ")");
        }
    }
    // next_to_last_date not null
    if (_next_to_last_date != Date()) {
        switch (*_rule) {
          case DateGeneration::Backward:
          case DateGeneration::Forward:
            myQL_REQUIRE(_next_to_last_date >= effective_date &&
                       _next_to_last_date < termination_date,
                       "next to last date (" << _next_to_last_date <<
                       ") out of effective-termination date range [" <<
                       effective_date << ", " << termination_date << ")");
            // we should ensure that the above condition is still
            // verified after adjustment
            break;
          case DateGeneration::ThirdWednesday:
//            myQL_REQUIRE(IMM::isIMMdate(_next_to_last_date, false),
//                       "next-to-last date (" << _next_to_last_date <<
//                       ") is not an IMM date");
            break;
          case DateGeneration::Zero:
          case DateGeneration::Twentieth:
          case DateGeneration::TwentiethIMM:
          case DateGeneration::OldCDS:
          case DateGeneration::CDS:
          case DateGeneration::CDS2015:
            myQL_FAIL("next to last date incompatible with " << *_rule <<" date generation rule");
          default:
            myQL_FAIL("unknown rule (" << int(*_rule) << ")");
        }
    }
    
    // calendar needed for end_of_month adjustment
    Calendar null_calendar = NullCalendar();
    int periods = 1;
    Date seed, exit_date;
    switch (*_rule) {
      case DateGeneration::Zero:
        _tenor = 0*Years;
        _dates.push_back(effective_date);
        _dates.push_back(termination_date);
        _is_regular.push_back(true);
        break;

      case DateGeneration::Backward:
        _dates.push_back(termination_date);
        seed = termination_date;
        if (_next_to_last_date != Date()) {
            _dates.insert(_dates.begin(), _next_to_last_date);
            Date temp = null_calendar.advance(seed,
                -periods*(*_tenor), convention, *_end_of_month);
            if (temp!=_next_to_last_date)
                _is_regular.insert(_is_regular.begin(), false);
            else
                _is_regular.insert(_is_regular.begin(), true);
            seed = _next_to_last_date;
        }

        exit_date = effective_date;
        if (_first_date != Date())
            exit_date = _first_date;

        for (;;) {
            Date temp = null_calendar.advance(seed,
                -periods*(*_tenor), convention, *_end_of_month);
            if (temp < exit_date) {
                if (_first_date != Date() &&
                    (_calendar.adjust(_dates.front(),convention)!=
                     _calendar.adjust(_first_date,convention))) {
                    _dates.insert(_dates.begin(), _first_date);
                    _is_regular.insert(_is_regular.begin(), false);
                }
                break;
            } else {
                // skip dates that would result in duplicates after adjustment
                if (_calendar.adjust(_dates.front(),convention)!=
                    _calendar.adjust(temp,convention)) {
                    _dates.insert(_dates.begin(), temp);
                    _is_regular.insert(_is_regular.begin(), true);
                }
                ++periods;
            }
        }

        if (_calendar.adjust(_dates.front(),convention)!=
            _calendar.adjust(effective_date,convention)) {
            _dates.insert(_dates.begin(), effective_date);
            _is_regular.insert(_is_regular.begin(), false);
        }
        break;

      case DateGeneration::Twentieth:
      case DateGeneration::TwentiethIMM:
      case DateGeneration::ThirdWednesday:
      case DateGeneration::ThirdWednesdayInclusive:
      case DateGeneration::OldCDS:
      case DateGeneration::CDS:
      case DateGeneration::CDS2015:
        myQL_REQUIRE(!*_end_of_month,
                   "end_of_month convention incompatible with " << *_rule <<
                   " date generation rule");
      // fall through
      case DateGeneration::Forward:

        if (*_rule == DateGeneration::CDS || *_rule == DateGeneration::CDS2015) {
//            Date prev20th = previousTwentieth(effective_date, *_rule);
//            if (_calendar.adjust(prev20th, convention) > effective_date) {
//                _dates.push_back(prev20th - 3 * Months);
//                _is_regular.push_back(true);
//            }
//            _dates.push_back(prev20th);
        } else {
            _dates.push_back(effective_date);
        }

        seed = _dates.back();

        if (_first_date!=Date()) {
            _dates.push_back(_first_date);
            Date temp = null_calendar.advance(seed, periods*(*_tenor),
                                             convention, *_end_of_month);
            if (temp!=_first_date)
                _is_regular.push_back(false);
            else
                _is_regular.push_back(true);
            seed = _first_date;
        } else if (*_rule == DateGeneration::Twentieth ||
                   *_rule == DateGeneration::TwentiethIMM ||
                   *_rule == DateGeneration::OldCDS ||
                   *_rule == DateGeneration::CDS ||
                   *_rule == DateGeneration::CDS2015) {
            Date next20th = next_twentieth(effective_date, *_rule);
            if (*_rule == DateGeneration::OldCDS) {
                // distance rule inforced in natural days
                static const Date::serial_type stubDays = 30;
                if (next20th - effective_date < stubDays) {
                    // +1 will skip this one and get the next
                    next20th = next_twentieth(next20th + 1, *_rule);
                }
            }
            if (next20th != effective_date) {
                _dates.push_back(next20th);
                _is_regular.push_back(*_rule == DateGeneration::CDS || *_rule == DateGeneration::CDS2015);
                seed = next20th;
            }
        }

        exit_date = termination_date;
        if (_next_to_last_date != Date())
            exit_date = _next_to_last_date;
        for (;;) {
            Date temp = null_calendar.advance(seed, periods*(*_tenor),
                                             convention, *_end_of_month);
            if (temp > exit_date) {
                if (_next_to_last_date != Date() &&
                    (_calendar.adjust(_dates.back(),convention)!=
                     _calendar.adjust(_next_to_last_date,convention))) {
                    _dates.push_back(_next_to_last_date);
                    _is_regular.push_back(false);
                }
                break;
            } else {
                // skip dates that would result in duplicates after adjustment
                if (_calendar.adjust(_dates.back(),convention)!=
                    _calendar.adjust(temp,convention)) {
                    _dates.push_back(temp);
                    _is_regular.push_back(true);
                }
                ++periods;
            }
        }

        if (_calendar.adjust(_dates.back(),termination_date_convention)!=
            _calendar.adjust(termination_date,termination_date_convention)) {
            if (*_rule == DateGeneration::Twentieth ||
                *_rule == DateGeneration::TwentiethIMM ||
                *_rule == DateGeneration::OldCDS ||
                *_rule == DateGeneration::CDS ||
                *_rule == DateGeneration::CDS2015) {
                _dates.push_back(next_twentieth(termination_date, *_rule));
                _is_regular.push_back(true);
            } else {
                _dates.push_back(termination_date);
                _is_regular.push_back(false);
            }
        }

        break;

      default:
        myQL_FAIL("unknown rule (" << int(*_rule) << ")");
    }

    // adjustments
    if (*_rule==DateGeneration::ThirdWednesday)
        for (std::size_t i=1; i<_dates.size()-1; ++i)
            _dates[i] = Date::nth_weekday(3, Wednesday,
                                         _dates[i].month(),
                                         _dates[i].year());
    else if (*_rule == DateGeneration::ThirdWednesdayInclusive)
        for (auto& date : _dates)
            date = Date::nth_weekday(3, Wednesday, date.month(), date.year());

    if (*_end_of_month && _calendar.is_end_of_month(seed)) {
        // adjust to end of month
        if (convention == Unadjusted) {
            for (std::size_t i=1; i<_dates.size()-1; ++i)
                _dates[i] = Date::end_of_month(_dates[i]);
        } else {
            for (std::size_t i=1; i<_dates.size()-1; ++i)
                _dates[i] = _calendar.end_of_month(_dates[i]);
        }
        Date d1 = _dates.front(), d2 = _dates.back();
        if (termination_date_convention != Unadjusted) {
            d1 = _calendar.end_of_month(_dates.front());
            d2 = _calendar.end_of_month(_dates.back());
        } else {
            // the termination date is the first if going backwards,
            // the last otherwise.
            if (*_rule == DateGeneration::Backward)
                d2 = Date::end_of_month(_dates.back());
            else
                d1 = Date::end_of_month(_dates.front());
        }
        // if the eom adjustment leads to a single date schedule
        // we do not apply it
        if(d1 != d2) {
            _dates.front() = d1;
            _dates.back() = d2;
        }
    } else {
        // first date not adjusted for old CDS schedules
        if (*_rule != DateGeneration::OldCDS)
            _dates[0] = _calendar.adjust(_dates[0], convention);
        for (std::size_t i=1; i<_dates.size()-1; ++i)
            _dates[i] = _calendar.adjust(_dates[i], convention);

        // termination date is NOT adjusted as per ISDA
        // specifications, unless otherwise specified in the
        // confirmation of the deal or unless we're creating a CDS
        // schedule
        if (termination_date_convention != Unadjusted
            && *_rule != DateGeneration::CDS
            && *_rule != DateGeneration::CDS2015) {
            _dates.back() = _calendar.adjust(_dates.back(),
                                             termination_date_convention);
        }
    }
    
    // Final safety checks to remove extra next-to-last date, if
    // necessary.  It can happen to be equal or later than the end
    // date due to EOM adjustments (see the Schedule test suite
    // for an example).
    if (_dates.size() >= 2 && _dates[_dates.size()-2] >= _dates.back()) {
        // there might be two dates only, then _is_regular has size one
        if (_is_regular.size() >= 2) {
            _is_regular[_is_regular.size() - 2] =
                (_dates[_dates.size() - 2] == _dates.back());
        }
        _dates[_dates.size() - 2] = _dates.back();
        _dates.pop_back();
        _is_regular.pop_back();
    }
    if (_dates.size() >= 2 && _dates[1] <= _dates.front()) {
        _is_regular[1] =
            (_dates[1] == _dates.front());
        _dates[1] = _dates.front();
        _dates.erase(_dates.begin());
        _is_regular.erase(_is_regular.begin());
    }

    myQL_ENSURE(_dates.size()>1,
        "degenerate single date (" << _dates[0] << ") schedule" <<
        "\n seed date: " << seed <<
        "\n exit date: " << exit_date <<
        "\n effective date: " << effective_date <<
        "\n first date: " << first <<
        "\n next to last date: " << next_to_last <<
        "\n termination date: " << termination_date <<
        "\n generation rule: " << *_rule <<
        "\n end of month: " << *_end_of_month);
    
    
}


Schedule Schedule::after(const Date& truncation_date) const {
    Schedule result = *this;
    
    myQL_REQUIRE(truncation_date < result._dates.back(),
                 "truncation date " << truncation_date <<
                 " must be before the last schedule date " <<
                 result._dates.back());
    
    //TODO: truncate schedule after truncation_date
    return result;
}

Schedule Schedule::until(const Date& truncation_date) const {
    Schedule result = *this;
    
    myQL_REQUIRE(truncation_date>result._dates[0],
                 "truncation date " << truncation_date <<
                 " must be later than schedule first date " <<
                 result._dates[0]);
    
    //TODO: truncate schedule before truncation_date
    return result;
}

Date Schedule::next_date(const Date &ref_date) const {
    auto res = lower_bound(ref_date);
    if(res != _dates.end())
        return *res;
    else
        return {};
}

Date Schedule::prev_date(const Date &ref_date) const {
    auto res = lower_bound(ref_date);
    if(res != _dates.begin())
        return *(--res);
    else
        return {};
}

// MakeSchedule is to make schedule by a rule
MakeSchedule::operator Schedule() const {
    // check for mandatory arguments
    myQL_REQUIRE(_effective_date != Date(), "effective date not provided");
    myQL_REQUIRE(_termination_date != Date(), "termination date not provided");
    myQL_REQUIRE(_tenor, "tenor/frequency not provided");

    // set dynamic defaults:
    BusinessDayConvention convention;
    // if a convention was set, we use it.
    if (_convention) { // NOLINT(readability-implicit-bool-conversion)
        convention = *_convention;
    } else {
        if (!_calendar.empty()) {
            // ...if we set a calendar, we probably want it to be used;
            convention = Following;
        } else {
            // if not, we don't care.
            convention = Unadjusted;
        }
    }

    BusinessDayConvention termination_date_convention;
    // if set explicitly, we use it;
    if (_termination_date_convention) { // NOLINT(readability-implicit-bool-conversion)
        termination_date_convention = *_termination_date_convention;
    } else {
        // Unadjusted as per ISDA specification
        termination_date_convention = convention;
    }

    Calendar calendar = _calendar;
    // if no calendar was set...
    if (calendar.empty()) {
        // ...we use a null one.
        calendar = NullCalendar();
    }

    return Schedule(_effective_date, _termination_date, *_tenor, calendar,
                    convention, termination_date_convention,
                    _rule, _end_of_month, _first_date, _next_to_last_date);
}




}
