#ifndef LDRIMAGE_H
#define LDRIMAGE_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <QImage>

using namespace std;
using namespace cv;

class LDRImage
{
public:
    LDRImage(cv::Mat mat, double exposureTime);
    LDRImage(const char *name);

    // constructor without exposure
    LDRImage(Mat mat);

    ~LDRImage();
    int getWidth();
    int getHeight();
    Mat getImageMat();
    void setImageMat(Mat mat);
    void toString();
    void showImage();
    bool getExifInfo(const char * name);
    double getExposureTime();
    double getAperture();
    long getISO();
    Vec3b getPixel(int x, int y);
    QImage getQImage(int maxWidth, int maxHeight);
    QImage getQImage();
    QImage Mat2QImage(cv::Mat const& src);
    void saveImage(QString fileName);


private:
    void saveExifData(QString fileName);
    double exposureTime, aperture;
    long iso;
    cv::Mat mat;
    double width, height;
    string dateTime;
    int gcd(int a,int b);
    void doubleToRational(double num);
};

#endif // LDRIMAGE_H
