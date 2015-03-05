#ifndef DEBEVECMALIKWF_H
#define DEBEVECMALIKWF_H
#include "weightfunction.h"

class DebevecMalikWF : public WeightFunction
{
public:
    DebevecMalikWF();
    double getWeight(unsigned char i);
    ~DebevecMalikWF();
};

#endif // DEBEVECMALIKWF_H
