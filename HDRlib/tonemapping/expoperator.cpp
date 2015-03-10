#include "expoperator.h"

ExpOperator::ExpOperator(HDRImage * image)
    :ToneMapping(image)
{

}

LDRImage * ExpOperator::process(double q, double k){
    double min, max;
    int width = image->getMat().cols;
    int height = image->getMat().rows;
    int channels = image->getMat().channels();
    cv::minMaxLoc(image->getMat(), &min, &max);
    cout << "MIN: " << min << " MAX: " <<max << endl;
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

