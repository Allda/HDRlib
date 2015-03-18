#ifndef REINHARDGLOBALOPERATOR_H
#define REINHARDGLOBALOPERATOR_H
#include "tonemapping/tonemapping.h"

class ReinhardGlobalOperator : public ToneMapping
{
public:
    ReinhardGlobalOperator(HDRImage *image);
    LDRImage * process();
    ~ReinhardGlobalOperator();
};

#endif // REINHARDGLOBALOPERATOR_H
