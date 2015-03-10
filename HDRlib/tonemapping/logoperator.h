#ifndef LOGOPERATOR_H
#define LOGOPERATOR_H
#include "tonemapping/tonemapping.h"

class LogOperator : public ToneMapping
{
public:
    LogOperator(HDRImage * image);
    LDRImage * process(double q, double k);
    ~LogOperator();
};

#endif // LOGOPERATOR_H
