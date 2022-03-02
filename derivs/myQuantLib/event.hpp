//
//  event.hpp
//  derivs
//
//  Created by Xin Li on 3/1/22.
//

#ifndef event_hpp
#define event_hpp

#include "observer.hpp"
#include "date.hpp"
#include "visitor.hpp"
#include "errors.hpp"

namespace myQuantLib {

// base class for event
class Event: public Observable {
public:
    ~Event() override = default;
    
    // returns the date at which the event occurs
    virtual Date date() const = 0;
    
    // return true if an event has already occurred before a date
    // if include_ref_date is true, then an event has NOT occurred if its date == ref_date
    // include_ref_date = true -> event happens EOD,23:59:59
    // include_ref_date = false -> event happens BOD, 00:00:01
    // while ref_date = 12:00:00
    virtual bool has_occurred(const Date& ref_date = Date(),
                              bool include_ref_date = false) const {
        if(include_ref_date)
            return date() < ref_date;
        else
            return date() <= ref_date;
    }
    
    // visitability, default behavior
    virtual void accept(AcyclicVisitor& v) {
        auto* v1 = dynamic_cast<Visitor<Event>*>(&v); // if cast failed, fallback to base class
        if(v1 != nullptr)
            v1->visit(*this); // rely on concrete Visitor class to implement visit method
        else
            myQL_FAIL("not an event visitor");
    }
};

}


#endif /* event_hpp */
