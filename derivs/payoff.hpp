//
//  payoff.hpp
//  derivs
//
//  Created by Xin Li on 1/16/22.
//

#ifndef payoff_hpp
#define payoff_hpp

/* Payoff is a function of spot at expiry, may depend on other paramters like strikes, barrier levels */

// base class for Payoff, provide common interface
class Payoff {
public:
    Payoff(){};
    virtual double operator()(double spot) const=0;
    
    virtual Payoff* clone() const=0;
    virtual ~Payoff(){};
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


#endif /* payoff_hpp */
