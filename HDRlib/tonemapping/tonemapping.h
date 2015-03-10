#ifndef TONEMAPPING_H
#define TONEMAPPING_H
#include "hdrimage.h"
#include "ldrimage.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;


class ToneMapping
{
public:
    ToneMapping(HDRImage * image);
    LDRImage * process();
    LDRImage * process(double q, double k);
    ~ToneMapping();

protected:
    HDRImage * image;
};

#endif // TONEMAPPING_H
