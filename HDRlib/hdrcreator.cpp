#include "hdrcreator.h"
#include "weightfunction.h"
#include "exceptions/notenoughimagesexception.h"
#include "exceptions/imagesizeexception.h"
#include <iomanip>
#include <math.h>

HDRCreator::HDRCreator()
{

    cout << "Step" <<output.step << endl;
    int min = 0, max = 255;
    double avg = (min + max)/2;
    cout << avg;
    for(int i = 0; i < 256;i++){
        if(0.5*255 > i){
            //cout << (i-min) /avg << endl;
            table[i] = (double)(i-min) ;
        }
        else{
            table[i] = (max - i);
        }
    }
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

HDRImage HDRCreator::buildHDR(){
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

    output = Mat(width,height,CV_32FC3,Scalar(0,0,0));
    //cout <<output << endl;
    /*for(int i = 0; i < 255; i++)
        cout << wf->getWeight(i) << endl;*/
    for(int x = 0; x < width;x++){ // each column
        for(int y = 0; y < height; y++){ // each row
            //cout << "Pixel " << x << " " << y << endl;
            double newPixel[channels];
            clearArray(newPixel,channels);
            double weightSum[channels];
            clearArray(weightSum,channels);
            //cout << fixed << setprecision(5);
            for(int ch = 0; ch < channels; ch++){ // each image


                //cout << "Channel " << ch << endl;
                for(unsigned int i = 0; i < imageList.size();i++){ // each color channel
                    LDRImage * image = imageList.at(i);
                    Vec3f pixel = image->getPixel(x,y);
                    int channelValue = pixel[ch];
                    //cout << channelValue << endl;
                    //double k =  wf->getWeight(channelValue);
                    //cout << newPixel[ch] <<"+ (" << channelValue << " * " << " / " << image->getExposureTime()<< " = ";
                    //cout << "qweqweqwe" << endl;
                    newPixel[ch] += ((channelValue * getWFvalue(channelValue))/image->getExposureTime());
                    //cout << newPixel[ch] << endl;
                    weightSum[ch] += getWFvalue(channelValue);
                }
            }
            for(int ch = 0; ch < channels; ch++){
                //cout << newPixel[ch] << " / " << weightSum[ch];
                newPixel[ch] = newPixel[ch] / weightSum[ch];
                //cout << " =  " << newPixel[ch] << endl;
                output.at<Vec3f>(x,y)[ch] = newPixel[ch];
                //cout <<"...."<< x << " " << y << " " << ch << " " << (double)output.at<Vec3f>(x,y)[ch] << " " <<(double)newPixel[ch] << endl;
            }
            //cout << output << endl;
            //cout << "x: " << x << " y: " << y << " " << width<<" " << height<< endl;
        }
        //cout << "hey" << endl;
    }
    double min, max;
    cv::minMaxLoc(output, &min, &max);
    cout << "MIN: " << min << " MAX: " <<max << endl;
    double e = 255/max;
    Mat output2 = Mat(width,height,CV_8UC3,Scalar(0,0,0));
    for(int x = 0; x < width;x++){
        for(int y = 0; y < height;y++){
            for(int ch = 0; ch < channels;ch++){
                output2.at<Vec3b>(x,y)[ch] = (int)((log10(1 + output.at<Vec3f>(x,y)[ch] * 0.7)/log10(1+0.7*max))*255);
            }
        }
    }

    cout << output2<< endl;
    namedWindow( "Display window", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );// Create a window for display.
    imshow( "Display window", output2 );
    //cout << output << endl;
    return output;

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

