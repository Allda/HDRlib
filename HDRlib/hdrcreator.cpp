#include "hdrcreator.h"
#include "weightfunction.h"
#include "exceptions/notenoughimagesexception.h"
#include "exceptions/imagesizeexception.h"
#include <iomanip>
#include <math.h>
#include "hdrimage.h"

HDRCreator::HDRCreator()
{

}

HDRCreator::HDRCreator(deque<LDRImage *> imageList){
    this->imageList = imageList;
}

void HDRCreator::addImage(LDRImage *i){
    imageList.push_back(i);
}

void HDRCreator::setWeightFunction(WeightFunction *wf){
    this->wf = wf;
}

bool HDRCreator::checkImages(){
    int width = imageList.at(0)->getImageMat().cols;
    int height = imageList.at(0)->getImageMat().rows;
    for(unsigned int i = 1; i < imageList.size();i++){
        LDRImage * img = imageList.at(i);
        if(img->getImageMat().cols != width)
            return false;
        if(img->getImageMat().rows != height)
            return false;
    }
    return true;
}

HDRImage * HDRCreator::buildHDR(){
    if(imageList.size() < 3){
        throw NotEnoughImagesException();
    }
    if(!checkImages())
        throw ImageSizeException();
    if(wf == NULL)
        throw exception();

    int width = imageList.at(0)->getImageMat().cols;
    int height = imageList.at(0)->getImageMat().rows;
    int channels = imageList.at(0)->getImageMat().channels();

    double newPixel[channels];
    double weightSum[channels];

    output = Mat(height,width,CV_32FC3,Scalar(0,0,0));

    for(int x = 0; x < width;x++){ // each column
        for(int y = 0; y < height; y++){ // each row
            clearArray(newPixel,channels);
            clearArray(weightSum,channels);
            for(int ch = 0; ch < channels; ch++){ // each color channel
                for(unsigned int i = 0; i < imageList.size();i++){ // each image
                    LDRImage * image = imageList.at(i);
                    Vec3f pixel = image->getPixel(x,y);
                    int channelValue = pixel[ch];
                    newPixel[ch] += ((channelValue * wf->getWeight(channelValue))/image->getExposureTime());
                    weightSum[ch] += wf->getWeight(channelValue);
                }
            }
            for(int ch = 0; ch < channels; ch++){
                newPixel[ch] = newPixel[ch] / weightSum[ch];
                output.at<Vec3f>(y,x)[ch] = newPixel[ch];
            }
        }
    }
    double min, max;
    cout << output.cols << "x" << output.rows << endl;
    cout << width << "x" << height << endl;

    HDRImage * hdrImage = new HDRImage(output);
    return hdrImage;



}

void HDRCreator::clearArray(double a[], int size){
    for(int i = 0; i < size; i++){
        a[i] = 0;
    }
}

void HDRCreator::setPixel(double x, double y, int ch){

}

double HDRCreator::getWFvalue(int i){
    if(i == 0 || i == 255)
        return table[1];
    return table[i];
}

HDRCreator::~HDRCreator()
{

}

