#include "linearoperator.h"

LinearOperator::LinearOperator(HDRImage * image)
    :ToneMapping(image)
{


}


LDRImage * LinearOperator::process(){
    double min, max;
    // get maximum for constatnt e
    cv::minMaxLoc(image->getMat(), &min, &max);
    double e = 255/max;
    Mat output = Mat(height,width,CV_8UC3,Scalar(0,0,0));
    for(int x = 0; x < width;x++){
        for(int y = 0; y < height;y++){
            for(int ch = 0; ch < channels;ch++){
                output.at<Vec3b>(y,x)[ch] = (int)(image->getMat().at<Vec3f>(y,x)[ch] * e);
            }
        }
    }
    LDRImage * ldrImage = new LDRImage(output);
    return ldrImage;
}

LinearOperator::~LinearOperator()
{

}

