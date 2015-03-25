#ifndef HDRCREATOR_H
#define HDRCREATOR_H
#include <deque>
#include "ldrimage.h"
#include <iostream>
#include "weightfunction.h"
#include "debevecmalikwf.h"
#include "hdrimage.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

class HDRCreator
{
public:
    HDRCreator();
    HDRCreator(vector<LDRImage *> imageList);
    void addVector(vector<LDRImage *> imageList);
    void addImage(LDRImage * i);
    void setWeightFunction(WeightFunction * wf);
    HDRImage *buildHDR();
    double getWFvalue(int i);
    ~HDRCreator();

private:
    vector<LDRImage *> imageList;
    Mat output;
    WeightFunction * wf = NULL;
    double table[256];

    bool checkImages();
    void clearArray(double a[], int size);
    void setPixel(double x,double y,int ch);

};

#endif // HDRCREATOR_H
