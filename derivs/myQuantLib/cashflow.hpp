//
//  cashflow.hpp
//  derivs
//
//  Created by Xin Li on 3/2/22.
//

#ifndef cashflow_hpp
#define cashflow_hpp

#include "event.hpp"
#include "comparison.hpp"

namespace myQuantLib {

// base class
class CashFlow : public Event {
public:
    ~CashFlow() override = default;
    // Event interface
    Date date() const override = 0;
    bool has_occurred(const Date& ref_date=Date(),
                      bool include_ref_date = false) const override {
        if(ref_date != Date()){
            Date cf = date();
            if(ref_date < cf)
                return false;
            if(cf < ref_date)
                return true;
        }
        return Event::has_occurred(ref_date, include_ref_date);
    }
    // CashFlow interface
    virtual double amount() const = 0;
    // returns the date that cash flow trades exCoupon
    virtual Date exCoupon_date() const {return {};}
    // returns true if the cashflow is trading ex-coupon on the ref_date
    bool trading_exCoupon(const Date& ref_date=Date()) const {
        Date ecd = exCoupon_date();
        if(ecd == Date()) return false;
        return ecd <= ref_date;
    }
    
    // visitable
    void accept(AcyclicVisitor& v) override {
        auto* v1 = dynamic_cast<Visitor<CashFlow>*>(&v);
        if (v1 != nullptr)
            v1->visit(*this);
        else
            Event::accept(v);
    }
};

typedef std::vector<std::shared_ptr<CashFlow>> Leg;


template <>
struct earlier_than<CashFlow> {
    typedef CashFlow first_argument_type;
    typedef CashFlow second_argument_type;
    typedef bool result_tpye;
    
    bool operator() (const CashFlow& c1,
                     const CashFlow& c2) const {
        return c1.date() < c2.date();
    }
};


}


#endif /* cashflow_hpp */
