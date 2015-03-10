#ifndef LINEAROPERATOR_H
#define LINEAROPERATOR_H
#include "tonemapping/tonemapping.h"
#include "ldrimage.h"

class LinearOperator : public ToneMapping
{
public:
    LinearOperator(HDRImage * image);
    LDRImage * process();
    ~LinearOperator();

};

#endif // LINEAROPERATOR_H
