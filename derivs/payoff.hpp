//
//  payoff.hpp
//  derivs
//
//  Created by Xin Li on 1/16/22.
//

#ifndef payoff_hpp
#define payoff_hpp

#include <utility>
#include "arglist.hpp"
#include "wrapper.hpp"
#include "factory.hpp"

/* Payoff is a function of spot at expiry, may depend on other paramters like strikes, barrier levels,
 Payoff does not know any concepts of time, discounting, etc. It only factors out functional forms
 */

// base class for Payoff, provide common interface
class Payoff {
public:
    Payoff(){};
    virtual double operator()(double spot) const=0;
    
    virtual Payoff* clone() const=0;
    virtual ~Payoff(){};
};


// Payoff bridge, factor out memory handling (big fives) codes for external classes, so any class using Payoff* can move/copy around its concrete derived Payoff classes. Memory management of entire concrete Payoff classes
class PayoffBridge {
public:
    PayoffBridge(const Payoff& payoff){payoffptr = payoff.clone();}  // implicitly convert from Payoff class
    PayoffBridge():payoffptr(nullptr){} // default constructor
    // copy
    PayoffBridge(const PayoffBridge& rhs){payoffptr = rhs.payoffptr->clone();}
    PayoffBridge& operator=(const PayoffBridge& rhs){
        if(this != &rhs){
            delete payoffptr;
            payoffptr = rhs.payoffptr->clone();
        }
        return *this;
    }
    // move
    PayoffBridge(PayoffBridge&& rhs){
        payoffptr = rhs.payoffptr;
        rhs.payoffptr = nullptr;
    }
    PayoffBridge& operator=(PayoffBridge&& rhs){
        PayoffBridge tmp;
        std::swap<PayoffBridge>(*this, tmp);
        this->payoffptr = rhs.payoffptr;
        rhs.payoffptr = nullptr;
        return *this;
    }
    // destructor
    ~PayoffBridge(){delete payoffptr;};
    
    double operator()(double spot) const {return (*payoffptr)(spot);}
    
private:
    Payoff* payoffptr;
};



// derived classes, strictly speaking base and derived classes are still different types
class CallPayoff: public Payoff {
public:
    // used by factory
    CallPayoff(ArgumentList args){
        if(args.get_struct_name() != "payoff")
            throw("payoff structure expected in CallPayoff class");
        if(args.get_str_arg_val("name") != "call")
            throw("payoff list not for call passed to CallPayoff : got " + args.get_str_arg_val("name"));
        k = args.get_double_arg_val("strike");
        args.check_all_used("CallPayoff");
    }
    CallPayoff(double strike):k(strike){}
    double operator()(double spot) const override {
        return spot > k ? spot - k : 0.0;
    }
    
    Payoff* clone() const override {
        return new CallPayoff(*this);
    }
    
    double get_strike() const {return k;}
    void set_strike(double strike) {k = strike;}
private:
    double k;
};

class PutPayoff: public Payoff {
public:
    PutPayoff(ArgumentList args){
        if(args.get_struct_name() != "payoff")
            throw("payoff structure expected in PutPayoff class");
        if(args.get_str_arg_val("name") != "put")
            throw("payoff list not for put passed to PutPayoff : got " + args.get_str_arg_val("name"));
        k = args.get_double_arg_val("strike");
        args.check_all_used("PutPayoff");
    }
    PutPayoff(double strike):k(strike){}
    double operator()(double spot) const override{
        return spot < k ? k - spot : 0.0;
    }
    
    Payoff* clone() const override {
        return new PutPayoff(*this);
    }
    
    double get_strike() const {return k;}
    void set_strike(double strike){k = strike;}
private:
    double k;
};


class ForwardPayoff: public Payoff {
public:
    ForwardPayoff(ArgumentList args){
        if(args.get_struct_name() != "payoff")
            throw("payoff structure expected in ForwardPayoff class");
        if(args.get_str_arg_val("name") != "forward")
            throw("payoff list not for put passed to ForwardPayoff : got " + args.get_str_arg_val("name"));
        k = args.get_double_arg_val("strike");
        args.check_all_used("ForwardPayoff");
    }
    ForwardPayoff(double strike):k(strike){}
    double operator()(double spot) const override{
        return spot - k;
    }
    Payoff* clone() const override{
        return new ForwardPayoff(*this);
    }
    
    double get_strike() const {return k;}
    
private:
    double k;
};


// linear multiple of two other payoffs
class SpreadPayoff: public Payoff{
public:
    SpreadPayoff(ArgumentList args){
        if(args.get_struct_name() != "payoff")
            throw("payoff structure expected in SpreadPayoff class");
        if(args.get_str_arg_val("name") != "spread")
            throw("payoff list not for spread passed to payoffspread: got " + args.get_str_arg_val("name"));
        if(!args.get_if_present("volume1", volume1))
            volume1 = 1.0;
        if(!args.get_if_present("volume2", volume2))
            volume2 = -1.0;
        
        opt1 = Wrapper<Payoff>
        (get_from_factory<Payoff>(args.get_arglist_arg_val("optionone")));
        opt2 = Wrapper<Payoff>
        (get_from_factory<Payoff>(args.get_arglist_arg_val("optiontwo")));
        
        args.check_all_used("SpreadPayoff");
    }
    
    SpreadPayoff(const Payoff& p1, const Payoff& p2, double v1=1.0, double v2=-1.0)
    :opt1(p1), opt2(p2), volume1(v1), volume2(v2){}
    
    double operator()(double spot) const override{
        return volume1 * (*opt1)(spot) + volume2 * (*opt2)(spot);
    }
    Payoff* clone() const override{
        return new SpreadPayoff(*this);
    }
    
private:
    Wrapper<Payoff> opt1;
    Wrapper<Payoff> opt2;
    double volume1;
    double volume2;
};


#endif /* payoff_hpp */
