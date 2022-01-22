//
//  mjarray.cpp
//  derivs
//
//  Created by Xin Li on 1/22/22.
//

#include <algorithm>
#include <numeric>
#include "mjarray.hpp"

// invariance to keep: sz <= capacity && sz == endptr - valptr;
MJArray::MJArray(unsigned long size): sz(size), capacity(size)
{
    if(sz > 0){
        valptr = new double[size];
        endptr = valptr + size;
    }else{
        valptr = endptr = nullptr;
    }
}

MJArray::MJArray(const MJArray& rhs): sz(rhs.sz), capacity(rhs.capacity)
{
    if(sz > 0){
        valptr = new double[sz];
        endptr = valptr + sz;
        // copy contents over
        std::copy(rhs.valptr, rhs.endptr, valptr);
    }else{
        valptr = endptr = nullptr;
    }
}

MJArray& MJArray::operator=(const MJArray& rhs)
{
    if(this == &rhs) return *this;
    if(rhs.sz > capacity){
        if(capacity > 0)
            delete[] valptr; // not actually needed if nullptr is used
        valptr = new double[rhs.sz];
        capacity = rhs.sz;
    }
    // copy contents over
    sz = rhs.sz;
    endptr = valptr + sz;
    std::copy(rhs.valptr, rhs.endptr, valptr);
    return *this;
}

MJArray::MJArray(MJArray&& rhs):sz(rhs.sz), capacity(rhs.capacity)
{
    // copy pointers, no copy of contents happened, no need to check sz>0
    valptr = rhs.valptr;
    endptr = rhs.endptr;
    rhs.sz = rhs.capacity = 0;
    rhs.valptr = rhs.endptr = nullptr;
}

MJArray& MJArray::operator=(MJArray&& rhs)
{
    sz = rhs.sz;
    capacity = rhs.capacity;
    delete[] valptr;
    valptr = rhs.valptr;
    endptr = valptr + sz;
    rhs.sz = rhs.capacity = 0;
    rhs.valptr = rhs.endptr = nullptr;
    return *this;
}

void MJArray::resize(unsigned long new_size)
{
    if(new_size > capacity){
        delete[] valptr;
        valptr = new double[new_size];
        capacity = new_size;
    }
    sz = new_size;
    endptr = valptr + sz;
}

MJArray& MJArray::operator=(const double& val)
{
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] = val;
    return *this;
}

MJArray& MJArray::operator+=(const MJArray& operand)
{
#ifdef RANGE_CHECKING
    if(sz != operand.sz)
        throw("to apply += two array must be of same size");
#endif
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] += operand[i];
    return *this;
}

MJArray& MJArray::operator-=(const MJArray& operand)
{
#ifdef RANGE_CHECKING
    if(sz != operand.sz)
        throw("to apply -= two array must be of same size");
#endif
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] -= operand[i];
    return *this;
}

MJArray& MJArray::operator/=(const MJArray& operand)
{
#ifdef RANGE_CHECKING
    if(sz != operand.sz)
        throw("to apply /= two array must be of same size");
#endif
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] /= operand[i];
    return *this;
}

MJArray& MJArray::operator*=(const MJArray& operand)
{
#ifdef RANGE_CHECKING
    if(sz != operand.sz)
        throw("to apply *= two array must be of same size");
#endif
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] *= operand[i];
    return *this;
}

MJArray& MJArray::operator+=(const double& operand)
{
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] += operand;
    return *this;
}

MJArray& MJArray::operator-=(const double& operand)
{
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] -= operand;
    return *this;
}

MJArray& MJArray::operator/=(const double& operand)
{
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] /= operand;
    return *this;
}

MJArray& MJArray::operator*=(const double& operand)
{
    for(unsigned long i=0; i<sz; ++i)
        valptr[i] *= operand;
    return *this;
}

double MJArray::sum() const
{
    return std::accumulate(valptr, endptr, 0.0);
}

double MJArray::min() const
{
#ifdef RANGE_CHECKING
    if(sz==0)
        throw("cannot take min of empty array");
#endif
    double* tmp = valptr;
    double* endtmp = endptr;
    return *std::min_element(tmp, endtmp);
}

double MJArray::max() const
{
#ifdef RANGE_CHECKING
    if(sz==0)
        throw("cannot take max of empty array");
#endif
    double* tmp = valptr;
    double* endtmp = endptr;
    return *std::max_element(tmp, endtmp);
}

MJArray MJArray::apply(double f(double)) const
{
    MJArray result(size());
    std::transform(valptr, endptr, result.valptr, f);
    return result;
}

