#ifndef LDRIMAGE_H
#define LDRIMAGE_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>

using namespace std;
using namespace cv;

class LDRImage
{
public:
    LDRImage(cv::Mat mat, int exposureTime);
    LDRImage(const char *name);
    ~LDRImage();
    int getWidth();
    int getHeight();
    void toString();
    void showImage();
    bool getExifInfo(const char * name);
    Vec3b getPixel(int x, int y);


private:

    double exposureTime;
    cv::Mat mat;
    double width, height;
    string dateTime;
};

#endif // LDRIMAGE_H
