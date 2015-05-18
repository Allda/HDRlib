#ifndef WEIGHTFUNCTION_H
#define WEIGHTFUNCTION_H


class WeightFunction
{
public:
    WeightFunction();
    ~WeightFunction();
    virtual double getWeight(unsigned char i) = 0;

protected:
    char table[256];
};

#endif // WEIGHTFUNCTION_H
