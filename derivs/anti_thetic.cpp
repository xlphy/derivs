//
//  anti_thetic.cpp
//  derivs
//
//  Created by Xin Li on 1/24/22.
//

#include "anti_thetic.hpp"

AntiThetic::AntiThetic(const Wrapper<RandomBase>& _inner)
    : RandomBase(*_inner), inner_generator(_inner) // concrete subclass get copied to inner_generator, create base class from concrete subclass
{
    inner_generator->reset();
    gen_next = true;
    next_variates.resize(get_dim());
}

void AntiThetic::get_uniforms(MJArray& variates)
{
    if(gen_next){
        // draw new sample from inner_generator and update next_variates for next use
        inner_generator->get_uniforms(variates);
        for(unsigned long i=0; i<get_dim(); ++i)
            next_variates[i] = 1.0 - variates[i];
        gen_next = false; // no need gen_next for the next
    }else{
        // use next_variates
        variates = next_variates;
        gen_next = true; // next_variates used need to generate next variates for the next
    }
    
}

// gen_next = true:
// even paths -> skip even / 2 - 1 seeds, update next_variates;
// odd paths -> skip (odd-1) / 2 seeds, use next_variates (???)
// gen_next = false:
// even paths -> skip even / 2 seeds, use next_variates;
// odd paths -> skip (odd-1) / seeds, update next_variates, use next_variates
void AntiThetic::skip(unsigned long num_of_paths)
{
    if(num_of_paths==0) return;
    if(gen_next){
        gen_next = false;
        num_of_paths--;
    }
    inner_generator->skip(num_of_paths/2);
    if(num_of_paths % 2){
        MJArray tmp(get_dim());
        get_uniforms(tmp);
    }
}

void AntiThetic::set_seed(unsigned long _seed)
{
    inner_generator->set_seed(_seed);
    gen_next = true;
}

void AntiThetic::reset_dim(unsigned long new_dim)
{
    RandomBase::reset_dim(new_dim);
    next_variates.resize(new_dim);
    inner_generator->reset_dim(new_dim);
}

void AntiThetic::reset()
{
    inner_generator->reset();
    gen_next = true;
}

