//
//  tree.cpp
//  derivs
//
//  Created by Xin Li on 1/29/22.
//

#include "tree.hpp"

BinomialTree::BinomialTree(double _spot,
                           const Parameters& _r,
                           const Parameters& _d,
                           double _vol,
                           unsigned long _steps,
                           double _time
                           )
: spot(_spot), r(_r), d(_d), vol(_vol), steps(_steps), time(_time), discounts(steps)
{
    tree_built = false;
}

/*build a underlying tree*/
void BinomialTree::build_tree()
{
    tree_built = true;
    tree.resize(steps+1);
    
    double init_log_spot = std::log(spot);
    // populate spot prices
    for(unsigned long i=0; i<=steps; ++i){
        tree[i].resize(i+1);
        double ti = (i * time) / steps;
        double drift_log_spot =
        init_log_spot + r.integrate(0.0, ti) - d.integrate(0.0, ti)
        - 0.5 * vol * vol * ti;
        double sd = vol * std::sqrt(time / steps);
        for(long j=-static_cast<long>(i), k=0;
            j <= static_cast<long>(i); j += 2, k++)
            tree[i][k].first = std::exp(drift_log_spot + j * sd);
    }
    // compute discount factor for each tree step
    for(unsigned long l=0; l<steps; l++)
        discounts[l] =
        std::exp(-r.integrate(l * time / steps, (l+1) * time / steps));
}

// use underlying spot tree to price TreeProduct
double BinomialTree::get_price(const TreeProduct& product)
{
    if(!tree_built) build_tree();
    
    if(product.get_final_time() != time)
        throw("Mismatched product and binomial tree time!");
    // populate final payoff at tree[steps]
    for(long j = -static_cast<long>(steps), k=0;
        j <= static_cast<long>(steps); j+=2, k++)
        tree[steps][k].second = product.final_payoff(tree[steps][k].first);
    // back propagate fair values
    for(unsigned long i=1; i<= steps; ++i){
        unsigned long idx = steps - i;
        double ti = idx * time / steps;
        for(long j=-static_cast<long>(idx), k=0;
            j <= static_cast<long>(idx); j+=2, k++){
            double s = tree[idx][k].first;
            double val = discounts[idx]
            * 0.5 * (tree[idx+1][k].second + tree[idx+1][k+1].second);
            tree[idx][k].second = product.pre_final_value(s, ti, val);
        }
    }
    return tree[0][0].second;
}
