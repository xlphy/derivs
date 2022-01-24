//
//  anti_thetic.hpp
//  derivs
//
//  Created by Xin Li on 1/24/22.
//

#ifndef anti_thetic_hpp
#define anti_thetic_hpp

/*apply anti-thetic sampling to any random number generator, decorator pattern*/
#include "random.hpp"
#include "wrapper.hpp"

class AntiThetic: public RandomBase{
public:
    AntiThetic(const Wrapper<RandomBase>& _inner);
    RandomBase* clone() const override {return new AntiThetic(*this);}
    void get_uniforms(MJArray& variates) override;
    void skip(unsigned long num_of_paths) override;
    void set_seed(unsigned long _seed) override;
    void reset() override;
    
    void reset_dim(unsigned long new_dim) override;
    
private:
    Wrapper<RandomBase> inner_generator;
    bool gen_next; // whether to use inner_generator for next draw
    MJArray next_variates;
};


#endif /* anti_thetic_hpp */
