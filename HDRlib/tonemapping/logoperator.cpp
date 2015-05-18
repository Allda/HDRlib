#include "logoperator.h"

LogOperator::LogOperator(HDRImage * image, double q, double k)
    :ToneMapping(image)
{
    this->q = q;
    this->k = k;
}

LDRImage * LogOperator::process(){
    double min, max;
    cv::minMaxLoc(image->getMat(), &min, &max);
    Mat output = Mat(height,width,CV_8UC3,Scalar(0,0,0));
    for(int x = 0; x < width;x++){
        for(int y = 0; y < height;y++){
            for(int ch = 0; ch < channels;ch++){
                output.at<Vec3b>(y,x)[ch] = (int)((log10(1 + image->getMat().at<Vec3f>(y,x)[ch] * q)/log10(1+k*max))*255);
            }
        }
    }

    LDRImage * ldrImage = new LDRImage(output);
    return ldrImage;
}

LogOperator::~LogOperator()
{

}

