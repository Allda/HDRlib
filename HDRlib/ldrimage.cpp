#include "ldrimage.h"
#include <iostream>
#include <exiv2/exiv2.hpp>
#include <cstdlib>
#include <cstddef>

using namespace cv;
using namespace std;

// Load image from cv Mat
LDRImage::LDRImage(cv::Mat mat, double exposureTime)
{
    this->mat = mat;
    this->exposureTime = exposureTime;
    width = mat.cols;
    height = mat.rows;

}
// Load image from storage. Must contain EXIF!
LDRImage::LDRImage(const char * name){
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

void LDRImage::setImageMat(Mat mat){
    this->mat = mat;
}

void LDRImage::toString(){
    cout << "Width :" << width << endl
         << "Height: " << height << endl
         << "Exposure time: " << exposureTime << endl
         << "DateTime: " << dateTime << endl;
}

// show image to new window
void LDRImage::showImage(){
    namedWindow( "Display window", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );// Create a window for display.
    imshow( "Display window", mat );                   // Show our image inside it.

    waitKey(0);
}

// load Exif info using Exiv2 library
bool LDRImage::getExifInfo(const char * name){
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(name);
    if (image.get() == 0)
        return false;
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();
    exposureTime = exifData["Exif.Photo.ExposureTime"].toFloat();
    dateTime = exifData["Exif.Image.DateTime"].toString();
    /*aperture = exifData["Exif.Image.ApertureValue"].toFloat();
    iso = exifData["Exif.Image.ISOSpeedRatings"].toLong();*/
    return true;
}

Vec3b  LDRImage::getPixel(int x, int y){
    if(x > width || y > height)
        throw exception();
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

double LDRImage::getAperture(){
    return aperture;
}
long LDRImage::getISO(){
    return iso;
}


// Convert cv Mat image to QImage
// keep original ratio
QImage LDRImage::getQImage(int maxWidth, int maxHeight){
    if(maxWidth < 1)
        maxWidth = 1;
    if(maxHeight < 1)
        maxHeight = 1;
    float ratio;
    bool cols;
    if(maxWidth > mat.cols && maxHeight > mat.rows)
        return this->getQImage();

    // scaled size with original ratio
    if(mat.cols > mat.rows){
        ratio = (float)maxWidth / mat.cols;
        cols = true;
    }
    else{
        ratio = (float)maxHeight / mat.rows;
        cols = false;
    }
    cv::Mat newMat;
    cv::resize(mat,newMat,cv::Size(),ratio, ratio);
    if(cols){
        if(newMat.rows > maxHeight)
            ratio = (float)maxHeight/newMat.rows;
        else
            ratio = 1.0;
    }
    else{
        if(newMat.cols > maxWidth)
            ratio = (float)maxWidth/newMat.cols;
        else
            ratio = 1.0;
    }
    cv::Mat newMat2;
    cv::resize(newMat,newMat2,cv::Size(),ratio,ratio);
    QImage newQimage = this->Mat2QImage(newMat2);
    return newQimage;
}

QImage LDRImage::getQImage(){
    return this->Mat2QImage(this->mat);
}

// Covert cv Mat to QImage
// keep same size
QImage LDRImage::Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

// Save image using cv functions
void LDRImage::saveImage(QString fileName){
    imwrite(fileName.toStdString(),mat);
    //saveExifData(fileName);
}

// save exif info to image file
void LDRImage::saveExifData(QString fileName){
    std::ostringstream strs;
    strs << exposureTime;
    std::string exposureTimeString = strs.str();
    Exiv2::ExifData exifData;
    std::cout << exposureTimeString << std::endl;
    doubleToRational(exposureTime);
    // Exiv2 support only rational format.. can´t save double value
    // => not implemented yet.. double to rational converion is not accurate
    exifData["Exif.Photo.ExposureTime"] = Exiv2::Rational(1,3);
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(fileName.toStdString());
    assert(image.get() != 0);
    image->setExifData(exifData);
    image->writeMetadata();

}

// double to rational conversion => not accurate
void LDRImage::doubleToRational(double num){

      int t=1;
      while((float)t*num!=(int)(t*num))
      {
       //cout<<t*a<<" "<<(float)t*a<<" "<<(int)(t*a)<<endl; // For Checking
       t=t*10;
      }
      int k=gcd(t*num,t);
      cout<<(int)t*num/k<<" / "<<t/k << endl;

}
// subfunction for doubleToRational
int LDRImage::gcd(int a,int b){
  if(b==0)
    return a;
  else
    return gcd(b,a%b);
}

LDRImage::~LDRImage()
{

}

