//
//  payoff.hpp
//  derivs
//
//  Created by Xin Li on 1/16/22.
//

#ifndef payoff_hpp
#define payoff_hpp

#include <utility>

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

#endif /* payoff_hpp */
