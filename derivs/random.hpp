//
//  random.hpp
//  derivs
//
//  Created by Xin Li on 1/16/22.
//

#ifndef random_hpp
#define random_hpp

/*facilities to generate random numbers,
 write your own random number generator to achieve:
 1) consistent results across different compilers, compiler-independent random number streams
 2) insulate random number stream generations from unrelated parts of program, avoid global srand command
 3) extensible to use different kinds of random number generating methodologies through decorators
 4) unify the user interfaces, flexible to change / replace random number streams, like using Low-discrepancy numbers
 */

#include "mjarray.hpp"

class RandomBase{
public:
    RandomBase(unsigned long _dim):dim(_dim){};
    
    // must implement
    virtual RandomBase* clone() const=0;
    virtual void get_uniforms(MJArray& variates)=0;
    virtual void skip(unsigned long num_of_paths)=0;
    virtual void set_seed(unsigned long seed)=0;
    virtual void reset()=0;
    
    // provide sensible defaults
    virtual void get_gaussians(MJArray& variates);
    virtual void reset_dim(unsigned long new_dim){dim = new_dim;}
    
    unsigned long get_dim() const {return dim;}
    
    
private:
    unsigned long dim; // the number of independent random numbers needed for one-go (i.e., simulating one-path)
};

//Park Miller the minimal standard generator, a linear congruential generator and adapter pattern
class ParkMiller{
public:
    ParkMiller(long _seed=1);
    long get_one_integer();
    void set_seed(long _seed);
    
    static unsigned long Max();
    static unsigned long Min();
    
private:
    long seed;
};

// adapt ParkMiller to RandomBase
class RandomParkMiller: public RandomBase{
public:
    RandomParkMiller(unsigned long _dim, unsigned long _seed)
    : RandomBase(_dim), inner_generator(_seed), init_seed(_seed){
        // convertion factor from random integers in [min, max] to (0, 1), open interval
        reciprocal = 1 / (1.0 + inner_generator.Max());
    }
    RandomBase* clone() const override {return new RandomParkMiller(*this);}
    void get_uniforms(MJArray& variates) override {
        for(unsigned long i=0; i<get_dim(); ++i)
            variates[i] = inner_generator.get_one_integer() * reciprocal;
    }
    
    void skip(unsigned long num_of_paths) override {
        // draw num of runs, and throw them away
        MJArray tmp(get_dim());
        for(unsigned long i=0; i<num_of_paths; ++i) get_uniforms(tmp);
    }
    void set_seed(unsigned long _seed) override {
        init_seed = _seed;
        inner_generator.set_seed(_seed);
    }
    void reset() override {inner_generator.set_seed(init_seed);}
    void reset_dim(unsigned long new_dim) override {
        RandomBase::reset_dim(new_dim);
        inner_generator.set_seed(init_seed);
    }
    
private:
    ParkMiller inner_generator;
    unsigned long init_seed;
    double reciprocal;
};





double get_one_uniform(double lower=0.0, double upper=1.0);
double get_one_gaussian_by_BoxMuller();
double get_one_gaussian_by_sum();


#endif /* random_hpp */
