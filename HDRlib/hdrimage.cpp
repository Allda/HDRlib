#include "hdrimage.h"

HDRImage::HDRImage(Mat mat)
{
    this->image = mat;

}

Mat HDRImage::getMat(){
    return image;
}

HDRImage::~HDRImage()
{

}

