#ifndef ADAPTIVELOGOPERATOR_H
#define ADAPTIVELOGOPERATOR_H
#include "tonemapping/tonemapping.h"

class AdaptiveLogOperator : public ToneMapping
{
public:
    AdaptiveLogOperator(HDRImage * image);
    LDRImage * process(double q, double k);
    ~AdaptiveLogOperator();
};

#endif // ADAPTIVELOGOPERATOR_H
