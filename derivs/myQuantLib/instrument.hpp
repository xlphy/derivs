//
//  instrument.hpp
//  derivs
//
//  Created by Xin Li on 2/14/22.
//

#ifndef instrument_hpp
#define instrument_hpp

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











#endif /* instrument_hpp */
