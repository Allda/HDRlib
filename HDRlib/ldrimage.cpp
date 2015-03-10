#include "ldrimage.h"
#include <iostream>
#include <exiv2/exiv2.hpp>
#include <cstdlib>
#include <cstddef>
using namespace cv;
using namespace std;

LDRImage::LDRImage(cv::Mat mat, double exposureTime)
{
    this->mat = mat;
    this->exposureTime = exposureTime;
    width = mat.cols;
    height = mat.rows;

}
LDRImage::LDRImage(const char * name){
    cout << "nahravam " << name << endl;
    mat = imread(name, CV_LOAD_IMAGE_COLOR);   // Read the file
    if(!mat.data){
        cout << "Nepodarilo se nahrat data" << endl;
    }
    width = mat.cols;
    height = mat.rows;
    this->getExifInfo(name);
}

LDRImage::LDRImage(Mat mat){
    this->mat = mat;
}

Mat LDRImage::getImageMat(){
    return mat;
}

void LDRImage::toString(){
    cout << "Width :" << width << endl
         << "Height: " << height << endl
         << "Exposure time: " << exposureTime << endl
         << "DateTime: " << dateTime << endl;
}

void LDRImage::showImage(){
    namedWindow( "Display window", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );// Create a window for display.
    imshow( "Display window", mat );                   // Show our image inside it.

    waitKey(0);
}

bool LDRImage::getExifInfo(const char * name){
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(name);
    if (image.get() == 0)
        return false;
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();
    exposureTime = exifData["Exif.Photo.ExposureTime"].toFloat();
    dateTime = exifData["Exif.Image.DateTime"].toString();


    return true;
}

Vec3b  LDRImage::getPixel(int x, int y){
    if(x > width || y > height)
        return NULL;
    return mat.at<Vec3b>(y,x);
}

int LDRImage::getHeight(){
    return height;
}

int LDRImage::getWidth(){
    return width;
}

double LDRImage::getExposureTime(){
    return exposureTime;
}

LDRImage::~LDRImage()
{

}

