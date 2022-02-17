//
//  instrument.hpp
//  derivs
//
//  Created by Xin Li on 2/14/22.
//

/*What we want with instrument class
 1. Generic enough to capture all kinds of financial instruments, we can write something like
 for (i = portfolio.begin(); i != portfolio.end(); ++i)
    total_npv += i->npv()
 preliminary interface:
class Instrument{
 public:
    virtual ~Instrument();
    virtual Real npv() const = 0;
    virtual Real error_estimate() const = 0;
    virtual bool is_expired() const = 0;
 };
 
 2. Be able to use different pricing logic or methods to evaluate present values of a given instrument
 Strategy Pattern, use pricing engine class
 
 3. Connected to market data, be able to update / recal as demand (i.e. new data updated) or switch easily to a different market data source
 Observer Pattern, implement caching mechanism, recalculate only when inputs are changed or it is instructed to
 
 */



#ifndef instrument_hpp
#define instrument_hpp


#include "observer.hpp"

namespace myQuantLib{

class LazyObject: public virtual Observer, public virtual Observable {
public:
    void update() override {calculated_ = false;}  // implements Observer update
    virtual void calculate() const {
        if(!calculated_){
            calculated_ = true;  // set first, terminate for recursive calls
            try {
                do_calculation();
            }catch(...){
                calculated_ = false;
                throw("do_calculation failed!");
            }
        }
        // do nothing if calculated_ = true
    }
protected:
    LazyObject(){};
    mutable bool calculated_;
    virtual void do_calculation() const = 0;
};

class Instrument : public LazyObject{
protected:
    mutable double _npv;
public:
    virtual ~Instrument(){};
    // to be implemented in derived classes
    // virtual void do_calculation() const (from LazyObject)
    virtual double error_estimate() const {return 0.0;}  // 0.0 means no error estimate is available.
    virtual bool is_expired() const = 0;
    virtual void setup_expired() const {_npv = 0.0;}
    
    double NPV() const {
        calculate(); // set _npv
        return _npv;
    }
    void calculate() const override {
        if(is_expired()){
            // setup expiry
            setup_expired();
            calculated_ = true;
        }else{
            LazyObject::calculate();
        }
    }
};

/*
 A concrete instrument class needs to at least implement:
 1. double error_estimate() const (optional)
 2. bool is_expired() const
 3. void setup_expired() const
 4. void do_calculation() const, requires to set _npv
 To proper setup Observer pattern, one also needs to consider when to use register/unregister functions
 To proper use instrument, one needs to decide when to trigger notify_observers()
 */


}







#endif /* instrument_hpp */
