#include "debevecmalikwf.h"
#include <iostream>


using namespace std;
DebevecMalikWF::DebevecMalikWF()
{
    int min = 0, max = 255;
    double avg = (min + max)/2;
    cout << avg;
    for(int i = 0; i < 256;i++){
        if(0.5*255 > i){
            //cout << (i-min) /avg << endl;
            table[i] = (double)(i-min) ;
        }
        else{
            table[i] = (max - i);
        }
    }

    /*for(int i = 0; i < 256; i++)
        cout << i <<" : " << scientific <<table[i]<< endl;*/

}

double DebevecMalikWF::getWeight(unsigned char i){
    //cout << (int)i << ":" << table[i]<< endl;
    if(i == 0 || i == 255)
        return table[1];
    return table[i];
}

DebevecMalikWF::~DebevecMalikWF()
{

}

