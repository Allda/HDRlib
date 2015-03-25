#include "debevecmalikwf.h"
#include <iostream>


using namespace std;
DebevecMalikWF::DebevecMalikWF()
{
    int min = 0, max = 255;
    for(int i = 0; i < 256;i++){
        if(0.5*255 > i){
            table[i] = (double)(i-min) ;
        }
        else{
            table[i] = (max - i);
        }
    }

}

double DebevecMalikWF::getWeight(unsigned char i){
    if(i == 0 || i == 255)
        return table[1];
    return table[i];
}

DebevecMalikWF::~DebevecMalikWF()
{

}

