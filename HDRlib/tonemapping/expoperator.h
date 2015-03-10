#ifndef EXPOPERATOR_H
#define EXPOPERATOR_H
#include "tonemapping/tonemapping.h"


class ExpOperator : public ToneMapping
{
public:
    ExpOperator(HDRImage * image);
    LDRImage * process(double q, double k);
    ~ExpOperator();
};

#endif // EXPOPERATOR_H
