#include "expoperator.h"

ExpOperator::ExpOperator(HDRImage * image, double q, double k)
    :ToneMapping(image)
{
    this->q = q;
    this->k = k;

}

LDRImage * ExpOperator::process(){
    double min, max;
    cv::minMaxLoc(image->getMat(), &min, &max);
    // new cv Mat for output image
    Mat output = Mat(height,width,CV_8UC3,Scalar(0,0,0));
    for(int x = 0; x < width;x++){
        for(int y = 0; y < height;y++){
            for(int ch = 0; ch < channels;ch++){
                output.at<Vec3b>(y,x)[ch] = (1-(exp(-(image->getMat().at<Vec3f>(y,x)[ch] * q)/(k*max))))*255;
            }
        }
    }

    LDRImage * ldrImage = new LDRImage(output);
    return ldrImage;
}

ExpOperator::~ExpOperator()
{

}

