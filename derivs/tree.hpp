//
//  tree.hpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#ifndef tree_hpp
#define tree_hpp

/*Tree is used as a discrete approximation of continuous stochastic price evolution, it is not a non-arbitrage tree, the non-arbitrage price is justified by its continuous version. One consequence is that the forward price is no longer exact but only approximated
 Price multiple products with same expiry, call methods many times but only build the tree once
 */
#include <vector>
#include "mjarray.hpp"
#include "parameters.hpp"
#include "tree_product.hpp"


class BinomialTree{
public:
    BinomialTree(double _spot,
                 const Parameters& _r,
                 const Parameters& _d,
                 double _vol, // constant vol
                 unsigned long _steps,
                 double _time
                 );
    double get_price(const TreeProduct& product);
    
protected:
    void build_tree();
private:
    double spot;
    Parameters r;
    Parameters d;
    double vol;
    unsigned long steps;
    double time;
    bool tree_built;
    
    std::vector<std::vector<std::pair<double, double>>> tree;
    MJArray discounts;
};


#endif /* tree_hpp */
