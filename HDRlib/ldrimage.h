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
    void toString();
    void showImage();
    bool getExifInfo(const char * name);
    double getExposureTime();
    Vec3b getPixel(int x, int y);
    QImage getQImage(int maxWidth, int maxHeight);
    QImage getQImage();
    QImage Mat2QImage(cv::Mat const& src);


private:

    double exposureTime;
    cv::Mat mat;
    double width, height;
    string dateTime;
};

#endif // LDRIMAGE_H
