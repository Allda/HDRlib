#ifndef EXPOPERATOR_H
#define EXPOPERATOR_H
#include "tonemapping/tonemapping.h"


class ExpOperator : public ToneMapping
{
public:
    ExpOperator(HDRImage * image, double q, double k);
    LDRImage * process();
    ~ExpOperator();
private:
    double q,k;
};

#endif // EXPOPERATOR_H
