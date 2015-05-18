#include "reinhardglobaloperator.h"

ReinhardGlobalOperator::ReinhardGlobalOperator(HDRImage *image)
    :ToneMapping(image)
{

}

LDRImage * ReinhardGlobalOperator::process(){
    double count = width * height;
    double delta = 0.01;
    double Lw[channels];
    memset(Lw,0,image->getMat().channels());

    // Firts part of algorithm -> count Lw for every channel
    for(int x = 0; x < width;x++){
        for(int y = 0; y < height;y++){
            for(int ch = 0; ch < channels;ch++){
                Lw[ch] += log(image->getMat().at<Vec3f>(y,x)[ch] + delta);
            }
        }
    }
    for(int i = 0; i < 3; i++){
        Lw[i] = exp((1/count)*Lw[i]);
    }

    Mat LMat = Mat(height,width,CV_32FC3);
    Mat output = Mat(height,width, CV_8UC3);
    double max[channels];
    for(int i = 0; i < channels;i++)
        max[i] = 0;
    // get max value of each channel
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            for(int ch = 0; ch < channels; ch++){
                if(image->getMat().at<Vec3f>(y,x)[ch] > max[ch])
                    max[ch] = image->getMat().at<Vec3f>(y,x)[ch];
            }
        }
    }
    for(int i = 0; i < channels;i++)
        max[i] *= max[i];
    //cv::minMaxLoc(image->getMat(), &min, &max);
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            for(int ch = 0; ch < channels; ch++){
                // Lvalue
                LMat.at<Vec3f>(y,x)[ch] = (0.72/Lw[ch]) * image->getMat().at<Vec3f>(y,x)[ch];
                double numerator = LMat.at<Vec3f>(y,x)[ch] * (1+(LMat.at<Vec3f>(y,x)[ch]/(max[ch])));
                // output value
                output.at<Vec3b>(y,x)[ch] = (numerator/(1+LMat.at<Vec3f>(y,x)[ch]))*255;
            }
        }
    }
    return new LDRImage(output);
}

ReinhardGlobalOperator::~ReinhardGlobalOperator()
{

}

