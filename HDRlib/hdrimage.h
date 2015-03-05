#ifndef HDRIMAGE_H
#define HDRIMAGE_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class HDRImage
{
public:
    HDRImage(Mat mat);
    Mat getMat();
    ~HDRImage();

private:
    Mat image;
};

#endif // HDRIMAGE_H
