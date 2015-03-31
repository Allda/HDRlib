#include "adaptivelogoperator.h"

AdaptiveLogOperator::AdaptiveLogOperator(HDRImage * image)
    :ToneMapping(image)
{

}

LDRImage * AdaptiveLogOperator::process(double q, double k){
    double delta = 0.01;
    int width = image->getMat().cols;
    int height = image->getMat().rows;
    int channels = image->getMat().channels();

    double bias = log(k)/log(0.5);

    double max[channels];
    for(int i = 0; i < channels;i++)
        max[i] = 0;
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            for(int ch = 0; ch < channels; ch++){
                if(image->getMat().at<Vec3f>(y,x)[ch] > max[ch])
                    max[ch] = image->getMat().at<Vec3f>(y,x)[ch];
            }
        }
    }
    Mat output = Mat(height,width, CV_8UC3);

    double a;
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            for(int ch = 0; ch < channels; ch++){
                double val = image->getMat().at<Vec3f>(y,x)[ch];
                a = ((q * delta)/log10(max[ch]+1)) * (log(val+1)/log(2+(pow(val/max[ch],bias))*8));
                output.at<Vec3b>(y,x)[ch] = a;
                //output.at<Vec3b>(y,x)[ch] *= 255;
            }
        }
    }
    return new LDRImage(output);
}

AdaptiveLogOperator::~AdaptiveLogOperator()
{

}

