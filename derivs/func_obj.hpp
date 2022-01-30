//
//  func_obj.hpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#ifndef func_obj_hpp
#define func_obj_hpp

/*function objects, class usually implements operator(); can be used with solvers*/

// treat it as a function of vol, others are parameters
class BSCall{
public:
    BSCall(double _r, double _d, double _ttx, double _spot, double _strike)
    :r(_r), d(_d), ttx(_ttx), spot(_spot), strike(_strike){}
    double operator()(double vol) const;
    
private:
    double r;
    double d;
    double ttx;
    double spot;
    double strike;
};

class BSPut{
public:
    BSPut(double _r, double _d, double _ttx, double _spot, double _strike)
    :r(_r), d(_d), ttx(_ttx), spot(_spot), strike(_strike){}
    double operator()(double vol) const;
    
private:
    double r;
    double d;
    double ttx;
    double spot;
    double strike;
};

// output both value and its derivative
class BSCall2{
public:
    BSCall2(double _r, double _d, double _ttx, double _spot, double _strike)
    :r(_r), d(_d), ttx(_ttx), spot(_spot), strike(_strike){}
    
    double price(double vol) const;
    double vega(double vol) const;
    
private:
    double r;
    double d;
    double ttx;
    double spot;
    double strike;
    
    
};

class BSPut2{
public:
    BSPut2(double _r, double _d, double _ttx, double _spot, double _strike)
    :r(_r), d(_d), ttx(_ttx), spot(_spot), strike(_strike){}
    
    double price(double vol) const;
    double vega(double vol) const;
    
private:
    double r;
    double d;
    double ttx;
    double spot;
    double strike;
};


#endif /* func_obj_hpp */
