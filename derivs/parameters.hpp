//
//  parameters.hpp
//  derivs
//
//  Created by Xin Li on 1/19/22.
//

#ifndef parameters_hpp
#define parameters_hpp

/*store market or model parameters like interest rate, volaility, etc
 add flexibility to encapsulate more complex structure of parameters (e.g., term structure vols)
 add functions to calculate useful measure of parameters like averages and variances
 */

#include <cmath>

// base class, define interface, keep only minimal and generic interfaces
class ParametersInner{
public:
    // base class stub
    ParametersInner(){}
    virtual ParametersInner* clone() const=0;
    virtual ~ParametersInner(){};
    
    // base class interfaces
    virtual double integrate(double from_time, double to_time) const=0;
    virtual double integrate_square(double from_time, double to_time) const=0;
    
private:
    // no data members, define only minimal interfaces
};

// bridge class, handle copy/move
class Parameters{
public:
    // bridge stub
    // stub-constructor
    Parameters(const ParametersInner& param){p = param.clone();}
    Parameters():p(nullptr){}
    // stub-copy
    Parameters(const Parameters& rhs){p = rhs.p->clone();}
    Parameters& operator=(const Parameters& rhs){
        if(this != &rhs){
            delete p;
            p = rhs.p->clone();
        }
        return *this;
    }
    // stub-move
    Parameters(Parameters&& rhs){
        p = rhs.p;
        rhs.p = nullptr;
    }
    Parameters& operator=(Parameters&& rhs){
        delete p;
        p = rhs.p;
        rhs.p = nullptr;
        return *this;
    }
    // stub-destructor
    ~Parameters(){delete p;}
    
    // bridge interfaces
    double integrate(double from_time, double to_time) const {return p->integrate(from_time, to_time);}
    double integrate_square(double from_time, double to_time) const {return p->integrate_square(from_time, to_time);}
    
    // additional interfaces
    double mean(double from_time, double to_time) const {
        return integrate(from_time, to_time) / (to_time - from_time);
    }
    double root_mean_square(double from_time, double to_time) const{
        return std::sqrt(integrate_square(from_time, to_time) / (to_time - from_time));
    }
    
private:
    ParametersInner* p;
};


// concrete ParametersInner class
class ParametersConstant: public ParametersInner{
public:
    ParametersConstant(double val): c(val), c2(val*val){}
    ParametersInner* clone() const override {
        return new ParametersConstant(*this);
    }
    double integrate(double from_time, double to_time) const override {
        return c * (to_time - from_time);
    }
    double integrate_square(double from_time, double to_time) const override {
        return c2 * (to_time - from_time);
    }
    
private:
    double c;
    double c2;
};



#endif /* parameters_hpp */
