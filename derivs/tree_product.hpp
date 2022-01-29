//
//  tree_product.hpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#ifndef tree_product_hpp
#define tree_product_hpp

#include "payoff.hpp"

/*derivative products that can be priced on a tree*/

class TreeProduct{
public:
    TreeProduct(double _final_time):final_time(_final_time){}
    double get_final_time() const {return final_time;}
    
    virtual double final_payoff(double spot) const=0;
    virtual double pre_final_value(double spot, double time, double disc_fut_val) const=0;
    
    virtual ~TreeProduct(){}
    virtual TreeProduct* clone() const=0;
private:
    double final_time;
};


// American option priced on a tree
class TreeAmerican: public TreeProduct{
public:
    TreeAmerican(double _final_time, const PayoffBridge& _payoff): TreeProduct(_final_time), payoff(_payoff){}
    
    double final_payoff(double spot) const override {return payoff(spot);}
    double pre_final_value(double spot, double time, double disc_fut_val) const override;
    
    TreeProduct* clone() const override {return new TreeAmerican(*this);}
private:
    PayoffBridge payoff;
};

// European option priced on a tree
class TreeEuropean: public TreeProduct{
public:
    TreeEuropean(double _final_time, const PayoffBridge& _payoff): TreeProduct(_final_time), payoff(_payoff){}
    
    double final_payoff(double spot) const override {return payoff(spot);}
    double pre_final_value(double spot, double time, double disc_fut_val) const override;
    
    TreeProduct* clone() const override {return new TreeEuropean(*this);}
private:
    PayoffBridge payoff;
};

#endif /* tree_product_hpp */
