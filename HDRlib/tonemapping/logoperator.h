#ifndef LOGOPERATOR_H
#define LOGOPERATOR_H
#include "tonemapping/tonemapping.h"

class LogOperator : public ToneMapping
{
public:
    LogOperator(HDRImage * image, double q, double k);
    LDRImage * process();
    ~LogOperator();
private:
    double q,k;
};

#endif // LOGOPERATOR_H
