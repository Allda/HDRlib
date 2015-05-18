#ifndef HDRIMAGE_H
#define HDRIMAGE_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "rgbe.h"
#include <QString>

using namespace cv;
using namespace std;

class HDRImage
{
public:
    HDRImage(Mat mat);
    HDRImage(QString fileName);
    Mat getMat();
    void saveHDR(QString fileName);
    ~HDRImage();

private:
    Mat image;
};

#endif // HDRIMAGE_H
